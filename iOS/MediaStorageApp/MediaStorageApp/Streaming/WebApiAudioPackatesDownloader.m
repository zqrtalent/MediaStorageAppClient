//
//  WebApiAudioPackatesDownloader.m
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 10/7/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "WebApiAudioPackatesDownloader.h"
#include "../MediaStorageWebApi/DataContracts/MediaPackets.h"
#import "../Extensions/StreamingSession+ApiRequests.h"
#import "../MediaStorageWebApi/AudioPacketsByOffsetRequest.h"
#import "../Models/AudioStreamPacketsInfo.h"

@interface WebApiAudioPackatesDownloader()
{
    UInt32 _bytesPerPacket;
    AudioStreamBasicDescription _mediaFormat;
    AudioPacketsByOffsetRequest* _audioPacketsReq;
}

@property (nonatomic, strong) id<AudioPacketsDownloaderProtocol> delegate;

@property (nonatomic, strong) NSString* mediaId;
@property (nonatomic, weak) StreamingSession* session;

@property (strong, nonatomic) NSThread* thread;
@property (strong, nonatomic) dispatch_semaphore_t download_pause;
@property (strong, nonatomic) dispatch_semaphore_t download_stop;
@property (strong, nonatomic) dispatch_semaphore_t download_sync;
@property (strong, nonatomic) NSCondition* operations_lock;

@property (strong, nonatomic) NSCondition* objects_lock;
@property (assign, nonatomic) AutoSortedArrayTempl<long, MediaPacket*>* mediaPacketsByOffset;
@property (assign, nonatomic) long packetOffsetCurrent;
@property (assign, nonatomic) long packetOffsetCurrentNew;
@property (assign, atomic) bool seeking;
@property (assign, nonatomic) int downloadPacketsAtTheTime;
@property (assign, atomic) int numberOfPackets;

@property (assign, atomic) BOOL isPaused;
@property (assign, atomic) BOOL pauseRequested;
@property (assign, atomic) BOOL isInProgress;
@property (assign, atomic) BOOL isEof;


-(MediaPackets*)downloadPackets:(NSRange)range andWait:(NSTimeInterval)waitTimeSec;
-(bool)download:(NSRange)range IsEof:(bool*)pIsEof;
-(void)freeUpMemoryBuffer;

@end

@implementation WebApiAudioPackatesDownloader

-(instancetype)init:(id<AudioPacketsDownloaderProtocol>)delegate Session:(StreamingSession* __weak)session MediaId:(NSString*)mediaId
{
    self.session = session;
    self.mediaId = mediaId;
    
    self.downloadPacketsAtTheTime = 50;
    self.delegate = delegate;
    self.objects_lock = [[NSCondition alloc] init];
    self.mediaPacketsByOffset = new AutoSortedArrayTempl<long, MediaPacket*>();
    self.numberOfPackets = 0;
    
    self.operations_lock = [[NSCondition alloc] init];
    
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

-(BOOL)start:(long)packetOffset
{
    BOOL ret = NO;
    assert(packetOffset >= 0);
    if(!self.thread.isExecuting)
    {
        if([self checkIfDownloadedCompleted])
            return ret; // Already downloaded.
        
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
        
        ret = YES;
    }
    else
    {
        [self.objects_lock lock];
        self.packetOffsetCurrentNew = packetOffset;
        self.seeking = YES;
        [self.objects_lock unlock];
        
        // Resume paused download.
        ret = [self resume];
    }
    
    return ret;
}

-(bool)pause
{
    bool ret = NO;
    [self.operations_lock lock];
    if(self.thread.isExecuting &&
       !self.isPaused &&
       !self.pauseRequested)
    {
        self.pauseRequested = YES;
        dispatch_semaphore_signal(self.download_pause);
        ret = YES;
    }
    [self.operations_lock unlock];
    return ret;
}

-(bool)resume
{
    [self.operations_lock lock];
    
    if(self.thread.isExecuting &&
       (self.isPaused || self.pauseRequested))
    {
        dispatch_semaphore_signal(self.download_pause);
    }
    [self.operations_lock unlock];
    return YES;
}

-(bool)seek:(long)packetOffset
{
    assert(packetOffset >= 0);
    if(self.thread.isExecuting)
    {
        [self start:packetOffset];
    }
    
    return YES;
}

-(bool)stop
{
    if(self.thread.isExecuting)
    {
        bool resume = (self.isPaused || self.pauseRequested);
        
        // In case if download is in progress.
        dispatch_semaphore_signal(self.download_stop);
        
        // Resume downloader if paused.
        if(resume)
            [self resume];
        
        // Interrupt download wait.
        dispatch_semaphore_signal(self.download_sync);
        
        [self.objects_lock lock]; // lock
        [self freeUpMemoryBuffer];
        [self.objects_lock unlock]; // unlock
    }
    
    return YES;
}

-(bool)checkIfDownloadedCompleted
{
    return (self.isEof && [self checkAudioPacketsAvailability:NSMakeRange(0, self.numberOfPackets)]);
}

-(bool)checkAudioPacketsAvailability:(NSRange)packetsRange
{
    bool available = NO;
    [self.objects_lock lock]; // lock
    if(self.mediaPacketsByOffset != nullptr && self.mediaPacketsByOffset->IndexOf(packetsRange.location) != -1)
    {
        available = (self.mediaPacketsByOffset->IndexOf(packetsRange.location+packetsRange.length) - self.mediaPacketsByOffset->IndexOf(packetsRange.location)) == packetsRange.length;
    }
    [self.objects_lock unlock]; // unlock
    return available;
}


-(bool)copyAudioPacketsData:(NSRange)range PacketsInfo:(AudioStreamPacketsInfo*)packets OutSize:(UInt32*)outDataSize
{
    bool ret = NO;
    [self.objects_lock lock]; // lock
    AudioStreamPacketDescription packetDesc;
    memset(&packetDesc, 0, sizeof(AudioStreamPacketDescription));
    [packets clearPacketsData:NO];
    packets.offset = range.location;

    UInt32 packetsDataSize = 0;

    for(long i=range.location; i<range.location + range.length; i++)
    {
        auto packet = self.mediaPacketsByOffset->GetValue(i);
        if(packet)
        {
            packetDesc.mStartOffset = 0;
            packetDesc.mVariableFramesInPacket = packet->_samplePerFrame; // VBR data only.
            packetDesc.mDataByteSize = packet->_data.GetBinarySize();

            [packets addAudioPacket:packet->_data.LockMemory() PacketDataSize:packet->_data.GetBinarySize() StreamPacketDesc:packetDesc];
            packet->_data.UnlockMemory();

            packetsDataSize += packetDesc.mDataByteSize;
        }
        else // One packet is missing.
        {
            [packets clearPacketsData:NO];
            packetsDataSize = 0;
            break;
        }
    }

    *outDataSize = packetsDataSize;
    ret = (packetsDataSize > 0);
    [self.objects_lock unlock]; // unlock
    return ret;
}

-(bool)checkIfPaused
{
    return self.isPaused;
}

-(bool)checkIfEof
{
    return self.isEof;
}

-(void)freeUpMemoryBuffer
{
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
}


#pragma mark - download thread proc

-(void)downloaderThreadEntry
{
    bool isEof = NO;
    self.isInProgress = YES;
    [self.delegate audioPacketsDownloadStarted:NO]; // Start event.
    
    while(true)
    {
        // Seeking operation.
        if(self.seeking)
        {
            [self.objects_lock lock];
            self.packetOffsetCurrent = self.packetOffsetCurrentNew;
            self.packetOffsetCurrentNew = 0;
            [self.objects_lock unlock];
            
            self.seeking = NO;
            isEof = NO;
        }
        
        // Download packets data.
        if( ![self download:NSMakeRange(self.packetOffsetCurrent, self.downloadPacketsAtTheTime) IsEof:&isEof])
        {
            break; // Can't download packets.
        }
        
        // Downloaded last chunk of audio packets.
        if(isEof)
        {
            if([self checkAudioPacketsAvailability:NSMakeRange(0, self.numberOfPackets)])
                break; // Download completed.
            else // Idle timeout.
                [NSThread sleepForTimeInterval:0.25];
        }
        
        // if pause event is signaled.
        if(!dispatch_semaphore_wait(self.download_pause, DISPATCH_TIME_NOW)) // Non-Timeout
        {
            [self.operations_lock lock];
            self.isPaused = YES;
            self.pauseRequested = NO;
            [self.operations_lock unlock];
            
            [self.delegate audioPacketsDownloadPaused]; // Paused event.
            
            dispatch_semaphore_wait(self.download_pause, DISPATCH_TIME_FOREVER);
            
            [self.operations_lock lock];
            self.isPaused = NO;
            [self.operations_lock unlock];
            [self.delegate audioPacketsDownloadStarted:YES]; // Resumed event.
        }
        
        // Check for stop signal.
        if(!dispatch_semaphore_wait(self.download_stop, DISPATCH_TIME_NOW)) // Non-Timeout
        {
            break; // Stop event signaled.
        }
    }
    
    // Download loop finished
    self.isEof = isEof;
    self.isInProgress = NO;
    self.isPaused = NO;
    self.pauseRequested = NO;
    
    [self.delegate audioPacketsDownloadStopped];
}

-(MediaPackets*)downloadPackets:(NSRange)range andWait:(NSTimeInterval)waitTimeSec;
{
    // Check packet offset.
    if(self.numberOfPackets > 0 && range.location >= self.numberOfPackets)
        return nullptr;
    
    // Download audio packets.
    if(!_audioPacketsReq)
        _audioPacketsReq = [self.session audioPacketsByOffset:_mediaId Range:range];
    else
        [_audioPacketsReq setQueryParams:self.session.sessionId SongId:_mediaId Range:range];
    
    __weak __typeof__(self) weakSelf = self;
    MediaPackets* __block packets = nullptr;
    [_audioPacketsReq makeRequest:^(MediaPackets* respPackets){
        packets = respPackets;
        // Signal download sync.
        dispatch_semaphore_signal(weakSelf.download_sync);
    }];
    
    // Wait forever until packets data arrive.
    if(waitTimeSec <= 0.0)
    {
        dispatch_semaphore_wait(self.download_sync, DISPATCH_TIME_FOREVER);
    }
    else
    {
        int maxLoop =  MAX((int)(waitTimeSec / 0.25), 1);
        while(maxLoop > 0 && dispatch_semaphore_wait(self.download_sync, DISPATCH_TIME_NOW) != 0)
        {
            [NSThread sleepForTimeInterval:0.25];
            maxLoop --;
        }
        //assert(maxLoop > 0);
    }
    
    return packets;
}

-(bool)download:(NSRange)range IsEof:(bool*)pIsEof
{
    //NSLog(@"start download: %ld - %ld", range.location, range.length);
    // Download audio packets.
    MediaPackets* packets = [self downloadPackets:range andWait:0.0];
    
    if(packets)
    {
        //NSLog(@"finish download: %ld - %d", packets->_offset, packets->_numPackets);
        
        int numberOfPackets = 0;
        [self.objects_lock lock];           // Lock
        if(self.mediaPacketsByOffset == nil)
            self.mediaPacketsByOffset = new AutoSortedArrayTempl<long, MediaPacket*>();
        
        //long offset = self.packetOffsetCurrent;
        long offset = range.location;
        for(int i=0; i<packets->_packets.GetCount(); i++)
        {
            auto f = packets->_packets.GetAt(i);
            if(f && f->_data.GetBinarySize() > 0)
            {
                auto existingFrame = self.mediaPacketsByOffset->GetValue(offset);
                if(existingFrame)
                {
                    delete existingFrame;   // Destroy media frame object.
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
        packets->_packets.RemoveAll(false); // Remove array but keep value.
        
        // Update number of packets.
        if(!self.numberOfPackets)
            self.numberOfPackets = numberOfPackets;
        
        // Download progress event.
        [self.delegate audioPacketsDownloadProgress:self.packetOffsetCurrent PacketsCt:packets->_numPackets IsEof:*pIsEof];
    }
    
    delete packets;
    return packets != nullptr;
}

@end
