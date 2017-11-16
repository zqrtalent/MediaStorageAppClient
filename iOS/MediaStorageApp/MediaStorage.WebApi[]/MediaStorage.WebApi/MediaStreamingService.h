//
//  MediaStreamingService.h
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 5/2/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "DataContracts/SessionInfo.h"
#include "DataContracts/MediaLibraryInfo.h"
#include "DataContracts/MediaPackets.h"

@protocol MediaStreamingServiceDelegate
@optional
-(void)OnAuthenticateResponse:(SessionInfo*)sesInfo;
-(void)OnLibraryInfoResponse:(MediaLibraryInfo*)libraryInfo;
-(void)OnAudioPacketsByTimeResponse:(MediaPackets*)packetsInfo;
-(void)OnAudioPacketsByOffsetResponse:(MediaPackets*)packetsInfo;
-(void)OnImageResourceResponse:(NSData*)imageData MimeType:(NSString*)mimeType;
@end

@interface MediaStreamingService : NSObject

-(BOOL)IsInProgress;
-(void)ForceToStop;
-(MediaStreamingService*)init:(id<MediaStreamingServiceDelegate>)delegate;
-(void)Authenticate:(NSString*)userName Password:(NSString*)password;
-(void)GetLibraryInfo:(NSString*)sessionKey;
-(void)GetImageResource:(NSString*)sessionKey ImageId:(NSString*)imageId SizeType:(NSString*)sizeType;
-(void)AudioPacketsByOffset:(NSString*)sessionKey SongId:(NSString*)songId byOffset:(int)offset Packets:(int)numPackets;
-(void)AudioPacketsByTime:(NSString*)sessionKey SongId:(NSString*)songId byMilliSecond:(int)msec Packets:(int)numPackets;

@end
