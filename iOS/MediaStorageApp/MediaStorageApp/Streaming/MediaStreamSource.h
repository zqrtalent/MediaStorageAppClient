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

typedef NS_ENUM(NSInteger, StreamReadPacketStatus)
{
    StreamReadPacketStatus_Success = noErr,         // Read completed successfully.
    StreamReadPacketStatus_ReadError = -1,          // Read completed with error, could be multiple reason for that.
    StreamReadPacketStatus_InvalidOffset = -2,      // Invalid packet offset, presumably greater than EOF packet index.
    StreamReadPacketStatus_DownloadScheduled = -3,  // Read operation wasn't successfull but download was initiated afterwards.

};

@protocol MediaStreamSourceProtocol

@required

-(bool)getStreamDescription:(AudioStreamBasicDescription*)streamDescOut;

-(UInt32)getPacketSizeInBytes;

-(UInt32)getNumberOfPackets;

-(AudioStreamPacketsInfo*)readPackets:(NSRange)range;


// @packetsInfo - packets info container object that holds (Existing info will be destroyed) read packets data info.
-(StreamReadPacketStatus)readPackets:(NSRange)range InPacketsInfoObject:(AudioStreamPacketsInfo*)packetsInfo;

-(long)timeMsecOffset2PacketOffset:(UInt32)positionMsec;

//-(bool)readPackets:(NSRange)range WithCallback:(AudioPacketsReadCallback)callback;

-(void)closeAndInvalidate;

@end
