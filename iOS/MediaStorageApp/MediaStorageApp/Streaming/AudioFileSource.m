//
//  AudioFileSource.m
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 11/11/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "AudioFileSource.h"

@interface AudioFileSource()
{
    AudioFileID _mediaFileId;
    NSURL* _urlMedia;
    NSMutableData* _readPacketsBuffer;
    UInt64 _numOfPackets;
    UInt32 _packetSizeInBytes;
}

@end

@implementation AudioFileSource

-(instancetype)init:(StreamingSession* __weak)session MediaId:(NSURL*)urlMedia FileType:(AudioFileTypeID)fileTypeId
{
    auto status = AudioFileOpenURL((__bridge CFURLRef)urlMedia, kAudioFileReadPermission, fileTypeId, &_mediaFileId);
    assert(status == noErr);
    _urlMedia = urlMedia;
    _readPacketsBuffer = [[NSMutableData alloc] initWithLength:[self getPacketSizeInBytes]*20];
    _numOfPackets = 0;
    _packetSizeInBytes = 0;
    
    AudioStreamBasicDescription desc = {};
    [self getStreamDescription:&desc];
    
    return [super init];
}

-(bool)getStreamDescription:(AudioStreamBasicDescription*)streamDescOut
{
    assert(_mediaFileId);
    // Get the source data format.
    AudioStreamBasicDescription sourceFormat = {};
    UInt32 size = sizeof(sourceFormat);
    if(AudioFileGetProperty(_mediaFileId, kAudioFilePropertyDataFormat, &size, &sourceFormat) == noErr)
    {
        memcpy(streamDescOut, &sourceFormat, sizeof(AudioStreamBasicDescription));
        return YES;
    }
    return NO;
}

-(UInt32)getPacketSizeInBytes
{
    assert(_mediaFileId);
    
    if(_packetSizeInBytes == 0)
    {
    
        UInt32 maxPacketSizeInBytes = 0;
        UInt32 size = sizeof(maxPacketSizeInBytes);
        if(AudioFileGetProperty(_mediaFileId, kAudioFilePropertyPacketSizeUpperBound, &size, &maxPacketSizeInBytes) == noErr)
        {
            _packetSizeInBytes = maxPacketSizeInBytes;
            return maxPacketSizeInBytes;
        }
    }
    
    return _packetSizeInBytes;
}

-(UInt32)getNumberOfPackets
{
    assert(_mediaFileId);
    if(!_numOfPackets)
    {
        UInt64 numberOfPackets = 0;
        UInt32 size = sizeof(numberOfPackets);
        if(AudioFileGetProperty(_mediaFileId, kAudioFilePropertyAudioDataPacketCount, &size, &numberOfPackets) == noErr)
        {
            _numOfPackets = numberOfPackets;
            return (UInt32)numberOfPackets;
        }
    }
    
    return (UInt32)_numOfPackets;
}

-(AudioStreamPacketsInfo*)readPackets:(NSRange)range
{
    assert(_mediaFileId);
    assert(range.length > 0);
    assert(range.location >= 0);
    
    AudioStreamPacketsInfo* packetsInfo = [[AudioStreamPacketsInfo alloc] init: [self getPacketSizeInBytes]*(UInt32)range.length];
    if([self readPackets:range InPacketsInfoObject:packetsInfo])
        return packetsInfo;
    return nil;
}

-(StreamReadPacketStatus)readPackets:(NSRange)range InPacketsInfoObject:(AudioStreamPacketsInfo*)packetsInfo
{
    assert(_mediaFileId);
    assert(range.length > 0);
    assert(range.location >= 0);
    
    OSStatus status = noErr;
    AudioStreamPacketDescription desc[20];
    UInt32 ioNumBytes = 0;
    UInt32 readPacketsCt = 0;
    UInt32 ioNumPackets = 0;
    SInt64 packetOffset = range.location;
    
    Byte* pData = (Byte*)_readPacketsBuffer.bytes;
    [packetsInfo clearPacketsData:NO];
    
    while(readPacketsCt < range.length)
    {
        ioNumBytes = (UInt32)_readPacketsBuffer.length;
        ioNumPackets = MIN(20, ((UInt32)range.length - readPacketsCt));
        
        status = AudioFileReadPacketData(_mediaFileId, false, &ioNumBytes, desc, packetOffset, &ioNumPackets, pData);
        if(status == noErr || status == kAudioFileEndOfFileError)
        {
            UInt32 packetDataOffset = 0;
            for(int i=0; i<ioNumPackets; i++)
            {
                [packetsInfo addAudioPacket:&pData[packetDataOffset] PacketDataSize:desc[i].mDataByteSize StreamPacketDesc:desc[i]];
                packetDataOffset += desc[i].mDataByteSize;
            }
            
            readPacketsCt += ioNumPackets;
            packetOffset += ioNumPackets;
            
            if(status == kAudioFileEndOfFileError)
                break;
        }
        else
            break;
    }
    
    return (status == noErr ? StreamReadPacketStatus_Success : StreamReadPacketStatus_ReadError);
}

-(long)timeMsecOffset2PacketOffset:(UInt32)positionMsec
{
    assert(_mediaFileId);
    AudioStreamBasicDescription desc = {};
    [self getStreamDescription:&desc];
    
    AudioFramePacketTranslation frame2packet;
    frame2packet.mFrame = (positionMsec*desc.mSampleRate) / 1000;
    
    UInt32 size = sizeof(frame2packet);
    if(AudioFileGetProperty(_mediaFileId, kAudioFilePropertyFrameToPacket, &size, &frame2packet) == noErr)
        return frame2packet.mPacket;
    return 0;
}

-(void)closeAndInvalidate
{
    if(_mediaFileId != nil){
        AudioFileClose(_mediaFileId);
        _mediaFileId = nil;
    }
    _urlMedia = nil;
}

@end
