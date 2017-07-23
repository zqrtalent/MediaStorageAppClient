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

/*
 Host address: http://45.35.50.10:81
 
 Authentication:
	Http: GET
	Endpoint: /streaming/api/v1/auth/{userName}/{password}/{hash}
	Response: SessionInfo object (using MfcSerializable) or HttpStatusCode.NotFound(404) if unsuccessful.
 
 GetLibraryInfo:
	Http: GET
	Endpoint: /streaming/api/v1/{sessionKey}/library/info"
	Response: MediaLibraryInfo object (using MfcSerializable).
 
 GetImageResource:
	Http: GET {sessionKey}/image/{imageGroupId}/{sizeType}
	Endpoint: /streaming/api/v1/{sessionKey}/image/{imageGroupId}/{sizeType}"
	Response: Image binary data or HttpStatusCode.NotFound(404) if unsuccessful.
 
 AudioPacketsByTime:
	Http: GET
	Endpoint: /streaming/api/v1/{sessionKey}/audiopackets/{songId}/offset/{offset}/{numPackets}"
	Response: MediaPackets object (using MfcSerializable).
 
 AudioPacketsByOffset:
	Http: GET
	Endpoint: /streaming/api/v1/{sessionKey}/audiopackets/{songId}/time/{msec}/{numPackets}"
	Response: MediaPackets object (using MfcSerializable).
 */


@end
