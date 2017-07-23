//
//  Player.m
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 2/11/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "Player.h"
#import "MediaStreamDownloader.h"
#import "MediaFileStream.h"
#import "Mp3Decoder.h"

@interface Player() <MediaStreamReaderProtocol>
{
@protected
    AVAudioEngine*          _engine;
    AVAudioPlayerNode*      _playerNode;
    NSError*                _error;
    AVAudioPCMBuffer*       _buffers[2];
    AVAudioPCMBuffer*       _bufferNoSound; // Used empty sound PCM buffer to interrupt all the scheduling PCM buffers.
    bool                    _buffersPlaying[2];
}

@property (nonatomic, copy) NSString* mediaId;
@property (nonatomic, copy) NSString* sessionKey;
@property (atomic, assign) AudioPlayerState state;

@property (nonatomic, strong) dispatch_queue_t decode_play_queue;
@property (nonatomic, strong) dispatch_semaphore_t playSyncSemaphore;
@property (nonatomic, strong) id<MediaStreamProtocol> media;
@property (nonatomic, strong) MediaStreamDownloader* mediaStream;
@property (nonatomic, strong) Mp3Decoder* decoder;

@property (atomic, assign) bool Seeking;
@property (atomic, assign) long PacketPosNew;
@property (atomic, assign) long PacketPos;
@property (atomic, assign) long packetOffsetDownloading;
@property (atomic, assign) long isEof;
@property (atomic, assign) bool isWaitingForBuffering;

@property (nonatomic, strong) dispatch_block_t decodeAndPlayBlock;
@property (nonatomic, strong) dispatch_semaphore_t downloader_buffering;
@property (nonatomic, strong) dispatch_semaphore_t player_stop_semaphore;
@property (nonatomic, strong) dispatch_semaphore_t player_pause_semaphore;
@property (nonatomic, strong) dispatch_semaphore_t player_resume_semaphore;

@end

@implementation Player

//-(void)dealloc{
//    [super dealloc];
//    
//    dispatch_release(self.decode_play_queue);
//    self.decode_play_queue = nil;
//    
//    self.mediaId = nil;
//    self.sessionKey = nil;
//    
//    [_engine release];
//    _engine = nil;
//    [_playerNode release];
//    _playerNode = nil;
//    
//    // Stop stream downloader.
//    [self.mediaStream stop];
//    [self.mediaStream release];
//    self.mediaStream = nil;
//    
//    [self.decoder release];
//    self.decoder = nil;
//    
//    [_bufferNoSound release];
//    _bufferNoSound = nil;
//    [_buffers[0] release];
//    _buffers[0] = nil;
//    [_buffers[1] release];
//    _buffers[1] = nil;
//    
//    if(self.playSyncSemaphore)
//        dispatch_release(self.playSyncSemaphore);
//    self.playSyncSemaphore = nil;
//    
//    if(self.downloader_buffering)
//        dispatch_release(self.downloader_buffering);
//    self.downloader_buffering = nil;
//    
//    if(self.player_pause_semaphore)
//        dispatch_release(self.player_pause_semaphore);
//    self.player_pause_semaphore = nil;
//    
//    if(self.player_resume_semaphore)
//        dispatch_release(self.player_resume_semaphore);
//    self.player_resume_semaphore = nil;
//
//    if(self.player_stop_semaphore)
//        dispatch_release(self.player_stop_semaphore);
//    self.player_stop_semaphore = nil;
//}

-(Player*)init:(NSString*)sessionKey{
    self.state = AudioPlayer_Stopped;
    self.decode_play_queue = dispatch_queue_create("mp3-audio-decode-and-playe-queue", DISPATCH_QUEUE_SERIAL);
    self.sessionKey = sessionKey;
    
    // Create audio engine and attach player node.
    _engine = [[AVAudioEngine alloc] init];
    
    // Create player node.
    _playerNode = [[AVAudioPlayerNode alloc] init];
    
    // Attach node to engine.
    [_engine attachNode:_playerNode];
    
    AVAudioChannelLayout * chLayout = [[AVAudioChannelLayout alloc] initWithLayoutTag:kAudioChannelLayoutTag_Stereo];
    AVAudioFormat* audioFormat = [[AVAudioFormat alloc] initWithCommonFormat:AVAudioPCMFormatFloat32 sampleRate:44100 interleaved: NO channelLayout:chLayout];
    
    // Use default audio mixer.
    [_engine connect:_playerNode to:[_engine mainMixerNode] format: audioFormat];
    
    // Set player volume.
    [_playerNode setVolume:0.2];
    return self;
}

-(void)play:(NSString*)mediaId At:(int)msecStartAt{
    PlayerEventCompletionHandler playHandler = ^(){
        self.mediaId = mediaId;
        // Open media.
        [self openMedia:msecStartAt];
        // Start decoding and playing audio.
        [self decodeAndPlayLoop];
    };
    
    if(_state != AudioPlayer_Stopped){
        // Stop playing first.
        if([_mediaId compare:mediaId options:NSCaseInsensitiveSearch] != NSOrderedSame)
            [self stop: playHandler];
        else{
            // Resume play.
            if(_state == AudioPlayer_Paused)
                [self resume];
        }
    }
    else{
        playHandler();
    }
    
//    // Start playing.
//    if(_state == AudioPlayer_Stopped){
//        self.mediaId = mediaId;
//        // Open media.
//        [self openMedia:msecStartAt];
//        // Start decoding and playing audio.
//        [self decodeAndPlayLoop];
//    }
}

-(void)pause:(PlayerEventCompletionHandler __nullable)completionHandler{
    auto state = [self getPlayerState];
    if(state == AudioPlayer_Playing || state == AudioPlayer_Buffering){
        // Pause stream downloader.
        [self.mediaStream pause];
        
        dispatch_semaphore_signal(self.player_pause_semaphore);
        
        // Interrupt scheduled playing buffers with silent audio buffer.
        [_playerNode scheduleBuffer:_bufferNoSound atTime:nil options:AVAudioPlayerNodeBufferInterrupts completionHandler:^(){
        }];
    }
}

-(void)resume{
    if([self getPlayerState] == AudioPlayer_Paused){
        // Resume stream downloader.
        [self.mediaStream start:-1];
        // Resume play.
        dispatch_semaphore_signal(self.player_pause_semaphore);
    }
}

-(void)seekAtTime:(float)seconds{
    long packetPosNew = (seconds * 1000.0) / MP3_FRAME_DURATION_MSEC;
    if(packetPosNew == self.PacketPos)
        return;
    
    if([self getPlayerState] != AudioPlayer_Stopped){
        if([self getPlayerState] != AudioPlayer_Paused){ // Playing or buffering.
            self.PacketPosNew = packetPosNew;
            self.Seeking = YES;
            
            // Interrupt scheduled buffers.
            [_playerNode scheduleBuffer:_bufferNoSound atTime:nil options:AVAudioPlayerNodeBufferInterrupts completionHandler:^(){
                dispatch_semaphore_signal(self.playSyncSemaphore); // Start decoding and playing from the new position.
            }];
        }
        else{ // Paused.
            
        }
    }
}

-(void)stop:(PlayerEventCompletionHandler __nullable)completionHandler{
    if(_state != AudioPlayer_Stopped){
        // Stop stream downloader.
        [self.mediaStream stop];
        
        // Resume play.
        if(_state == AudioPlayer_Paused)
            dispatch_semaphore_signal(self.player_pause_semaphore);
        
        // Signal buffering event.
        if(_state == AudioPlayer_Buffering)
            dispatch_semaphore_signal(self.downloader_buffering);
        
        // Stop play.
        dispatch_semaphore_signal(self.player_stop_semaphore);
        
        // Fading out effect ???
        // Interrupt scheduled playing buffers with silent audio buffer.
        [_playerNode scheduleBuffer:_bufferNoSound atTime:nil options:AVAudioPlayerNodeBufferInterrupts completionHandler:^(){
        }];
        
        if(completionHandler != nil && _state != AudioPlayer_Stopped){
            dispatch_block_notify(self.decodeAndPlayBlock, self.decode_play_queue, ^(){
                completionHandler();
            });
        }
    }
}

-(AudioPlayerState)getPlayerState{
    return self.state;
}

-(float)getDurationInSec{
    return [self.media getMediaDurationInSeconds];
}

-(float)getCurrentTimeInSec{
    return (float)(self.PacketPos*MP3_FRAME_DURATION_MSEC)/1000.0;
}

-(float)getCurrentTimeInMSec{
    return (float)(self.PacketPos*MP3_FRAME_DURATION_MSEC);
}

-(void)setVolume:(float)volume{
    // Set player volume.
    [_playerNode setVolume:volume];
}

-(NSString*)getMediaId{
    return self.mediaId;
}

-(void)openMedia: (int)msecStartAt{
    /*
     // Close previous media.
     [_media close:_token];
     
     // Open media stream.
     _streamId = [_media open:_token Media:[NSString stringWithFormat:@"%@/02 Stressed Out.mp3", [[NSBundle mainBundle] resourcePath]] WithCallback:(StreamReadCallbackProc)&MyStreamReadProc];
     */
    
    // Clean up.
#if !__has_feature(objc_arc)
    auto decoder = self.decoder;
    self.decoder = nil;
    [decoder dealloc];
    
    auto mediaStream = self.mediaStream;
    self.mediaStream = nil;
    self.media = nil;
    [mediaStream dealloc];
    
    if(_bufferNoSound){
        [_bufferNoSound dealloc];
        _bufferNoSound = nil;
    }
    
    if(_buffers[0]){
        [_buffers[0] dealloc];
        [_buffers[1] dealloc];
        _buffers[0] = nil;
        _buffers[1] = nil;
    }
#endif
    
    long packetPostStartAt = msecStartAt > 0 ? (long)(((float)msecStartAt) / MP3_FRAME_DURATION_MSEC) : 0;
    
    // Use media file stream.
    //self.media = [MediaFileStream alloc];
    self.mediaStream = [[MediaStreamDownloader alloc] init:self];
    self.media = self.mediaStream;
    
    // Open media.
    [self.mediaStream open:_sessionKey Media:_mediaId WithCallback:nil];
    // Start media downloading.
    [self.mediaStream start:packetPostStartAt];
    // Create and initialize decoder for media.
    self.decoder = [[Mp3Decoder alloc] init:_media OutputBufferMsec:50]; // Keep decoding audio for optimal msec chunks.
    
    // Initialize buffers based on decoder settings.
    AudioStreamBasicDescription outputStreamDesc;
    [_decoder getOutputStreamDescriptor:&outputStreamDesc];
    //AVAudioFormat* audioOutFormat1 = [[AVAudioFormat alloc] initWithStreamDescription:&outputStreamDesc];
    
    AVAudioChannelLayout * chLayout = [[AVAudioChannelLayout alloc] initWithLayoutTag:kAudioChannelLayoutTag_Stereo];
    AVAudioFormat* audioOutFormat = [[AVAudioFormat alloc] initWithCommonFormat:AVAudioPCMFormatFloat32 sampleRate:44100 interleaved: NO channelLayout:chLayout];

    //auto streamDec = [audioOutFormat streamDescription];
    
    _bufferNoSound = [[AVAudioPCMBuffer alloc] initWithPCMFormat:audioOutFormat
                    frameCapacity:outputStreamDesc.mBytesPerFrame*outputStreamDesc.mChannelsPerFrame*441];
    _bufferNoSound.frameLength = 441; // 10 msec
    _buffers[0] = [[AVAudioPCMBuffer alloc] initWithPCMFormat:audioOutFormat frameCapacity: [_decoder getOutputBufferSize]];
    _buffers[1] = [[AVAudioPCMBuffer alloc] initWithPCMFormat:audioOutFormat frameCapacity:[_decoder getOutputBufferSize]];
    
    _buffersPlaying[0] = _buffersPlaying[1] = NO;
    
     // Initialize playback slider.
     //self.DurationInSec = [_media getMediaDurationInSeconds];
     
    // float packetDurationInMS = 0.0;
     //self.NumPackets =  [_media getNumberPackets:&packetDurationInMS];
     //self.PacketDurationMS = packetDurationInMS;
    self.PacketPos = packetPostStartAt;
    
#if !__has_feature(objc_arc)
    //[audioOutFormat dealloc];
    //audioOutFormat = nil;
    //[chLayout dealloc];
    //chLayout = nil;
#endif
}

-(void)updateState:(AudioPlayerState)stateNew {
    if(self.state == stateNew)
        return;
    
    AudioPlayerState stateOld = self.state;
    self.state = stateNew;
    
    switch (stateNew) {
        case AudioPlayer_Stopped:
            [self.delegate onPlayEnded];
            break;
        case AudioPlayer_Playing:
            if(stateOld == AudioPlayer_Buffering)
               [self.delegate onBufferingEnded];
            [self.delegate onPlayStarted:(self.PacketPos > 0)];
            break;
        case AudioPlayer_Buffering:
            [self.delegate onBufferingStarted];
            break;
        case AudioPlayer_Paused:
            [self.delegate onPaused];
            break;
        default:
            break;
    }
}

-(void)decodeAndPlayLoop{
#if __has_feature(objc_arc)
    __weak __typeof__(self) weakSelf = self;
#else
    auto weakSelf = self;
    if(self.playSyncSemaphore)
        dispatch_release(self.playSyncSemaphore);
    self.playSyncSemaphore = nil;
    
    if(self.downloader_buffering)
        dispatch_release(self.downloader_buffering);
    self.downloader_buffering = nil;
    
    if(self.player_pause_semaphore)
        dispatch_release(self.player_pause_semaphore);
    self.player_pause_semaphore = nil;
    
    if(self.player_resume_semaphore)
        dispatch_release(self.player_resume_semaphore);
    self.player_resume_semaphore = nil;
    
    if(self.player_stop_semaphore)
        dispatch_release(self.player_stop_semaphore);
    self.player_stop_semaphore = nil;
#endif
    
    self.playSyncSemaphore = dispatch_semaphore_create(0);
    self.downloader_buffering = dispatch_semaphore_create(0);
    self.player_pause_semaphore = dispatch_semaphore_create(0);
    self.player_resume_semaphore = dispatch_semaphore_create(0);
    self.player_stop_semaphore = dispatch_semaphore_create(0);
    
    // Make semaphore signal to proceed first loop.
    dispatch_semaphore_signal(weakSelf.playSyncSemaphore);
    
    NSError* error = nil;
    [_engine startAndReturnError:&error];
    [_playerNode play];
    
    self.decodeAndPlayBlock =
    dispatch_block_create(DISPATCH_BLOCK_ASSIGN_CURRENT, ^{
        int bufferIndex = 0, buffersCt = 2;
        UInt32 packetSizeOut = 0;
        bool isBuffering = true;
        weakSelf.state = AudioPlayer_Buffering;
        
        while(YES){
            if(weakSelf.Seeking){
                bufferIndex = 0;
                weakSelf.Seeking = NO;
                _buffersPlaying[0] = _buffersPlaying[1] = NO;
                dispatch_semaphore_wait(weakSelf.playSyncSemaphore, DISPATCH_TIME_FOREVER);
                
                // Seek downloading.
                [self.mediaStream seek:weakSelf.PacketPos];
            }
            else
                if(_buffersPlaying[bufferIndex])
                    dispatch_semaphore_wait(weakSelf.playSyncSemaphore, DISPATCH_TIME_FOREVER);
            
        retry:
            // Pause signaled.
            if(!dispatch_semaphore_wait(weakSelf.player_pause_semaphore, DISPATCH_TIME_NOW)){
                //                // Play paused
                //                weakSelf.state = AudioPlayer_Paused;
                //                if(weakSelf.delegate)
                //                    [weakSelf.delegate onPaused];
                
                // Set paused state.
                [weakSelf updateState:AudioPlayer_Paused];
                
                // Wait until pause semaphore is signaled.
                dispatch_semaphore_wait(weakSelf.player_pause_semaphore, DISPATCH_TIME_FOREVER);
                
                // Set playing state.
                [weakSelf updateState:AudioPlayer_Playing];
                
                //                // Play resumed.
                //                if(weakSelf.delegate)
                //                    [weakSelf.delegate onPlayStarted:YES];
                //                weakSelf.state = AudioPlayer_Playing;
            }
            
            // Stop signaled.
            if(!dispatch_semaphore_wait(weakSelf.player_stop_semaphore, DISPATCH_TIME_NOW)){
                weakSelf.state = AudioPlayer_Stopped;
                [weakSelf.delegate onPlayEnded];
                break;
            }
            
            // Decode packets to fill audio buffer.
            packetSizeOut = 0;
            int error = [weakSelf.decoder decode:weakSelf.PacketPos decodedPacketSizeOut:&packetSizeOut];
            if(error == noErr){
                if(isBuffering){
                    isBuffering = NO;
                    
                    // Set playing state.
                    [weakSelf updateState:AudioPlayer_Playing];
                    
                    //                    // Invoke buffering ended callback.
                    //                    [weakSelf.delegate onBufferingEnded];
                    //                    weakSelf.state = AudioPlayer_Playing;
                    //                    // Play started/resumed.
                    //                    if(weakSelf.delegate)
                    //                        [weakSelf.delegate onPlayStarted:(weakSelf.PacketPos > 0)];
                }
                
                // Fill play buffer with decoded audio data.
                [weakSelf.decoder fillAudioPCMBuffer:_buffers[bufferIndex]];
            }
            else{
                if(error == -50){ // -50 is need more data.
                    NSLog(@"More data decoder");
                    // Buffering is not in progress.
                    if(dispatch_semaphore_wait(weakSelf.downloader_buffering, DISPATCH_TIME_NOW)){
                        // Set buffering state.
                        [weakSelf updateState:AudioPlayer_Buffering];
                        
                        // Invoke buffering started callback.
                        //[weakSelf.delegate onBufferingStarted];
                        //weakSelf.state = AudioPlayer_Buffering;
                        
                        isBuffering = YES;
                        // Wait while buffering.
                        dispatch_semaphore_wait(weakSelf.downloader_buffering, DISPATCH_TIME_FOREVER);
                        // Retry decode operation.
                        goto retry;
                    }
                }
            }
            
            if(packetSizeOut == 0){
                //                // Invoke buffering started callback.
                //                [weakSelf.delegate onPlayEnded];
                //                weakSelf.state = AudioPlayer_Stopped;
                
                // Set ended state.
                [weakSelf updateState:AudioPlayer_Stopped];
                break; // EOF or error
            }
            
            [_playerNode scheduleBuffer:_buffers[bufferIndex] completionHandler:^(){
                if(!weakSelf.Seeking && weakSelf.state != AudioPlayer_Stopped ){
                    [weakSelf.delegate onPlayTimeUpdate: (int)(weakSelf.CurrentTimeInMSec)];
                    _buffersPlaying[bufferIndex] = NO;
                    dispatch_semaphore_signal(weakSelf.playSyncSemaphore);
                }
            }];
            
            if(!weakSelf.Seeking)
                _buffersPlaying[bufferIndex] = YES;
            else{
                weakSelf.PacketPos = weakSelf.PacketPosNew;
                weakSelf.PacketPosNew = 0;
            }
            weakSelf.PacketPos += packetSizeOut;
            bufferIndex = (bufferIndex + 1) % buffersCt;
        }
    });
    
    dispatch_async(weakSelf.decode_play_queue, self.decodeAndPlayBlock);
}

-(void)mediaPacketsDownloadStarted:(bool)resumed{
}

-(void)mediaPacketsDownloadStopped{
    // If buffering is in progress and stop operation is requested.
    if(self.state == AudioPlayer_Buffering)
        dispatch_semaphore_signal(self.downloader_buffering);
}

-(void)mediaPacketsDownloadPaused{
    // If buffering is in progress and pause operation is requested.
    if(self.state == AudioPlayer_Buffering)
        dispatch_semaphore_signal(self.downloader_buffering);
}

-(void)mediaPacketsDownloadProgress:(long)packetOffset packetsCt:(int)packetsCt isEof:(bool)isEof{
    self.packetOffsetDownloading = packetOffset;
    self.isEof = isEof;
    
    //if(isEof || (packetOffset - self.PacketPos) >= 100){
    if(isEof || self.state == AudioPlayer_Buffering){
        dispatch_semaphore_signal(self.downloader_buffering);
    }
}

@end
