//
//  MediaStream1.m
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 12/26/16.
//  Copyright © 2016 zaqro butskrikidze. All rights reserved.
//

#import "MediaFileStream.h"
#import <AudioToolbox/AudioToolbox.h>
#import <AudioToolbox/AudioFile.h>

@interface MediaFileStream()
{
    AudioFileID _audioFileId;
    NSString* _fileUrl;
    AudioStreamBasicDescription _mediaFormat;
    UInt32 _bytesPerPacket;
    StreamReadCallbackProc _readCallback;
    unsigned char* _buffer;
    UInt32 _bufferSize;
}

-(BOOL)checkError:(OSStatus)error withErrorString:(NSString *)string;
@end

@implementation MediaFileStream

-(void)dealloc{
}

-(AudioFileID)getFileId{
    return _audioFileId;
}

-(void*)open:(NSString*)token Media:(NSString*)mediaId WithCallback:(StreamReadCallbackProc)readStreamCallback{
    AudioFileID audioFileId;
    if(![self checkError:AudioFileOpenURL((__bridge CFURLRef)[NSURL fileURLWithPath:mediaId], kAudioFileReadPermission, 0, &audioFileId) withErrorString:nil])
        return nil;
    
    AudioStreamBasicDescription sourceFormat = {};
    UInt32 size = sizeof(sourceFormat);
    if (![self checkError:AudioFileGetProperty(audioFileId, kAudioFilePropertyDataFormat, &size, &sourceFormat) withErrorString:@"AudioFileGetProperty couldn't get the source data format"])
        return nil;
    
    if (sourceFormat.mBytesPerPacket == 0) {
        /*
         if the source format is VBR, we need to get the maximum packet size
         use kAudioFilePropertyPacketSizeUpperBound which returns the theoretical maximum packet size
         in the file (without actually scanning the whole file to find the largest packet,
         as may happen with kAudioFilePropertyMaximumPacketSize)
         */
        UInt32 size = sizeof(sourceFormat.mBytesPerPacket);
        if (![self checkError:AudioFileGetProperty(audioFileId, kAudioFilePropertyPacketSizeUpperBound, &size, &_bytesPerPacket) withErrorString:@"AudioFileGetProperty kAudioFilePropertyPacketSizeUpperBound failed!"]) {
            AudioFileClose(audioFileId);
            return nil;
        }
        
        // How many packets can we read for our buffer size?
        //afio.numPacketsPerRead = afio.srcBufferSize / afio.srcSizePerPacket;
        // Allocate memory for the PacketDescription structs describing the layout of each packet.
        //afio.packetDescriptions = malloc(afio.numPacketsPerRead * sizeof(AudioStreamPacketDescription));
    } else {
        // CBR source format
        _bytesPerPacket = sourceFormat.mBytesPerPacket;
        //afio.srcSizePerPacket = sourceFormat.mBytesPerPacket;
        //afio.numPacketsPerRead = afio.srcBufferSize / afio.srcSizePerPacket;
        //afio.packetDescriptions = NULL;
    }
    
    memmove(&_mediaFormat, &sourceFormat, sizeof(sourceFormat));
    _audioFileId = audioFileId;
    _fileUrl = mediaId;
    _readCallback = readStreamCallback;
    
    _bufferSize = 32768;
    _buffer = (unsigned char*)malloc(_bufferSize);
    return audioFileId;
}

-(UInt64)getNumberPackets:(float*)packetDurationMS{
    if(_audioFileId == nil)
        return 0;
    UInt64 packetCount = 0;
    UInt32 propertySize = sizeof(packetCount);
    OSStatus err = AudioFileGetProperty(_audioFileId, kAudioFilePropertyAudioDataPacketCount, &propertySize, &packetCount);
    if(err != noErr)
        return 0;
    *packetDurationMS = ([self getMediaDurationInSeconds] * 1000.0) / packetCount;
    return packetCount;
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

-(float)getMediaDurationInSeconds{
    if(_audioFileId == nil)
        return 0.0;
    NSTimeInterval seconds = 0.0;
    UInt32 propertySize = sizeof(seconds);
    OSStatus err = AudioFileGetProperty(_audioFileId, kAudioFilePropertyEstimatedDuration, &propertySize, &seconds);
    if(err != noErr)
        return 0.0;
    return seconds;
}

-(UInt32)getPacketSizeInBytes{
    return _bytesPerPacket;
}

-(BOOL)getStreamDescription:(AudioStreamBasicDescription*)streamDesc{
    memmove(streamDesc, &_mediaFormat, sizeof(_mediaFormat));
    return YES;
}

-(void)close:(NSString*)token{
    AudioFileClose(_audioFileId);
    _audioFileId = nil;
    _fileUrl = nil;
    free(_buffer);
    _buffer = 0;
    _bufferSize = 0;
    memset(&_mediaFormat, 0, sizeof(_mediaFormat));
}

//-(bool)readAsync:(NSString*)token ByStreamId:(void*)streamId ByPosition:(int)atSec WithCallback:(StreamReadCallbackProc)readStreamCallback WithUserData:(void*)lpUserData
//{
//    if(_audioFileId == nil || _audioFileId != streamId)
//        return NO;
//    
//    // Number of packets per millisecond.
//    float packetsPerMSec = 1000.0 * (_mediaFormat.mFramesPerPacket / _mediaFormat.mSampleRate);
//    
//    UInt32 ioNumberDataPackets = 31;
//    // figure out how much to read
//    UInt32 maxPackets = _bufferSize / _mediaFormat.mBytesPerPacket;
//    if (ioNumberDataPackets > maxPackets)
//        ioNumberDataPackets = maxPackets;
//    // read from the file
//    UInt32 outNumBytes = maxPackets * _mediaFormat.mBytesPerPacket;
//
//    // Offset of packet.
//    SInt64 inStartingPacket = (_mediaFormat.mSampleRate / (_mediaFormat.mBitsPerChannel/8) / _mediaFormat.mBytesPerPacket) * atSec;
//    
//    AudioStreamPacketDescription outDesc;
//    OSStatus status = AudioFileReadPacketData(_audioFileId, false, &outNumBytes, &outDesc, inStartingPacket, &ioNumberDataPackets, _buffer);
//    if(status != noErr && status != kAudioFileEndOfFileError )
//        return NO;
//    (_readCallback)(_buffer, outNumBytes, packetsPerMSec*ioNumberDataPackets, (noErr == kAudioFileEndOfFileError), lpUserData);
//    return YES;
//}

-(bool)readPacketData:(UInt32)packetSize PacketOffset:(UInt32)packetOffset Buffer:(void*)buffer GivenSize:(UInt32)bufferSize PacketDesc:(AudioStreamPacketDescription*)streamPacketsDesc OutSize:(UInt32*)outDataSize{
    if(_audioFileId == nil)
        return NO;
    
    // read from the file
    UInt32 ioNumBytes =  31*_bytesPerPacket;
    UInt32 packetSizeRead = packetSize;
    if( ![self checkError:AudioFileReadPacketData(_audioFileId, false, &ioNumBytes, streamPacketsDesc, packetOffset, &packetSizeRead, buffer) withErrorString:nil])
        return NO;
    *outDataSize = ioNumBytes;
    return YES;
}

-(BOOL)checkError:(OSStatus)error withErrorString:(NSString *)string {
    if (error == noErr) {
        return YES;
    }
    /*
    if ([self.delegate respondsToSelector:@selector(audioFileConvertOperation:didEncounterError:)]) {
        NSError *err = [NSError errorWithDomain:@"AudioFileConvertOperationErrorDomain" code:error userInfo:@{NSLocalizedDescriptionKey : string}];
        [self.delegate audioFileConvertOperation:self didEncounterError:err];
    }*/
    return NO;
}
@end
