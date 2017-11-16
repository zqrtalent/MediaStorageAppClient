//
//  AudioPacketsByOffsetRequest.h
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 10/1/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "ApiRequestBase.h"
#include "DataContracts/MediaPackets.h"

@interface AudioPacketsByOffsetRequest : ApiRequestBase

-(instancetype)init:(NSString*)sessionKey SongId:(NSString*)songId Range:(NSRange)range;

-(void)setQueryParams:(NSString*)sessionKey SongId:(NSString*)songId Range:(NSRange)range;

-(void)makeRequest: (void (^)(MediaPackets* response))callback;

@end
