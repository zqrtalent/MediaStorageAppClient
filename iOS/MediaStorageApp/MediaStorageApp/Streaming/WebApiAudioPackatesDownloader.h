//
//  WebApiAudioPackatesDownloader.h
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 10/7/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AudioToolBox/AudioToolBox.h>
#import "AudioPacketsDownloaderProtocol.h"

@class StreamingSession;
@class AudioStreamPacketsInfo;
@interface WebApiAudioPackatesDownloader : NSObject

-(instancetype)init:(id<AudioPacketsDownloaderProtocol>)delegate Session:(StreamingSession* __weak)session MediaId:(NSString*)mediaId;

-(bool)start:(long)packetOffset;
-(bool)resume;
-(bool)seek:(long)packetOffset;
-(bool)pause;
-(bool)stop;

-(bool)checkIfPaused;
-(bool)checkIfEof;

-(bool)checkIfDownloadCompleted;
-(bool)checkAudioPacketsAvailability:(NSRange)packetsRange;

// Converts time offset (In milliseconds) into packet offset.
-(long)timeMsecOffset2PacketOffset:(UInt32)positionMsec;

-(bool)copyAudioPacketsData:(NSRange)range PacketsInfo:(AudioStreamPacketsInfo*)packets OutSize:(UInt32*)outDataSize;

-(UInt32)getNumberOfPackets;

@end
