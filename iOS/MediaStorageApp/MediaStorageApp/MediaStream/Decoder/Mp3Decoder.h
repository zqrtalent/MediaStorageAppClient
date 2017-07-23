//
//  Mp3Decoder.h
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 1/4/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import "../MediaFileStream.h"

@interface Mp3Decoder : NSObject
{
}

-(Mp3Decoder*)init:(id<MediaStreamProtocol>)media OutputBufferMsec:(int)outputBufferMsec;
-(int)decode:(UInt64)packetIndex decodedPacketSizeOut:(UInt32*)packetSizeOut;
-(UInt32)fillAudioPCMBuffer:(AVAudioPCMBuffer*)audioBuffer;
-(UInt32)getOutputBufferSize;
-(BOOL)getOutputStreamDescriptor:(AudioStreamBasicDescription*)outStreamDesc;
@end
