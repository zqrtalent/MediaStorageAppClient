//
//  Mp3Decoder.m
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 1/4/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "Mp3Decoder.h"

@interface Mp3Decoder()
{
@private
    id<MediaStreamProtocol>     _media;
    AudioConverterRef           _converter;
    AudioStreamBasicDescription  _sourceFormat;
    AudioStreamBasicDescription  _destinationFormat;
    void*                       _buffer;
    UInt32                      _bufferSize;
    UInt32                      _bufferTimeMSec;
    UInt32                      _bufferSizeUsed;
}
@end

@implementation Mp3Decoder

typedef struct {
    void*                        mediaStream;
    UInt64                       srcFilePos;
    char *                       srcBuffer;
    UInt32                       srcBufferSize;
    AudioStreamBasicDescription  srcFormat;
    UInt32                       srcSizePerPacket;
    UInt32                       numPacketsPerRead;
    AudioStreamPacketDescription *packetDescriptions;
} AudioFileIO, *AudioFileIOPtr;

OSStatus
AudioConverterComplexInputDataProcMy(AudioConverterRef  inAudioConverter,
                                     UInt32* ioNumberDataPackets,
                                     AudioBufferList *ioData,
                                     AudioStreamPacketDescription * __nullable * __nullable outDataPacketDescription,
                                     void* inUserData)
{
    OSStatus error;
    AudioFileIOPtr afio = (AudioFileIOPtr)inUserData;
    id<MediaStreamProtocol> media = (__bridge id<MediaStreamProtocol>)afio->mediaStream;
    
    // figure out how much to read
    UInt32 maxPackets = afio->srcBufferSize / afio->srcSizePerPacket;
    if (*ioNumberDataPackets > maxPackets) *ioNumberDataPackets = maxPackets;
    
    UInt32 outSize = 0;
    
    error = 0;
    [media readPacketData:*ioNumberDataPackets PacketOffset:(UInt32)afio->srcFilePos Buffer:afio->srcBuffer GivenSize:afio->srcBufferSize PacketDesc:afio->packetDescriptions OutSize:&outSize];
    if(!outSize)
        return -50; // Need data.
    
    //NSLog(@"decode pos: %d size: %d", (int)afio->srcFilePos, outSize);
    
    // advance input file packet position
    afio->srcFilePos += *ioNumberDataPackets;
    
    // put the data pointer into the buffer list
    ioData->mBuffers[0].mData = afio->srcBuffer;
    ioData->mBuffers[0].mDataByteSize = outSize;
    ioData->mBuffers[0].mNumberChannels = afio->srcFormat.mChannelsPerFrame;
    
    // don't forget the packet descriptions if required
    if (outDataPacketDescription) {
        if (afio->packetDescriptions) {
            *outDataPacketDescription = afio->packetDescriptions;
        } else {
            *outDataPacketDescription = NULL;
        }
    }
    
    return error;
}

-(void)dealloc{
    if(_converter != nil){    // Dispose converter.
        AudioConverterDispose(_converter);
        _converter = nil;
    }
    
    if(_buffer != nil){
        free(_buffer);
        _buffer = nil;
        _bufferSize = 0;
    }
    
    _media = nil;
}

-(Mp3Decoder*)init:(id<MediaStreamProtocol>)media OutputBufferMsec:(int)outputBufferMsec{
    _media = media;
    // Get source format description.
    [_media getStreamDescription:&_sourceFormat];
  
    /*
    // Setup the output file format.
    _destinationFormat.mSampleRate = _sourceFormat.mSampleRate;
    _destinationFormat.mFormatID = kAudioFormatLinearPCM;
    _destinationFormat.mChannelsPerFrame = _sourceFormat.mChannelsPerFrame;
    _destinationFormat.mBitsPerChannel = 32;
    _destinationFormat.mBytesPerPacket = _destinationFormat.mBytesPerFrame = (_destinationFormat.mBitsPerChannel/8) * _destinationFormat.mChannelsPerFrame;
    _destinationFormat.mFramesPerPacket = 1;
    //_destinationFormat = kLinearPCMFormatFlagIsPacked | kLinearPCMFormatFlagIsSignedInteger; // little-endian
    _destinationFormat.mFormatFlags =  kLinearPCMFormatFlagIsFloat; // little-endian
    _destinationFormat.mReserved = 0;
     */
    
    _destinationFormat.mSampleRate = 44100;
    _destinationFormat.mFormatID = kAudioFormatLinearPCM;
    _destinationFormat.mChannelsPerFrame = 2;
    _destinationFormat.mBitsPerChannel = 32;
    _destinationFormat.mBytesPerPacket = _destinationFormat.mBytesPerFrame = (_destinationFormat.mBitsPerChannel/8) * _destinationFormat.mChannelsPerFrame;
    _destinationFormat.mFramesPerPacket = 1;
    //_destinationFormat = kLinearPCMFormatFlagIsPacked | kLinearPCMFormatFlagIsSignedInteger; // little-endian
    _destinationFormat.mFormatFlags =  kLinearPCMFormatFlagIsFloat; // little-endian
    _destinationFormat.mReserved = 0;
    
    // Initialize output buffer
    _bufferSize = (_destinationFormat.mBytesPerFrame*_destinationFormat.mSampleRate) * (float)(outputBufferMsec / 1000.0); // Time sec buffer.
    _bufferTimeMSec = outputBufferMsec;
    _buffer = malloc(_bufferSize);
    return self;
}

-(int)decode:(UInt64)packetIndex decodedPacketSizeOut:(UInt32*)packetSizeOut {
    OSStatus error = noErr;
    // Create converter.
    if( _converter == nil ){
        error = AudioConverterNew(&_sourceFormat, &_destinationFormat, &_converter);
        if(error != noErr) return error;
    }
    
    NSData* dataBuffer = [[NSMutableData alloc] initWithLength:32768];
    
    // Setup source buffers and data proc info struct.
    AudioFileIO afio = {};
    afio.mediaStream = (__bridge void*)_media;
    afio.srcBufferSize = (UInt32)dataBuffer.length;
    afio.srcBuffer = (char*)dataBuffer.bytes;
    afio.srcFilePos = packetIndex;
    afio.srcSizePerPacket = [_media getPacketSizeInBytes];
    memmove(&afio.srcFormat, &_sourceFormat, sizeof(_sourceFormat));
    afio.packetDescriptions = (AudioStreamPacketDescription*)malloc(100 * sizeof(AudioStreamPacketDescription));
    
    // Set up output buffer list.
    AudioBufferList fillBufferList = {};
    fillBufferList.mNumberBuffers = 1;
    fillBufferList.mBuffers[0].mNumberChannels = _destinationFormat.mChannelsPerFrame;
    fillBufferList.mBuffers[0].mDataByteSize = _bufferSize;
    fillBufferList.mBuffers[0].mData = _buffer;

    // Convert data
    UInt32 ioOutputDataPackets = _bufferSize / _destinationFormat.mBytesPerPacket;
    AudioStreamPacketDescription* outPacketDesc = NULL;//(AudioStreamPacketDescription*)malloc(ioOutputDataPackets * sizeof(AudioStreamPacketDescription));
    
    error = AudioConverterFillComplexBuffer(_converter, &AudioConverterComplexInputDataProcMy, &afio, &ioOutputDataPackets, &fillBufferList, outPacketDesc);
    if(error == noErr){
        *packetSizeOut = (UInt32)(afio.srcFilePos - packetIndex);
        _bufferSizeUsed = fillBufferList.mBuffers[0].mDataByteSize;
    }
    
    // Clean up.
    free(afio.packetDescriptions);
    //[dataBuffer dealloc];
    return error;
}

-(UInt32)fillAudioPCMBuffer:(AVAudioPCMBuffer*)audioBuffer{
    if(_bufferSizeUsed == 0)
        return 0;
    
    int framesCtMax =  audioBuffer.frameCapacity / _destinationFormat.mBytesPerFrame;
    int framesCt = MIN(framesCtMax, _bufferSizeUsed / _destinationFormat.mBytesPerFrame);
    
    const float* fSamples = (const float*)_buffer;
    int loop = 0;
    
    for(int i=0; i<framesCt; i++){
        audioBuffer.floatChannelData[0][i] = fSamples[loop];   // Left
        audioBuffer.floatChannelData[1][i] = fSamples[loop+1]; // Right
        loop += 2;
    }
    
    audioBuffer.frameLength = framesCt;
    return _bufferSizeUsed;
}

-(UInt32)getOutputBufferSize{
    return _bufferSize;
}

-(BOOL)getOutputStreamDescriptor:(AudioStreamBasicDescription*)outStreamDesc{
    memcpy(outStreamDesc, &_destinationFormat, sizeof(AudioStreamBasicDescription));
    return YES;
}

@end
