//
//  MediaStreamReader.h
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 10/7/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>
#import "../Models/AudioStreamPacketsInfo.h"

typedef void(^AudioPacketsReadCallback)(AudioStreamPacketsInfo* packetsInfo);

@protocol MediaStreamSourceProtocol

@required

-(bool)getStreamDescription:(AudioStreamBasicDescription*)streamDescOut;

-(UInt32)getPacketSizeInBytes;

-(AudioStreamPacketsInfo*)readPackets:(NSRange)range;

-(bool)readPackets:(NSRange)range InPacketsInfoObject:(AudioStreamPacketsInfo*)packetsInfo;

//-(bool)readPackets:(NSRange)range WithCallback:(AudioPacketsReadCallback)callback;

-(void)closeAndInvalidate;

@end
