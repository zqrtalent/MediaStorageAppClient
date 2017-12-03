//
//  AudioPacketByTimeRequest.h
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 12/3/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "AuthRequest.h"
#include "DataContracts/MediaPackets.h"

@interface AudioPacketsByTimeRequest : AuthRequest

-(instancetype)init:(NSString*)sessionKey SongId:(NSString*)songId TimeMsec:(UInt32)offset NumPackets:(UInt32)packets;

-(void)setQueryParams:(NSString*)sessionKey SongId:(NSString*)songId TimeMsec:(UInt32)offset NumPackets:(UInt32)packets;

-(void)makeRequest: (void (^)(MediaPackets* response))callback;

@end
