//
//  StreamingSession+ApiRequests.m
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 11/5/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "StreamingSession+ApiRequests.h"
#import "../MediaStorageWebApi/AuthRequest.h"
#import "../MediaStorageWebApi/LibraryInfoRequest.h"
#import "../MediaStorageWebApi/ImageResourceRequest.h"
#import "../MediaStorageWebApi/AudioPacketsByOffsetRequest.h"

@implementation StreamingSession(ApiRequests)

-(AuthRequest*)authRequest:(NSString*)userName Pass:(NSString*)password Hash:(NSString*)hash
{
    auto req = [[AuthRequest alloc] init:userName Pass:password Hash: hash];
    req.host = self.settings.webApiHost;
    return req;
}

-(LibraryInfoRequest*)libraryInfoRequest
{
    auto req = [[LibraryInfoRequest alloc] init:self.sessionId];
    req.host = self.settings.webApiHost;
    return req;
}

-(ImageResourceRequest*)imageResourceRequest:(NSString*)imageId SizeType:(NSString*)sizeType
{
    auto req = [[ImageResourceRequest alloc] init:self.sessionId ImageId:imageId SizeType:sizeType];
    req.host = self.settings.webApiHost;
    return req;
}

-(AudioPacketsByOffsetRequest*)audioPacketsByOffset:(NSString*)songId Range:(NSRange)packetsRange
{
    auto req = [[AudioPacketsByOffsetRequest alloc] init:self.sessionId SongId:songId Range:packetsRange];
    req.host = self.settings.webApiHost;
    return req;
}

@end
