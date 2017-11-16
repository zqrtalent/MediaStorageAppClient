//
//  AudioPacketsByOffsetRequest.m
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 10/1/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "AudioPacketsByOffsetRequest.h"

@interface AudioPacketsByOffsetRequest()

@property (nonatomic, strong) NSString* requestUrl;

@end

@implementation AudioPacketsByOffsetRequest

-(instancetype)init:(NSString*)sessionKey SongId:(NSString*)songId Range:(NSRange)range
{
    [self setQueryParams:sessionKey SongId:songId Range:range];
    return [super init];
}

-(void)setQueryParams:(NSString*)sessionKey SongId:(NSString*)songId Range:(NSRange)range
{
    self.requestUrl = [NSString stringWithFormat:@"/streaming/api/v1/%@/audiopackets/%@/offset/%d/%d", sessionKey, songId, (int)range.location, (int)range.length];
}

-(void)makeRequest:(void (^)(MediaPackets* response))callback;
{
    [self httpGet:self.requestUrl WithCallback:^(void* mediaPackets, NSData* data, NSString* errorString)
    {
        if(errorString == nil)
            (callback)((MediaPackets*)mediaPackets);
    }];
}

#pragma mark - Create HTTP response body object to be deserialized as.

-(Serializable*)createResponseBodyObject
{
    return new MediaPackets();
}

@end
