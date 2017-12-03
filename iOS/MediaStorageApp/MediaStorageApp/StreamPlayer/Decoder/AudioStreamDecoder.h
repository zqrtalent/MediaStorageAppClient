//
//  Mp3Decoder.h
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 1/4/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import "../../Streaming/MediaStreamSource.h"

#ifndef MP3_FRAME_DURATION_MSEC
// 1152 /*samples per packet (frame)*/ / 44100 /*samples per second*/) * 1000 = 26,122449
#define MP3_FRAME_DURATION_MSEC 26.122449
#endif

// Decoder error codes.
typedef NS_ENUM(NSInteger, AudioStreamDecoderError)
{
    Decoder_NoError = noErr,
    Decoder_ErrorNeedMoreData = -50,
    Decoder_ErrorUnavailableData = -52,
    Decoder_UnknownError = -1
};

#pragma pack(1)
typedef struct DecodedAudioInfoStruct
{
    long packetOffset;              // Packets offset used as a starting offset for reading audio packets.
    UInt32 numPackets;              // Number of packets used for decode operation.
    UInt32 durationMsec;            // Decoded audio duration in milliseconds.
    AudioStreamDecoderError status; // Status of decode operation.
    bool isEof;                     // Indicates that there are no more packets available.
} DecodedAudioInfo;

@interface AudioStreamDecoder : NSObject

// Initialize decoder object and set LinierPCM as a output format.
-(instancetype)init:(id<MediaStreamSourceProtocol>)mediaSource WithLinierPCMOutputBufferMsec:(UInt32)outputBufferMsec;

// Initialize decode and use defined format as an output format.
-(instancetype)init:(id<MediaStreamSourceProtocol>)mediaSource WithCustomOutputFormat:(AudioStreamBasicDescription*)outFormat OutputBufferSize:(UInt32)outputBufferBytesSize;

// Reset converter.
-(void)reset;

-(bool)decode:(long)audioPacketOffset AndWriteResultInto:(DecodedAudioInfo&)info;

// TODO: Review code and optimize it!
-(UInt32)fillAudioPCMBuffer:(AVAudioPCMBuffer*)audioBuffer;

-(UInt32)getOutputBufferSize;

-(BOOL)getOutputStreamDescriptor:(AudioStreamBasicDescription*)outStreamDesc;

-(void)closeAndInvalidate;

@end
