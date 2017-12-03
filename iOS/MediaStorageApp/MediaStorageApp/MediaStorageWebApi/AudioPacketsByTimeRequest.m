//
//  AudioPacketByTimeRequest.m
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 12/3/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "AudioPacketsByTimeRequest.h"

@interface AudioPacketsByTimeRequest()
@property (nonatomic, strong) NSString* requestUrl;
@end

@implementation AudioPacketsByTimeRequest

-(instancetype)init:(NSString*)sessionKey SongId:(NSString*)songId TimeMsec:(UInt32)offset NumPackets:(UInt32)packets
{
    [self setQueryParams:sessionKey SongId:songId TimeMsec:offset NumPackets:packets];
    return [super init];
}

-(void)setQueryParams:(NSString*)sessionKey SongId:(NSString*)songId TimeMsec:(UInt32)offset NumPackets:(UInt32)packets
{
    self.requestUrl = [NSString stringWithFormat:@"/streaming/api/v1/%@/audiopackets/%@/time/%d/%d", sessionKey, songId, offset, packets];
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
