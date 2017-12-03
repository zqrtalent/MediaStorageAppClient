//
//  StreamingSession+ApiRequests.h
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 11/5/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "../Streaming/StreamingSession.h"

@class AuthRequest;
@class LibraryInfoRequest;
@class ImageResourceRequest;
@class AudioPacketsByOffsetRequest;
@class AudioPacketsByTimeRequest;

@interface StreamingSession(ApiRequests)

-(AuthRequest*)authRequest:(NSString*)userName Pass:(NSString*)password Hash:(NSString*)hash;
-(LibraryInfoRequest*)libraryInfoRequest;
-(ImageResourceRequest*)imageResourceRequest:(NSString*)imageId SizeType:(NSString*)sizeType;
-(AudioPacketsByOffsetRequest*)audioPacketsByOffset:(NSString*)songId Range:(NSRange)packetsRange;
-(AudioPacketsByTimeRequest*)audioPacketsByTime:(NSString*)songId Offset:(UInt32)timeMSec NumPackets:(UInt32)packets;

@end
