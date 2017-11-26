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

@interface AudioStreamDecoder : NSObject
{
}

// Initialize decoder object and set LinierPCM as a output format.
-(instancetype)init:(id<MediaStreamSourceProtocol>)mediaSource WithLinierPCMOutputBufferMsec:(UInt32)outputBufferMsec;

// Initialize decore and use defined format as an output format.
-(instancetype)init:(id<MediaStreamSourceProtocol>)mediaSource WithCustomOutputFormat:(AudioStreamBasicDescription*)outFormat OutputBufferSize:(UInt32)outputBufferBytesSize;

// Reset converter.
-(void)reset;

-(OSStatus)decode:(long)audioPacketOffset OutPacketsNum:(UInt32*)decodedPacketsNum;


// TODO: Review code and optimize it!
-(UInt32)fillAudioPCMBuffer:(AVAudioPCMBuffer*)audioBuffer;

-(UInt32)getOutputBufferSize;

-(BOOL)getOutputStreamDescriptor:(AudioStreamBasicDescription*)outStreamDesc;

-(void)closeAndInvalidate;

@end
