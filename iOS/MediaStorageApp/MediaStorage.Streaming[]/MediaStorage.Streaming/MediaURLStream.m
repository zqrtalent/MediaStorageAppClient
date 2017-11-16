//
//  MediaURLStream.m
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 2/11/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "MediaURLStream.h"
#import "MediaStreamingService.h"
#include "Serialize/Serializable.h"
#include "Utility/GrowableMemory.h"
#include "../MediaStorageWebApi/DataContracts/MediaPackets.h"

@interface MediaURLStream()<MediaStreamingServiceDelegate>
{
    MediaStreamingService* _streamSvc;
    NSString* _mediaId;
    NSString* _sessionKey;
    MediaPackets _packets;
    
    AudioStreamBasicDescription _mediaFormat;
    UInt32 _bytesPerPacket;
    StreamReadCallbackProc _readCallback;
    unsigned char* _buffer;
    UInt32 _bufferSize;
    
    AutoSortedArrayTempl<long,MediaPacket*> _dicPacketsByOffset;
}

-(BOOL)checkError:(OSStatus)error withErrorString:(NSString *)string;
@end

@implementation MediaURLStream

-(void)dealloc{
    for(auto i=0; i<_dicPacketsByOffset.GetCount(); i++)
        delete _dicPacketsByOffset.GetAt(i);
    _dicPacketsByOffset.DeleteAll();
    
    [self close:@""];
}

-(void)init{
    _streamSvc = [[MediaStreamingService alloc] init:self];
    _packets.ZeroInit();
}

-(void*)open:(NSString*)sessionKey Media:(NSString*)mediaId WithCallback:(StreamReadCallbackProc)readStreamCallback{
    _sessionKey = sessionKey;
    _mediaId = mediaId;
    
    _bytesPerPacket = 1052;
    
    _mediaFormat.mSampleRate = 44100;
    _mediaFormat.mFormatID = kAudioFormatMPEGLayer3;//778924083
    _mediaFormat.mChannelsPerFrame = 2;
    _mediaFormat.mBitsPerChannel = 0;
    _mediaFormat.mBytesPerPacket = 0;
    _mediaFormat.mFramesPerPacket = 1152;
    //sourceFormat = kLinearPCMFormatFlagIsPacked | kLinearPCMFormatFlagIsSignedInteger; // little-endian
    //_mediaFormat.mFormatFlags =  kLinearPCMFormatFlagIsFloat; // little-endian
    _mediaFormat.mReserved = 0;
    
    //_streamSvc AudioPacketsByOffset:<#(NSString *)#> MediaId:<#(NSString *)#> byOffset:<#(int)#> Packets:<#(int)#>
    return nil;
}

-(UInt64)getNumberPackets:(float*)packetDurationMS{
//    if(_audioFileId == nil)
//        return 0;
//    UInt64 packetCount = 0;
//    UInt32 propertySize = sizeof(packetCount);
//    OSStatus err = AudioFileGetProperty(_audioFileId, kAudioFilePropertyAudioDataPacketCount, &propertySize, &packetCount);
//    if(err != noErr)
//        return 0;
//    *packetDurationMS = ([self getMediaDurationInSeconds] * 1000.0) / packetCount;
//    return packetCount;
    return 0;
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

-(AudioFileID)getFileId{
    return nil;
}

-(float)getMediaDurationInSeconds{
//    if(_audioFileId == nil)
//        return 0.0;
//    NSTimeInterval seconds = 0.0;
//    UInt32 propertySize = sizeof(seconds);
//    OSStatus err = AudioFileGetProperty(_audioFileId, kAudioFilePropertyEstimatedDuration, &propertySize, &seconds);
//    if(err != noErr)
//        return 0.0;
//    return seconds;
    return 0.0;
}

-(UInt32)getPacketSizeInBytes{
    return _bytesPerPacket;
}

-(BOOL)getStreamDescription:(AudioStreamBasicDescription*)streamDesc{
    memmove(streamDesc, &_mediaFormat, sizeof(_mediaFormat));
    return YES;
}

-(void)close:(NSString*)token{
//    AudioFileClose(_audioFileId);
//    _audioFileId = nil;
//    _fileUrl = nil;
//    free(_buffer);
//    _buffer = 0;
//    _bufferSize = 0;
//    memset(&_mediaFormat, 0, sizeof(_mediaFormat));
}

-(void)OnAudioPacketsByTimeResponse:(MediaPackets *)packetsInfo{
    unsigned long packetOffset = packetsInfo->_offset;
    for(int i=0; i<packetsInfo->_packets.GetCount(); i++){
        auto f = packetsInfo->_packets.GetAt(i);
        if(f && f->_data.GetBinarySize() > 0)
            _dicPacketsByOffset.Add(packetOffset, f);
        packetOffset ++;
        }
    packetsInfo->_packets.RemoveAll(false);
    delete packetsInfo;
}

-(void)OnAudioPacketsByOffsetResponse:(MediaPackets *)packetsInfo{
    delete packetsInfo;
}

-(bool)readPacketData:(UInt32)packetSize PacketOffset:(UInt32)packetOffset Buffer:(void*)buffer GivenSize:(UInt32)bufferSize PacketDesc:(AudioStreamPacketDescription*)streamPacketsDesc OutSize:(UInt32*)outDataSize{
    
//    MediaFrame* pFrame = _dicPacketsByOffset.GetValue(packetOffset);
//    if(pFrame){
//        *outDataSize = pFrame;
//    }
//    
//    NSURL* url = [NSURL URLWithString:[NSString stringWithFormat:@"http://%@/library/media/%@/stream/%d/packet/%d", _mediaWebApiHost, _mediaId, packetOffset, packetSize]];
//    
//    NSURLSessionConfiguration *defaultConfigObject = [NSURLSessionConfiguration defaultSessionConfiguration];
//    NSURLSession *defaultSession = [NSURLSession sessionWithConfiguration: defaultConfigObject delegate: nil delegateQueue: [NSOperationQueue mainQueue]];
//    
//    NSURLSessionDataTask * dataTask = [defaultSession dataTaskWithURL:url];
//    [dataTask resume];
    
//    if(_packets._frames.GetCount() == 0)
//        return NO;
//    
//    for(int i=0; i<_packets._frames.GetCount(); i++){
//        *outDataSize += _packets._frames.GetAt(i)->_data.GetBinarySize();
//    }
    
    
//    if(_audioFileId == nil)
//        return NO;
//
//    // read from the file
//    UInt32 ioNumBytes =  31*_bytesPerPacket;
//    UInt32 packetSizeRead = packetSize;
//    if( ![self checkError:AudioFileReadPacketData(_audioFileId, false, &ioNumBytes, streamPacketsDesc, packetOffset, &packetSizeRead, buffer) withErrorString:nil])
//        return NO;
//    *outDataSize = ioNumBytes;
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
