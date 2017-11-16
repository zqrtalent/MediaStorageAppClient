//
//  MediaStreamDownloader.m
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 2/20/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "MediaStreamDownloader.h"
#import "AppDelegate.h"
#import "MediaStreamingService.h"
#include "../MediaStorageWebApi/DataContracts/MediaPackets.h"
#include "../MediaStorageWebApi/AudioPacketsByOffsetRequest.h"
#include <pthread.h>

@interface MediaStreamDownloader() <MediaStreamingServiceDelegate>
{
    UInt32 _bytesPerPacket;
    AudioStreamBasicDescription _mediaFormat;
    MediaPackets* _packetsReceived;
    AudioPacketsByOffsetRequest* _audioPacketsReq;
}

@property (nonatomic, strong) MediaStreamingService* streamSvc;
@property (nonatomic, strong) NSString* mediaId;
@property (nonatomic, strong) NSString* sessionKey;

@property (assign, nonatomic) id<MediaStreamReaderProtocol> delegate;
@property (strong, nonatomic) NSThread* thread;
@property (strong, nonatomic) dispatch_semaphore_t download_pause;
@property (strong, nonatomic) dispatch_semaphore_t download_stop;
@property (strong, nonatomic) dispatch_semaphore_t download_sync;

@property (strong, nonatomic) NSCondition* objects_lock;
@property (assign, nonatomic) AutoSortedArrayTempl<long, MediaPacket*>* mediaPacketsByOffset;
@property (assign, nonatomic) long packetOffsetCurrent;
@property (assign, nonatomic) long packetOffsetCurrentNew;
@property (assign, atomic) bool seeking;
@property (assign, nonatomic) int downloadPacketsAtTheTime;
@property (assign, atomic) int numberOfPackets;

@property (assign, atomic) BOOL isPaused;
@property (assign, atomic) BOOL isInProgress;
@property (assign, atomic) BOOL isEof;

@end

@implementation MediaStreamDownloader

-(void)dealloc{
    [self cleanup];
    
#if !__has_feature(objc_arc)
    auto thread = self.thread;
    self.thread = nil;
    [thread dealloc];
    
    auto objects_lock = self.objects_lock;
    self.objects_lock = nil;
    [objects_lock dealloc];
    
    auto streamSvc = self.streamSvc;
    self.streamSvc = nil;
    [streamSvc dealloc];
    
    if(self.download_stop)
        dispatch_release(self.download_stop);
    if(self.download_pause)
        dispatch_release(self.download_pause);
    if(self.download_sync)
        dispatch_release(self.download_sync);
    
    self.download_pause = nil;
    self.download_stop = nil;
    self.download_sync = nil;

#endif
}

-(void)cleanup{
    // Destroy MediaFrame objects dictionary.
    if(self.mediaPacketsByOffset)
    {
        for(int i=0; i<self.mediaPacketsByOffset->GetCount(); i++)
        {
            auto* pFrame = self.mediaPacketsByOffset->GetValueByIndex(i);
            if(pFrame)
                delete pFrame;
        }
        self.mediaPacketsByOffset->DeleteAll();
        delete self.mediaPacketsByOffset;
        self.mediaPacketsByOffset = nullptr;
    }
    
    self.packetOffsetCurrent = 0;
    _packetsReceived = nil;
    
//    self.download_pause = nil;
//    self.download_stop = nil;
//    self.download_sync = nil;
//    self.thread = nil;
}

-(MediaStreamDownloader*)init:(id<MediaStreamReaderProtocol>)delegate
{
    self.downloadPacketsAtTheTime = 50;
    self.delegate = delegate;
    self.objects_lock = [[NSCondition alloc] init];
    self.mediaPacketsByOffset = new AutoSortedArrayTempl<long, MediaPacket*>();
    self.streamSvc = [[MediaStreamingService alloc] init:self];
    self.numberOfPackets = 0;
    
    /*
     NSString* mp3File = [NSString stringWithFormat:@"%@/2000", [[NSBundle mainBundle] resourcePath]];
     NSURL* fileUrl = [NSURL fileURLWithPath:mp3File];
     NSData* data = [NSData dataWithContentsOfURL:fileUrl];
     
     GrowableMemory mem(0, 0x400, false);
     mem.SetReadonlyBuffer((BYTE*)[data bytes], [data length]);
     
     MediaPackets packets;
     while(packets.Deserialize(&mem)){
     long offset = self.packetOffsetCurrent;
     for(int i=0; i<packets._frames.GetCount(); i++){
     auto f = packets._frames.GetAt(i);
     if(f && f->_data.GetBinarySize() > 0){
     auto frameExisting = self.mediaPacketsByOffset->GetValue(offset);
     if(frameExisting)
     {
     delete frameExisting; // Destroy media frame object.
     self.mediaPacketsByOffset->SetAt(offset, f);
     }
     else
     self.mediaPacketsByOffset->Add(offset, f);
     }
     offset ++;
     }
     packets._frames.RemoveAll(false); // Keep allocated data.
     self.packetOffsetCurrent = offset;
     }*/
    return self;
}

-(void)downloaderThreadEntry
{
    bool isEof = false;
    self.isInProgress = true;
    [self.delegate mediaPacketsDownloadStarted:NO]; // Start event.
    
    while(true){
        if(isEof) break;
        
        // Seeking operation.
        if(self.seeking)
        {
            [self.objects_lock lock];
            self.packetOffsetCurrent = self.packetOffsetCurrentNew;
            self.packetOffsetCurrentNew = 0;
            [self.objects_lock unlock];
            self.seeking = NO;
        }
        
        // Download packets data.
        if(![self download:self.packetOffsetCurrent Packets:self.downloadPacketsAtTheTime IsEof:&isEof])
        {
            [self.objects_lock lock]; // lock
            self.isEof = isEof;
            [self.objects_lock unlock]; // Unlock
            self.isInProgress = false;
            self.isPaused = false;
            [self.delegate mediaPacketsDownloadStopped];
            break;
        }
        
        // [NSThread sleepForTimeInterval:0.250];
        
        // Check for pause signal.
        if(!dispatch_semaphore_wait(self.download_pause, DISPATCH_TIME_NOW)){ // Non-Timeout
            self.isPaused = true;
            [self.delegate mediaPacketsDownloadPaused]; // Paused event.
            dispatch_semaphore_wait(self.download_pause, DISPATCH_TIME_FOREVER);
            self.isPaused = false;
            [self.delegate mediaPacketsDownloadStarted:YES]; // Resumed event.
        }

        // Check for stop signal.
        if(!dispatch_semaphore_wait(self.download_stop, DISPATCH_TIME_NOW)){ // Non-Timeout
            self.isInProgress = false;
            self.isPaused = false;
            [self.delegate mediaPacketsDownloadStopped];
            break;
        }
    }
}

-(bool)download:(long)packetOffset Packets:(int)packetsCt IsEof:(bool*)pIsEof
{
    // Download audio packets.
    //[self.streamSvc AudioPacketsByOffset:_sessionKey SongId:_mediaId byOffset:(unsigned int)packetOffset Packets:packetsCt];
    //dispatch_semaphore_wait(self.download_sync, DISPATCH_TIME_FOREVER);
    
    if(!_audioPacketsReq)
        _audioPacketsReq = [[AudioPacketsByOffsetRequest alloc] init:_sessionKey SongId:_mediaId byOffset:(int)packetOffset Packets:packetsCt];
    else
        [_audioPacketsReq setQueryParams:_sessionKey SongId:_mediaId byOffset:(int)packetOffset Packets:packetsCt];
    
    __weak __typeof__(self) weakSelf = self;
    [_audioPacketsReq makeRequest:^(MediaPackets* packets)
    {
        [weakSelf OnAudioPacketsByOffsetResponse:packets];
    }];
    dispatch_semaphore_wait(self.download_sync, DISPATCH_TIME_FOREVER);
    //_audioPacketsReq = nil; // Delete request object.
    
    MediaPackets* packets = _packetsReceived;
    _packetsReceived = nullptr;
    
    if(packets)
    {
        int numberOfPackets = 0;
        [self.objects_lock lock];           // Lock
        if(self.mediaPacketsByOffset == nil)
            self.mediaPacketsByOffset = new AutoSortedArrayTempl<long, MediaPacket*>();
        //long offset = self.packetOffsetCurrent;
        long offset = packetOffset;
        for(int i=0; i<packets->_packets.GetCount(); i++)
        {
            auto f = packets->_packets.GetAt(i);
            if(f && f->_data.GetBinarySize() > 0)
            {
                auto frameExisting = self.mediaPacketsByOffset->GetValue(offset);
                if(frameExisting)
                {
                    delete frameExisting;   // Destroy media frame object.
                    self.mediaPacketsByOffset->SetAt(offset, f);
                }
                else
                    self.mediaPacketsByOffset->Add(offset, f);
            }
            offset ++;
        }
        
        numberOfPackets = (int)packets->_framesCt;
        self.packetOffsetCurrent = offset;
        if(packets->_numPackets == 0)
            self.isEof = true;
        else
            self.isEof = packets->_isEof;
        
        *pIsEof = self.isEof;
        
        [self.objects_lock unlock];         // Unlock
        packets->_packets.RemoveAll(false);   // Remove array but keep value.
        
        // Update number of packets.
        if(!self.numberOfPackets)
            self.numberOfPackets = numberOfPackets;
        
        // Download progress event.
        [self.delegate mediaPacketsDownloadProgress:self.packetOffsetCurrent packetsCt:packets->_numPackets isEof:*pIsEof];
    }
    
    delete packets;
    return packets != nullptr;
}

-(void)OnAudioPacketsByTimeResponse:(MediaPackets *)packetsInfo
{
    _packetsReceived = packetsInfo;
    dispatch_async(dispatch_get_main_queue(), ^{
        // Signal download sync.
        dispatch_semaphore_signal(self.download_sync);
    });
}

-(void)OnAudioPacketsByOffsetResponse:(MediaPackets *)packetsInfo{
    _packetsReceived = packetsInfo;
    dispatch_async(dispatch_get_main_queue(), ^{
        // Signal download sync.
        dispatch_semaphore_signal(self.download_sync);
    });
}

-(void)start:(long)packetOffset{
    if(!self.thread.isExecuting){
    
#if !__has_feature(objc_arc)
        auto thread = self.thread;
        self.thread = nil;
        [thread dealloc];
        
        if(self.download_stop)
            dispatch_release(self.download_stop);
        if(self.download_pause)
            dispatch_release(self.download_pause);
        if(self.download_sync)
            dispatch_release(self.download_sync);
#endif
        
        if(self.isEof)
            return; // Already downloaded.
        
        [self.objects_lock lock];
        self.download_stop = dispatch_semaphore_create(0);
        self.download_pause = dispatch_semaphore_create(0);
        self.download_sync = dispatch_semaphore_create(0);
        
        self.packetOffsetCurrent = packetOffset < 0 ? 0 : packetOffset;
        [self.objects_lock unlock];
        
        // Create thread.
        self.thread = [[NSThread alloc] initWithTarget:self selector:@selector(downloaderThreadEntry) object:nil];
        
        // Start download
        [self.thread start];
    }
    else{
        if(self.isPaused) // Resume download
            dispatch_semaphore_signal(self.download_pause);
        
        [self.objects_lock lock];
        if(packetOffset > 0){
            self.packetOffsetCurrentNew = packetOffset;
            self.seeking = YES;
        }
        [self.objects_lock unlock];
    }
}

-(BOOL)resume{
    if(!self.thread.isExecuting && self.isPaused){
        dispatch_semaphore_signal(self.download_pause);
        return YES;
    }
    return NO;
}

-(void)seek:(long)packetOffset{
    if(self.thread.isExecuting){
        [self start:packetOffset];
    }
}

-(void)pause{
    if(self.thread.isExecuting){
        if(dispatch_semaphore_signal(self.download_pause))
            NSLog(@"Pause download nonzero returned");
    }
}

-(void)stop{
    if(self.thread.isExecuting){
        bool isPaused = self.isPaused;
        // Stop streaming service.
        [self.streamSvc ForceToStop];
        // In case if download is in progress.
        if(dispatch_semaphore_signal(self.download_stop))
            NSLog(@"Stop download nonzero returned");
        if(isPaused) // Resume downloader.
            dispatch_semaphore_signal(self.download_pause);
        // Interrupt download wait.
        dispatch_semaphore_signal(self.download_sync);
        
        [self.objects_lock lock]; // lock
        [self cleanup];
        [self.objects_lock unlock]; // unlock

    }
}

-(void*)open:(NSString*)sessionKey Media:(NSString*)mediaId WithCallback:(StreamReadCallbackProc)readStreamCallback{
    _sessionKey = [NSString stringWithString:sessionKey]; // Non Arc
    _mediaId = mediaId;
    
    self.numberOfPackets = 0;
    _bytesPerPacket = 1052;
    _mediaFormat.mSampleRate = 44100;
    _mediaFormat.mFormatID = kAudioFormatMPEGLayer3;//778924083
    _mediaFormat.mChannelsPerFrame = 2;
    _mediaFormat.mBitsPerChannel = 0;
    _mediaFormat.mBytesPerPacket = 0;
    //_mediaFormat.mFramesPerPacket = 1152;
    //sourceFormat = kLinearPCMFormatFlagIsPacked | kLinearPCMFormatFlagIsSignedInteger; // little-endian
    //_mediaFormat.mFormatFlags =  kLinearPCMFormatFlagIsFloat; // little-endian
    _mediaFormat.mReserved = 0;
    return nil;
}

-(bool)readPacketData:(UInt32)packetSize PacketOffset:(UInt32)packetOffset Buffer:(void*)buffer GivenSize:(UInt32)bufferSize PacketDesc:(AudioStreamPacketDescription*)streamPacketsDesc OutSize:(UInt32*)outDataSize
{
    bool ret = NO;
    [self.objects_lock lock]; // lock
    if(packetOffset + packetSize < self.packetOffsetCurrent){
        UInt32 dataSizeUsed = 0;
        char* pBuffer = (char*)buffer;
        for(long i=packetOffset; i<packetOffset + packetSize; i++)
        {
            auto packet = self.mediaPacketsByOffset->GetValue(i);
            if(packet){
                long index = (i - packetOffset);
                streamPacketsDesc[index].mStartOffset = dataSizeUsed;
                streamPacketsDesc[index].mVariableFramesInPacket = packet->_samplePerFrame; // VBR data only.
                streamPacketsDesc[index].mDataByteSize = packet->_data.GetBinarySize();
                
                memcpy(pBuffer, packet->_data.LockMemory(), packet->_data.GetBinarySize());
                packet->_data.UnlockMemory();
                dataSizeUsed += packet->_data.GetBinarySize();
                pBuffer += packet->_data.GetBinarySize();
                
                if(dataSizeUsed > bufferSize){
                    [self.objects_lock unlock]; // unlock
                    return NO; // Insufficient buffer
                }
            }
        }
        *outDataSize = dataSizeUsed;
    }
    [self.objects_lock unlock]; // unlock
    return ret;
}

-(UInt64)getNumberPackets:(float*)packetDurationMS{
    return self.numberOfPackets;
}

-(float)getMediaDurationInSeconds{
    return (self.numberOfPackets * MP3_FRAME_DURATION_MSEC)/1000; // Temporary
}

-(UInt32)getPacketSizeInBytes{
    return _bytesPerPacket;
}

-(NSString*)getSongName{
    return nil;
}

-(NSString*)getArtistName{
    return nil;
}

-(NSString*)getAlbumName{
    return nil;
}

-(BOOL)getStreamDescription:(AudioStreamBasicDescription*)streamDesc{
    memmove(streamDesc, &_mediaFormat, sizeof(_mediaFormat));
    return YES;
}

-(void)close:(NSString*)token{
}
@end
