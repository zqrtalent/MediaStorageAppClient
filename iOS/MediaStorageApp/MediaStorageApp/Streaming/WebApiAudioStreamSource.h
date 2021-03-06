//
//  WebApiAudioStreamReader.h
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 10/7/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>
#import "StreamingSession.h"
#import "WebApiAudioStreamReaderSettings.h"

@interface WebApiAudioStreamSource : NSObject<MediaStreamSourceProtocol>

-(instancetype)init:(StreamingSession* __weak)session MediaId:(NSString*)mediaId StreamDescription:(const AudioStreamBasicDescription*)streamDesc WithSettings:(WebApiAudioStreamReaderSettings*)settings;

-(bool)getStreamDescription:(AudioStreamBasicDescription*)streamDescOut;

-(AudioStreamPacketsInfo*)readPackets:(NSRange)range;

-(StreamReadPacketStatus)readPackets:(NSRange)range InPacketsInfoObject:(AudioStreamPacketsInfo*)packetsInfo;

-(long)timeMsecOffset2PacketOffset:(UInt32)positionMsec;

-(void)closeAndInvalidate;

@end
