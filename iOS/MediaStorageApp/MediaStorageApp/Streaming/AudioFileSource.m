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
}

@end

@implementation AudioFileSource

-(instancetype)init:(StreamingSession* __weak)session MediaId:(NSURL*)urlMedia FileType:(AudioFileTypeID)fileTypeId
{
    auto status = AudioFileOpenURL((__bridge CFURLRef)urlMedia, kAudioFileReadPermission, fileTypeId, &_mediaFileId);
    assert(status == noErr);
    _urlMedia = urlMedia;
    
    return [super init];
}

-(bool)getStreamDescription:(AudioStreamBasicDescription*)streamDescOut
{
    assert(_mediaFileId);
    // Get the source data format.
    AudioStreamBasicDescription sourceFormat = {};
    UInt32 size = sizeof(sourceFormat);
    return (AudioFileGetProperty(_mediaFileId, kAudioFilePropertyDataFormat, &size, &sourceFormat) == noErr);
}

-(UInt32)getPacketSizeInBytes
{
    assert(_mediaFileId);
    UInt32 maxPacketSizeInBytes = 0;
    UInt32 size = sizeof(maxPacketSizeInBytes);
    if(AudioFileGetProperty(_mediaFileId, kAudioFilePropertyPacketSizeUpperBound, &size, &maxPacketSizeInBytes) == noErr)
        return maxPacketSizeInBytes;
    return 0;
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

-(bool)readPackets:(NSRange)range InPacketsInfoObject:(AudioStreamPacketsInfo*)packetsInfo
{
    assert(_mediaFileId);
    assert(range.length > 0);
    assert(range.location >= 0);
    
    UInt32 ioNumBytes = packetsInfo.dataSize;
    UInt32 ioNumPackets = (UInt32)range.length;
    SInt64 packetOffset = range.location;
    
    return (noErr == AudioFileReadPacketData(_mediaFileId, false, &ioNumBytes, packetsInfo.streamPacketsDesc, packetOffset, &ioNumPackets, (void*)packetsInfo.data));
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
