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

/* Playing buffer info structure. */
typedef struct PlayingBufferInfoStruct{
    bool isPlaying;
    long packetPos;
    UInt32 packetsSize;
} PlayingBufferInfo, * PPlayingBufferInfoStruct;

@interface Player()
{
@protected
    AVAudioEngine*          _engine;
    AVAudioPlayerNode*      _playerNode;
    NSError*                _error;
    AVAudioPCMBuffer*       _buffers[2];
    AVAudioPCMBuffer*       _bufferNoSound;     // Used empty sound PCM buffer to interrupt all the scheduling PCM buffers.
    PlayingBufferInfo       _buffersPlaying[2];
}

@property (nonatomic, strong) AudioStreamDecoder* decoder;
@property (atomic, assign) AudioPlayerState state;

@property (nonatomic, strong) dispatch_queue_t decode_play_queue;
@property (nonatomic, strong) dispatch_semaphore_t playSyncSemaphore;

@property (atomic, assign) bool Seeking;                    // Indicates seek operation.
@property (atomic, assign) long SeekPacketPos;              // Seek packet position.

@property (atomic, assign) long PacketPos;                  // Decoding audio packet position.
@property (atomic, assign) long PacketPosPlaying;           // Playing audio packet position.
@property (atomic, assign) UInt32 PlayingTimeMsec;          // Current playing time in milliseconds.

@property (atomic, assign) long isEof;
@property (atomic, assign) bool isWaitingForBuffering;

@property (nonatomic, strong) dispatch_block_t decodeAndPlayBlock;
@property (nonatomic, strong) dispatch_semaphore_t downloader_buffering;
@property (nonatomic, strong) dispatch_semaphore_t player_stop_semaphore;
@property (nonatomic, strong) dispatch_semaphore_t player_pause_semaphore;
@property (nonatomic, strong) dispatch_semaphore_t player_resume_semaphore;

-(void)prepareForPlay: (int)msecStartAt;
-(void)resetPlay;
-(void)resetPlayBufferInfo;

@end

@implementation Player

-(instancetype)init
{
    self = [super init];
    
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
    [_playerNode setVolume:0.0];
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
        
        //__typeof__(self) __weak weakSelf = self;
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
    // TODO: Validate seconds parameter.
    
    // Convert time sec into packet offset.
    UInt32 seekTimeMs = seconds * 1000.0f;
    long packetPosNew = [self.audioStream timeMsecOffset2PacketOffset:seekTimeMs];
    if(packetPosNew == self.PacketPos)
        return;
    
    if([self getPlayerState] != AudioPlayer_Stopped)
    {
        if([self getPlayerState] != AudioPlayer_Paused) // Playing or buffering.
        {
            self.SeekPacketPos = packetPosNew;
            self.Seeking = YES;
            
            // Interrupt scheduled buffers.
            __typeof__(self) __weak weakSelf = self;
            [_playerNode scheduleBuffer:_bufferNoSound atTime:nil options:AVAudioPlayerNodeBufferInterrupts completionHandler:^()
            {
                // Update playing time.
                weakSelf.PlayingTimeMsec = seekTimeMs;
                [weakSelf.delegate onPlayTimeUpdate:seekTimeMs];
                
                dispatch_semaphore_signal(self.playSyncSemaphore); // Start decoding and playing from the new position.
            }];
        }
        else // Paused.
        {
            self.PlayingTimeMsec = seekTimeMs;
            self.SeekPacketPos = packetPosNew;
            self.Seeking = YES;
        }
        
        [self.delegate onPlayTimeUpdate: seekTimeMs];
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
    return (float)(self.PlayingTimeMsec/1000.0);
    //return (float)(self.PacketPos * MP3_FRAME_DURATION_MSEC)/1000.0;
}

-(float)getCurrentTimeInMSec
{
    return (float)(self.PlayingTimeMsec);
    //return (float)(self.PacketPos * MP3_FRAME_DURATION_MSEC);
}

-(void)setVolume:(float)volume
{
    // Set player volume.
    [_playerNode setVolume:volume];
}

-(float)getVolume
{
    // Get player node volume.
    return _playerNode.volume;
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
    
    _buffersPlaying[0].isPlaying = _buffersPlaying[1].isPlaying = NO;
    _buffersPlaying[0].packetPos = _buffersPlaying[1].packetPos = 0;
    _buffersPlaying[0].packetsSize = _buffersPlaying[1].packetsSize = 0;
    
    // Reset decoder.
    [_decoder reset];
    
    self.Seeking = NO;
    self.SeekPacketPos = 0;
    self.PacketPos = 0;
    self.PlayingTimeMsec = 0;
    self.PacketPosPlaying = 0;
    self.isEof = NO;
    self.isWaitingForBuffering = NO;
}

-(void)prepareForPlay: (int)msecStartAt
{
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
    
    // Reset play buffer infos.
    [self resetPlayBufferInfo];
    
    // Reset play parameters.
    [self resetPlay];
    
    // Seek at start position.
    if(msecStartAt > 0)
        [self seekAtTime:(float)(msecStartAt / 1000.0f)];
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

-(void)resetPlayBufferInfo
{
    _buffersPlaying[0].isPlaying = _buffersPlaying[1].isPlaying = NO;
    _buffersPlaying[0].packetPos = _buffersPlaying[1].packetPos = 0;
    _buffersPlaying[0].packetsSize = _buffersPlaying[1].packetsSize = 0;
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
        DecodedAudioInfo info = {0};
        //memset(&info, sizeof(info), 0);
        
        bool isBuffering = true;
        weakSelf.state = AudioPlayer_Buffering;
        
        while(YES)
        {
            // Check for seek operation.
            if(weakSelf.Seeking)
            {
                bufferIndex = 0;
                weakSelf.Seeking = NO;
                
                // Reset play buffer infos.
                [weakSelf resetPlayBufferInfo];
                
                dispatch_semaphore_wait(weakSelf.playSyncSemaphore, DISPATCH_TIME_FOREVER);
                
                // Update decode packet position.
                weakSelf.PacketPos = weakSelf.SeekPacketPos;
                weakSelf.SeekPacketPos = 0;
                
                // Reset decoder.
                [weakSelf.decoder reset];
            }
            else
            {
                if(_buffersPlaying[bufferIndex].isPlaying)
                {
                    dispatch_semaphore_wait(weakSelf.playSyncSemaphore, DISPATCH_TIME_FOREVER);
                }
            }
            
        retry:
            // Pause signaled.
            if(!dispatch_semaphore_wait(weakSelf.player_pause_semaphore, DISPATCH_TIME_NOW))
            {
                // Set paused state.
                [weakSelf updateState:AudioPlayer_Paused];
                
                // Wait until pause semaphore is signaled.
                dispatch_semaphore_wait(weakSelf.player_pause_semaphore, DISPATCH_TIME_FOREVER);
                
                // If there was seek operation while pause.
                if(weakSelf.Seeking)
                {
                    // Update decode packet position.
                    //weakSelf.PacketPos = weakSelf.SeekPacketPos;
                    //weakSelf.SeekPacketPos = 0;
                    continue;
                }
                
                // Reset decoder.
                [weakSelf.decoder reset];
                
                // Update decode packet position to last playing packet position.
                weakSelf.PacketPos = self.PacketPosPlaying;
                
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
            
            // Decode audio packets.
            if([weakSelf.decoder decode:weakSelf.PacketPos AndWriteResultInto:info])
            {
                // Fill play buffer with decoded audio data.
                [weakSelf.decoder fillAudioPCMBuffer:_buffers[bufferIndex]];
            }
            else
            {
                // Need more data.
                if(info.status == Decoder_ErrorNeedMoreData)
                {
                    NSLog(@"Decoder: need more data");
                    
                    // Set buffering state.
                    [weakSelf updateState:AudioPlayer_Buffering];
                    isBuffering = YES;
                    
                    // Wait 500 msec before retry.
                    [NSThread sleepForTimeInterval:0.5];
                    
                    goto retry;
                }
            }
            
            if(info.isEof == YES ||
               info.status == Decoder_ErrorUnavailableData ||
               info.status == Decoder_UnknownError )
            {
                // Set ended state.
                [weakSelf updateState:AudioPlayer_Stopped];
                // Reset player.
                // Note: there's might be some audio playing in the same time of reseting play!!!
                [weakSelf resetPlay];
                break; // EOF or error
            }
            
            long packetPosPlaying = weakSelf.PacketPos;
            PlayingBufferInfo* buffersPlaying = &_buffersPlaying[0];
            
            // TODO: the logic below is not 100% correct, I encountered problem when I was playing with seeking operation!
            assert(buffersPlaying[0].isPlaying == NO || buffersPlaying[1].isPlaying == NO);
            
            [_playerNode scheduleBuffer:_buffers[bufferIndex] completionHandler:^()
            {
                NSLog(@"%ld", packetPosPlaying);
                buffersPlaying[bufferIndex].isPlaying = NO;
                
                if(!weakSelf.Seeking && weakSelf.state != AudioPlayer_Stopped)
                {
                    // Seed playing packet position.
                    int nextBufferIndex = (bufferIndex + 1) % buffersCt;
                    if(buffersPlaying[nextBufferIndex].isPlaying)
                        weakSelf.PacketPosPlaying = buffersPlaying[nextBufferIndex].packetPos;
                    
                    weakSelf.PlayingTimeMsec += info.durationMsec;
                    [weakSelf.delegate onPlayTimeUpdate: weakSelf.PlayingTimeMsec];
                    //buffersPlaying[bufferIndex].isPlaying = NO;
                    
                    dispatch_semaphore_signal(weakSelf.playSyncSemaphore);
                }
            }];
            
            if(isBuffering)
            {
                isBuffering = NO;
                
                // Set playing state.
                [weakSelf updateState:AudioPlayer_Playing];
            }
            
            if(!weakSelf.Seeking)
            {
                _buffersPlaying[bufferIndex].isPlaying = YES;
                _buffersPlaying[bufferIndex].packetPos = weakSelf.PacketPos;
                _buffersPlaying[bufferIndex].packetsSize = info.numPackets;
                
                // Set playing packet position.
                int prevBufferIndex = bufferIndex - 1;
                if(prevBufferIndex == -1)
                    prevBufferIndex = buffersCt - 1;
                
                if(!_buffersPlaying[prevBufferIndex].isPlaying)
                {
                    weakSelf.PacketPosPlaying = weakSelf.PacketPos;
                    // Update play time.
                    [weakSelf.delegate onPlayTimeUpdate: (int)(weakSelf.getCurrentTimeInMSec)];
                }
                
                // Reset play sync event if signaled.
                if(_buffersPlaying[0].isPlaying == YES && _buffersPlaying[1].isPlaying == YES)
                {
                    dispatch_semaphore_wait(weakSelf.playSyncSemaphore, DISPATCH_TIME_NOW);
                }
                
                // Advance decode packet position.
                weakSelf.PacketPos += info.numPackets;
            }
            else
            {
                // Update decode packet position.
                //weakSelf.PacketPos = weakSelf.SeekPacketPos;
                //weakSelf.SeekPacketPos = 0;
                continue;
            }
            
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
