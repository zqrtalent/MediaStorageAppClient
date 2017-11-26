//
//  Mp3Decoder.m
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 1/4/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "AudioStreamDecoder.h"
#include <memory>

@interface AudioStreamDecoder()
{
@private
    id<MediaStreamSourceProtocol>     _media;
    AudioConverterRef                _converter;
    AudioStreamBasicDescription       _inFormat;
    AudioStreamBasicDescription       _outFormat;
    
    std::unique_ptr<Byte>           _buffer;
    UInt32                          _bufferSize;
    UInt32                          _bufferTimeMSec;
    UInt32                          _bufferSizeUsed;
}
@end

@implementation AudioStreamDecoder

typedef struct
{
    void*                           mediaStream;
    void*                           audioStreamPacketsInfo;
    UInt64                          srcFilePos;
    AudioStreamBasicDescription     srcFormat;
    UInt32                          srcSizePerPacket;
} AudioFileIO, *AudioFileIOPtr;

-(instancetype)init:(id<MediaStreamSourceProtocol>)mediaSource WithLinierPCMOutputBufferMsec:(UInt32)outputBufferMsec;
{
    _media = mediaSource;
    // Get source format description.
    [_media getStreamDescription:&_inFormat];
  
    // Configure output format.
    _outFormat.mSampleRate = 44100;
    _outFormat.mFormatID = kAudioFormatLinearPCM;
    _outFormat.mChannelsPerFrame = 2;
    _outFormat.mBitsPerChannel = 32;
    _outFormat.mBytesPerPacket = _outFormat.mBytesPerFrame =
    (_outFormat.mBitsPerChannel/8) * _outFormat.mChannelsPerFrame;
    _outFormat.mFramesPerPacket = 1;
    //_outFormat = kLinearPCMFormatFlagIsPacked | kLinearPCMFormatFlagIsSignedInteger; // little-endian
    _outFormat.mFormatFlags =  kLinearPCMFormatFlagIsFloat; // little-endian
    _outFormat.mReserved = 0;
    
    // Initialize output buffer
    _bufferSize = (_outFormat.mBytesPerFrame*_outFormat.mSampleRate) * (float)(outputBufferMsec / 1000.0); // Time sec buffer.
    _bufferTimeMSec = outputBufferMsec;
    _buffer = std::unique_ptr<Byte>(new Byte[_bufferSize]);
    return self;
}

-(instancetype)init:(id<MediaStreamSourceProtocol>)mediaSource WithCustomOutputFormat:(AudioStreamBasicDescription*)outFormat OutputBufferSize:(UInt32)outputBufferSize
{
    assert(outFormat && mediaSource && outputBufferSize > 0);
    _media = mediaSource;
    // Get source format description.
    [_media getStreamDescription:&_inFormat];
    
    // Configure output format.
    memcpy(&_outFormat, outFormat, sizeof(AudioStreamBasicDescription));
    
    // Initialize output buffer
    _bufferSize = outputBufferSize;
    _bufferTimeMSec = 0;
    _buffer = std::unique_ptr<Byte>(new Byte[_bufferSize]);
    return self;
}

-(void)reset
{
    if(_converter){
        AudioConverterReset(_converter);
    }
}

-(void)closeAndInvalidate
{
    // Dispose audio converter.
    if(_converter != nil){
        AudioConverterDispose(_converter);
        _converter = nil;
    }
    
    if(_buffer != nil){
        _buffer.reset();
        _bufferSize = 0;
    }
    
    _media = nil;
}

OSStatus
AudioConverterComplexInputDataProcMy(AudioConverterRef  inAudioConverter,
                                     UInt32* ioNumberDataPackets,
                                     AudioBufferList *ioData,
                                     AudioStreamPacketDescription * __nullable * __nullable outDataPacketDescription,
                                     void* inUserData)
{
    OSStatus error = 0;
    //UInt32 outSize = 0;
    
    AudioFileIOPtr afio = (AudioFileIOPtr)inUserData;
    id<MediaStreamSourceProtocol> mediaSource = (__bridge id<MediaStreamSourceProtocol>)afio->mediaStream;
    AudioStreamPacketsInfo* packetsInfo = (__bridge AudioStreamPacketsInfo*)afio->audioStreamPacketsInfo;
    
    // figure out how much to read
//    UInt32 maxPackets = afio->srcBufferSize / afio->srcSizePerPacket;
//    if (*ioNumberDataPackets > maxPackets)
//        *ioNumberDataPackets = maxPackets;
    
    if(![mediaSource readPackets:NSMakeRange(afio->srcFilePos, *ioNumberDataPackets) InPacketsInfoObject:packetsInfo])
        return -50; // Need more data.
    
    // advance input file packet position
    afio->srcFilePos += *ioNumberDataPackets;
    
    // put the data pointer into the buffer list
    ioData->mBuffers[0].mData = (void*)packetsInfo.data;
    ioData->mBuffers[0].mDataByteSize = packetsInfo.dataSize;
    ioData->mBuffers[0].mNumberChannels = afio->srcFormat.mChannelsPerFrame;
    
    // don't forget the packet descriptions if required
    if (outDataPacketDescription)
        *outDataPacketDescription = packetsInfo.streamPacketsDesc;
    return error;
}

-(OSStatus)decode:(long)audioPacketOffset OutPacketsNum:(UInt32*)decodedPacketsNum
{
    assert(decodedPacketsNum);
    
    OSStatus error = noErr;
    // Create converter.
    if( _converter == nil ){
        error = AudioConverterNew(&_inFormat, &_outFormat, &_converter);
        if(error != noErr) return error;
    }
    
    //NSData* dataBuffer = [[NSMutableData alloc] initWithLength:32768];
    AudioStreamPacketsInfo* packetsInfo = [[AudioStreamPacketsInfo alloc] init:32768];
    
    // Setup source buffers and data proc info struct.
    AudioFileIO afio = {};
    afio.srcFilePos = audioPacketOffset;
    afio.srcSizePerPacket = [_media getPacketSizeInBytes];
    afio.mediaStream = (__bridge void*)_media;
    afio.audioStreamPacketsInfo = (__bridge void*)packetsInfo;
    
    //afio.srcBufferSize = (UInt32)dataBuffer.length;
    //afio.srcBuffer = (char*)dataBuffer.bytes;
    //memmove(&afio.srcFormat, &_inFormat, sizeof(AudioStreamBasicDescription));
    //afio.packetDescriptions = (AudioStreamPacketDescription*)malloc(100 * sizeof(AudioStreamPacketDescription));
    
    // Set up output buffer list.
    AudioBufferList fillBufferList = {};
    fillBufferList.mNumberBuffers = 1;
    fillBufferList.mBuffers[0].mNumberChannels = _outFormat.mChannelsPerFrame;
    fillBufferList.mBuffers[0].mDataByteSize = _bufferSize;
    fillBufferList.mBuffers[0].mData = _buffer.get();

    // Convert data
    UInt32 ioOutputDataPackets = _bufferSize / _outFormat.mBytesPerPacket;
    AudioStreamPacketDescription* outPacketDesc = NULL;//(AudioStreamPacketDescription*)malloc(ioOutputDataPackets * sizeof(AudioStreamPacketDescription));
    
    error = AudioConverterFillComplexBuffer(_converter, &AudioConverterComplexInputDataProcMy, &afio, &ioOutputDataPackets,
                                            &fillBufferList, outPacketDesc);
    
    if(error == noErr)
    {
        *decodedPacketsNum = (UInt32)(afio.srcFilePos - audioPacketOffset);
        _bufferSizeUsed = fillBufferList.mBuffers[0].mDataByteSize;
    }
    
    // Clean up.
    //free(afio.packetDescriptions);
    return error;
}

-(UInt32)fillAudioPCMBuffer:(AVAudioPCMBuffer*)audioBuffer
{
    if(_bufferSizeUsed == 0)
        return 0;
    
    int framesCtMax =  audioBuffer.frameCapacity / _outFormat.mBytesPerFrame;
    int framesCt = MIN(framesCtMax, _bufferSizeUsed / _outFormat.mBytesPerFrame);
    
    const float* fSamples = (const float*)_buffer.get();
    //const int* nSamples = (const int*)_buffer.get();
    int loop = 0;
    
    for(int i=0; i<framesCt; i++)
    {
        audioBuffer.floatChannelData[0][i] = fSamples[loop];   // Left
        audioBuffer.floatChannelData[1][i] = fSamples[loop+1]; // Right
        //audioBuffer.int32ChannelData[0][i] = nSamples[loop];   // Left
        //audioBuffer.int32ChannelData[1][i] = nSamples[loop+1]; // Right
loop += 2;
    }
    
    audioBuffer.frameLength = framesCt;
    return _bufferSizeUsed;
}

-(UInt32)getOutputBufferSize
{
    return _bufferSize;
}

-(BOOL)getOutputStreamDescriptor:(AudioStreamBasicDescription*)outStreamDesc
{
    memcpy(outStreamDesc, &_outFormat, sizeof(AudioStreamBasicDescription));
    return YES;
}

@end
