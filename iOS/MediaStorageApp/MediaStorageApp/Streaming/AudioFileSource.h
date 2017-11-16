//
//  AudioFileSource.h
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 11/11/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>
#import "StreamingSession.h"
#import "WebApiAudioStreamReaderSettings.h"

@interface AudioFileSource : NSObject<MediaStreamSourceProtocol>

-(instancetype)init:(StreamingSession* __weak)session MediaId:(NSURL*)urlMedia FileType:(AudioFileTypeID)fileTypeId;

-(bool)getStreamDescription:(AudioStreamBasicDescription*)streamDescOut;

-(UInt32)getPacketSizeInBytes;

-(AudioStreamPacketsInfo*)readPackets:(NSRange)range;

-(bool)readPackets:(NSRange)range InPacketsInfoObject:(AudioStreamPacketsInfo*)packetsInfo;

-(void)closeAndInvalidate;

@end
