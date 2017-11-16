//
//  Player.m
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 2/11/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "Player.h"
#import "Decoder/AudioStreamDecoder.h"
#import "../Streaming/MediaStreamSource.h"
#import "../Models/AudioMetadataInfo.h"

@interface Player()
{
@protected
    AVAudioEngine*          _engine;
    AVAudioPlayerNode*      _playerNode;
    NSError*                _error;
    AVAudioPCMBuffer*       _buffers[2];
    AVAudioPCMBuffer*       _bufferNoSound;     // Used empty sound PCM buffer to interrupt all the scheduling PCM buffers.
    bool                   _buffersPlaying[2];
}

@property (nonatomic, strong) AudioStreamDecoder* decoder;
@property (atomic, assign) AudioPlayerState state;

@property (nonatomic, strong) dispatch_queue_t decode_play_queue;
@property (nonatomic, strong) dispatch_semaphore_t playSyncSemaphore;

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

-(void)prepareForPlay: (int)msecStartAt;
-(void)resetPlay;

@end

@implementation Player

-(instancetype)init
{
    self.state = AudioPlayer_Stopped;
    
    // Create decode and play queue.
    self.decode_play_queue = dispatch_queue_create("audio.decode.and.play.queue", DISPATCH_QUEUE_SERIAL);
    
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
    //[_playerNode setVolume:0.2];
    return self;
}

-(void)play:(id<MediaStreamSourceProtocol> __nonnull)audioStream AudioMetadata:(AudioMetadataInfo* __nullable)metadata At:(int)msecStartAt
{
    PlayerEventCompletionHandler playHandler = ^()
    {
        // Close previous stream and invalidate.
        if(self.audioStream)
            [self.audioStream closeAndInvalidate];
        self.audioStream = audioStream;
        
        // Keep audio stream metadata.
        self.audioMetadata = metadata;
        
        // Open media.
        [self prepareForPlay:msecStartAt];
        
        // Start decoding and playing audio.
        [self decodeAndPlayLoop];
    };
    
    if(_state != AudioPlayer_Stopped)
    {
        [self stop: playHandler];
        /*
        // Stop playing first.
        if([_mediaId compare:mediaId options:NSCaseInsensitiveSearch] != NSOrderedSame)
            [self stop: playHandler];
        else
        {
            // Resume play.
            if(_state == AudioPlayer_Paused)
                [self resume];
        }*/
    }
    else
    {
        playHandler();
    }
}

-(void)pause:(PlayerEventCompletionHandler __nullable)completionHandler
{
    auto state = [self getPlayerState];
    if(state == AudioPlayer_Playing || state == AudioPlayer_Buffering)
    {
        dispatch_semaphore_signal(self.player_pause_semaphore);
        
        // Interrupt scheduled playing buffers with silent audio buffer.
        [_playerNode scheduleBuffer:_bufferNoSound atTime:nil options:AVAudioPlayerNodeBufferInterrupts completionHandler:^{
        }];
    }
}

-(void)resume
{
    auto state = [self getPlayerState];
    if(state == AudioPlayer_Paused)
    {
        // Resume play.
        dispatch_semaphore_signal(self.player_pause_semaphore);
    }
    else
    if(state == AudioPlayer_Stopped)
    {
        [self decodeAndPlayLoop];
    }
}

-(void)seekAtTime:(float)seconds
{
    long packetPosNew = (seconds * 1000.0) / MP3_FRAME_DURATION_MSEC;
    if(packetPosNew == self.PacketPos)
        return;
    
    if([self getPlayerState] != AudioPlayer_Stopped)
    {
        if([self getPlayerState] != AudioPlayer_Paused) // Playing or buffering.
        {
            self.PacketPosNew = packetPosNew;
            self.Seeking = YES;
            
            // Interrupt scheduled buffers.
            [_playerNode scheduleBuffer:_bufferNoSound atTime:nil options:AVAudioPlayerNodeBufferInterrupts completionHandler:^()
            {
                dispatch_semaphore_signal(self.playSyncSemaphore); // Start decoding and playing from the new position.
            }];
        }
        else{ // Paused.
        }
    }
}

-(void)stop:(PlayerEventCompletionHandler __nullable)completionHandler
{
    if(_state == AudioPlayer_Stopped)
        return;
    
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

    if(completionHandler != nil && _state != AudioPlayer_Stopped)
    {
        dispatch_block_notify(self.decodeAndPlayBlock, self.decode_play_queue, ^(){
            completionHandler();
        });
    }
}

-(AudioPlayerState)getPlayerState
{
    return self.state;
}

-(float)getDurationInSec
{
    return self.audioMetadata ? (float)self.audioMetadata.durationSec : 0.0f;
}

-(float)getCurrentTimeInSec
{
    return (float)(self.PacketPos * MP3_FRAME_DURATION_MSEC)/1000.0;
}

-(float)getCurrentTimeInMSec
{
    return (float)(self.PacketPos * MP3_FRAME_DURATION_MSEC);
}

-(void)setVolume:(float)volume
{
    // Set player volume.
    [_playerNode setVolume:volume];
}

-(NSString*)getMediaId
{
    return (self.audioMetadata ? self.audioMetadata.mediaId : nil);
}

-(void)resetPlay
{
    // Reset player node and engine.
    [_playerNode stop];
    [_engine stop];
    
    _buffersPlaying[0] = NO;
    _buffersPlaying[1] = NO;
    
    // Reset decoder.
    [_decoder reset];
    
    self.Seeking = NO;
    self.PacketPos = 0;
    self.PacketPosNew = 0;
    self.packetOffsetDownloading = 0;
    self.isEof = NO;
    self.isWaitingForBuffering = NO;
}

-(void)prepareForPlay: (int)msecStartAt
{
    long packetPostStartAt = msecStartAt > 0 ? (long)(((float)msecStartAt) / MP3_FRAME_DURATION_MSEC) : 0;
    
    // Create and initialize decoder for media.
    self.decoder = [[AudioStreamDecoder alloc] init:self.audioStream WithLinierPCMOutputBufferMsec: 4*27];
    
    // Initialize buffers based on decoder settings.
    AudioStreamBasicDescription outputStreamDesc;
    [_decoder getOutputStreamDescriptor:&outputStreamDesc];
    //AVAudioFormat* audioOutFormat1 = [[AVAudioFormat alloc] initWithStreamDescription:&outputStreamDesc];
    
    AVAudioChannelLayout * chLayout = [[AVAudioChannelLayout alloc] initWithLayoutTag:kAudioChannelLayoutTag_Stereo];
    AVAudioFormat* audioOutFormat = [[AVAudioFormat alloc] initWithCommonFormat:AVAudioPCMFormatFloat32 sampleRate:outputStreamDesc.mSampleRate interleaved: NO channelLayout:chLayout];

    //auto streamDec = [audioOutFormat streamDescription];
    
    _bufferNoSound = [[AVAudioPCMBuffer alloc] initWithPCMFormat:audioOutFormat
                    frameCapacity:outputStreamDesc.mBytesPerFrame*outputStreamDesc.mChannelsPerFrame*(outputStreamDesc.mSampleRate/10.0)];
    _bufferNoSound.frameLength = (long)outputStreamDesc.mSampleRate/10.0; // 10 msec
    _buffers[0] = [[AVAudioPCMBuffer alloc] initWithPCMFormat:audioOutFormat frameCapacity: [_decoder getOutputBufferSize]];
    _buffers[1] = [[AVAudioPCMBuffer alloc] initWithPCMFormat:audioOutFormat frameCapacity:[_decoder getOutputBufferSize]];
    
    _buffersPlaying[0] = _buffersPlaying[1] = NO;
    
     // Initialize playback slider.
     //self.DurationInSec = [_media getMediaDurationInSeconds];
     
     //float packetDurationInMS = 0.0;
     //self.NumPackets =  [_media getNumberPackets:&packetDurationInMS];
     //self.PacketDurationMS = packetDurationInMS;
    self.PacketPos = packetPostStartAt;
}

-(void)updateState:(AudioPlayerState)stateNew
{
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

-(void)decodeAndPlayLoop
{
    __typeof__(self) __weak weakSelf = self;
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
        
        while(1)
        {
            if(weakSelf.Seeking)
            {
                bufferIndex = 0;
                weakSelf.Seeking = NO;
                _buffersPlaying[0] = NO;
                _buffersPlaying[1] = NO;
                dispatch_semaphore_wait(weakSelf.playSyncSemaphore, DISPATCH_TIME_FOREVER);
                
                // Reset decoder.
                [weakSelf.decoder reset];
                
                // Seek stream.
                //[self.audioStream seek:weakSelf.PacketPos];
            }
            else
            {
                if(_buffersPlaying[bufferIndex])
                    dispatch_semaphore_wait(weakSelf.playSyncSemaphore, DISPATCH_TIME_FOREVER);
            }
            
        retry:
            // Pause signaled.
            if(!dispatch_semaphore_wait(weakSelf.player_pause_semaphore, DISPATCH_TIME_NOW))
            {
                // Set paused state.
                [weakSelf updateState:AudioPlayer_Paused];
                
                // Wait until pause semaphore is signaled.
                dispatch_semaphore_wait(weakSelf.player_pause_semaphore, DISPATCH_TIME_FOREVER);
                
                // Set playing state.
                [weakSelf updateState:AudioPlayer_Playing];
            }
            
            // Stop signaled.
            if(!dispatch_semaphore_wait(weakSelf.player_stop_semaphore, DISPATCH_TIME_NOW))
            {
                weakSelf.state = AudioPlayer_Stopped;
                [weakSelf.delegate onPlayEnded];
                break;
            }
            
            // Decode packets to fill audio buffer.
            packetSizeOut = 0;
            OSType error = [weakSelf.decoder decode:weakSelf.PacketPos OutPacketsNum:&packetSizeOut];
            if(error == noErr)
            {
                if(isBuffering)
                {
                    isBuffering = NO;
                    
                    // Set playing state.
                    [weakSelf updateState:AudioPlayer_Playing];
                }
                
                // Fill play buffer with decoded audio data.
                [weakSelf.decoder fillAudioPCMBuffer:_buffers[bufferIndex]];
            }
            else
            {
                // -50 means need more data.
                if(error == -50)
                {
                    NSLog(@"Decoder: need more data");
                    
                    // Set buffering state.
                    [weakSelf updateState:AudioPlayer_Buffering];
                    isBuffering = YES;
                    
                    // Wait 500 msec before retry.
                    [NSThread sleepForTimeInterval:0.5];
                    
                    goto retry;
                    
                    /*
                    // Buffering is not in progress.
                    if(dispatch_semaphore_wait(weakSelf.downloader_buffering, DISPATCH_TIME_NOW))
                    {
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
                    }*/
                }
            }
            
            if(!packetSizeOut)
            {
                // Set ended state.
                [weakSelf updateState:AudioPlayer_Stopped];
                // Reset player.
                // Note: there's might be some audio playing in the same time of reseting play!!!
                [weakSelf resetPlay];
                break; // EOF or error
            }
            
            [_playerNode scheduleBuffer:_buffers[bufferIndex] completionHandler:^()
            {
                if(!weakSelf.Seeking && weakSelf.state != AudioPlayer_Stopped)
                {
                    [weakSelf.delegate onPlayTimeUpdate: (int)(weakSelf.CurrentTimeInMSec)];
                    _buffersPlaying[bufferIndex] = NO;
                    dispatch_semaphore_signal(weakSelf.playSyncSemaphore);
                }
            }];
            
            if(!weakSelf.Seeking)
            {
                _buffersPlaying[bufferIndex] = YES;
            }
            else
            {
                weakSelf.PacketPos = weakSelf.PacketPosNew;
                weakSelf.PacketPosNew = 0;
            }
            
            weakSelf.PacketPos += packetSizeOut;
            //NSLog(@"%ld", weakSelf.PacketPos);
            bufferIndex = (bufferIndex + 1) % buffersCt;
        }
    });
    
    dispatch_async(weakSelf.decode_play_queue, self.decodeAndPlayBlock);
}

/*
-(void)mediaPacketsDownloadStarted:(bool)resumed
{
}

-(void)mediaPacketsDownloadStopped
{
    // If buffering is in progress and stop operation is requested.
    if(self.state == AudioPlayer_Buffering)
        dispatch_semaphore_signal(self.downloader_buffering);
}

-(void)mediaPacketsDownloadPaused
{
    // If buffering is in progress and pause operation is requested.
    if(self.state == AudioPlayer_Buffering)
        dispatch_semaphore_signal(self.downloader_buffering);
}

-(void)mediaPacketsDownloadProgress:(long)packetOffset packetsCt:(int)packetsCt isEof:(bool)isEof
{
    self.packetOffsetDownloading = packetOffset;
    self.isEof = isEof;
    
    //if(isEof || (packetOffset - self.PacketPos) >= 100){
    if(isEof || self.state == AudioPlayer_Buffering)
    {
        dispatch_semaphore_signal(self.downloader_buffering);
    }
}
*/
@end
