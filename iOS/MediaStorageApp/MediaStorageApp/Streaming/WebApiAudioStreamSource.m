//
//  WebApiAudioStreamReader.m
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 10/7/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "WebApiAudioStreamSource.h"
#import "WebApiAudioPackatesDownloader.h"
#import "AudioPacketsDownloaderProtocol.h"

@interface WebApiAudioStreamSource()<AudioPacketsDownloaderProtocol>
{
    AudioStreamBasicDescription     _streamBasicDesc;
    AudioStreamPacketDescription*   _streamPacketsDesc;
    NSRange                         _rangeReadPackets;
    AudioPacketsReadCallback       _streamReadCallback;
    NSCondition*                    _lockDownloadSync;
}

@property (atomic, assign) BOOL packetsReadIsInProgress;
@property (atomic, assign) long lastReadOffset; // Last successful read packet offset.
@property (atomic, assign) long currentDownloadedOffset;
@property (atomic, assign) long bufferingPacketsCount;
@property (nonatomic, strong) WebApiAudioPackatesDownloader* downloader;

-(bool)waitForPacketsReadToComplete:(NSRange)range;

-(bool)needToFillBuffer;

@end

@implementation WebApiAudioStreamSource

-(instancetype)init:(StreamingSession* __weak)session MediaId:(NSString*)mediaId StreamDescription:(const AudioStreamBasicDescription*)streamDesc WithSettings:(WebApiAudioStreamReaderSettings*)settings
{
    assert(streamDesc);
    self.downloader = [[WebApiAudioPackatesDownloader alloc] init:self Session:session MediaId:mediaId];
    self.packetsReadIsInProgress = NO;
    self.lastReadOffset = 0;
    self.currentDownloadedOffset = 0;
    self.bufferingPacketsCount = 500;
    
    // Copy stream basic description info object.
    memcpy(&_streamBasicDesc, streamDesc, sizeof(AudioStreamBasicDescription));
    
    _lockDownloadSync = [[NSCondition alloc] init];
    return [super init];
}

-(UInt32)getPacketSizeInBytes
{
    // Note: find better solution !!!
    return 1052; // kAudioFilePropertyPacketSizeUpperBound
}

-(UInt32)getNumberOfPackets
{
    return [self.downloader getNumberOfPackets];
}

-(bool)getStreamDescription:(AudioStreamBasicDescription*)streamDescOut
{
    NSAssert(streamDescOut != nil, @"Stream description object can't be nil!");
    memcpy(streamDescOut, &_streamBasicDesc, sizeof(AudioStreamBasicDescription));
    return YES;
}

-(AudioStreamPacketsInfo*)readPackets:(NSRange)range
{
    assert(range.length > 0);
    assert(range.location >= 0);
    
    AudioStreamPacketsInfo* packetsInfo = [[AudioStreamPacketsInfo alloc] init: [self getPacketSizeInBytes]*(UInt32)range.length];
    if([self readPackets:range InPacketsInfoObject:packetsInfo])
        return packetsInfo;
    return nil;
}

-(bool)readPackets:(NSRange)range InPacketsInfoObject:(AudioStreamPacketsInfo*)packetsInfo
{
    assert(range.length > 0);
    assert(range.location >= 0);
    
    BOOL ret = NO;
    // Check availability of audio packets.
    if([self.downloader checkAudioPacketsAvailability:range])
    {
        UInt32 packetsDataSize = 0;
        // Copy audio packets.
        ret = [self.downloader copyAudioPacketsData:range PacketsInfo:packetsInfo OutSize:&packetsDataSize];
    }
    else
    {
        // Lock section.
        [_lockDownloadSync lock];
        
        // Wait for previous read request to complete.
        bool r = [self waitForPacketsReadToComplete:range];
        if(!r)
        {
            // Download requesting packets.
            self.packetsReadIsInProgress = YES;
            _rangeReadPackets = range;
            _streamReadCallback = nil;
            
            [self.downloader start:range.location];
            //NSLog(@"read packets: %ld", range.location);
        }
        
        // Unlock section.
        [_lockDownloadSync unlock];
    }
    
    if(ret)
    {
        // Lock section.
        [_lockDownloadSync lock];
        
        self.lastReadOffset = (range.location + range.length);
        
        // Start buffering packets.
        if([self.downloader checkIfPaused] && [self needToFillBuffer])
        {
            NSLog(@"Resume buffering");
            [self.downloader resume];
        }
        
        // Unlock section.
        [_lockDownloadSync unlock];
    }
    
    return ret;
}

//-(bool)readPackets:(NSRange)range WithCallback:(AudioPacketsReadCallback)callback
//{
//    assert(range.length > 0);
//    assert(range.location >= 0);
//    
//    BOOL ret = NO;
//    
//    // Check availability of audio packets.
//    if( [self.downloader checkAudioPacketsAvailability:range] )
//    {
//        // Read packets into buffer.
//        callback([self readPackets:range]);
//        
////        if( !self.packetsReadIsInProgress &&
////           ![self.downloader checkAudioPacketsAvailability: NSMakeRange(range.location + range.length, self.bufferingPacketsCount)] && )
//    }
//    else
//    {
//        // Only one read request at the time!
//        if(self.packetsReadIsInProgress)
//            return ret; // Packets read is already in progress.
//
//        self.packetsReadIsInProgress = YES;
//        _rangeReadPackets = range;
//        _streamReadCallback = callback;
//        [self.downloader start:range.location];
//        ret = YES;
//    }
//    
//    return ret;
//}

-(void)closeAndInvalidate
{
    if(self.downloader != nil)
    {
        [self.downloader stop];
        self.downloader = nil;
    }
    
    self.packetsReadIsInProgress = NO;
    self.lastReadOffset = 0;
    self.currentDownloadedOffset = 0;
}

#pragma mark - protected methods.
-(bool)waitForPacketsReadToComplete:(NSRange)range
{
    if(self.packetsReadIsInProgress &&
       _rangeReadPackets.location == range.location &&
       _rangeReadPackets.length == range.length)
    {
        return YES;
    }
    return NO;
}

-(bool)needToFillBuffer
{
    if([self.downloader checkIfEof])
        return NO; // No need to fill buffer.
    return [self.downloader checkAudioPacketsAvailability:NSMakeRange(self.lastReadOffset, self.bufferingPacketsCount)] == NO;
}


#pragma mark - AudioPacketsDelegate protocol methods.
-(void)audioPacketsDownloadStarted:(BOOL)resumed
{
    //if(resumed)
    //    NSLog(@"Download resumed");
}

-(void)audioPacketsDownloadStopped
{
}

-(void)audioPacketsDownloadPaused
{
    //NSLog(@"Download paused");
}

-(void)audioPacketsDownloadProgress:(long)packetOffset PacketsCt:(int)packetsCt IsEof:(bool)isEof
{
    // Lock section
    [_lockDownloadSync lock];
    self.currentDownloadedOffset = packetOffset;
    
    // Invoke packets read callback as we were waiting for download process.
    if(self.packetsReadIsInProgress && [self.downloader checkAudioPacketsAvailability:_rangeReadPackets])
    {
        // Read audio stream packest with callback.
        //if(_streamReadCallback)
        //    [self readPackets:_rangeReadPackets WithCallback:_streamReadCallback];
        self.packetsReadIsInProgress = NO;
        _streamReadCallback = nil;
    }
    
    // Pause buffering.
    // Need to review logic in case when download offset is ahead of read offset.
    if(![self needToFillBuffer])
        [self.downloader pause];
    
    // Unlock section.
    [_lockDownloadSync unlock];
}

@end
