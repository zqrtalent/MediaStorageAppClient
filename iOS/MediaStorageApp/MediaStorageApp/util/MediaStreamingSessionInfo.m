//
//  MediaStreamingSessionInfo.m
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 6/21/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "MediaStreamingSessionInfo.h"

@implementation MediaStreamingSessionInfo

-(instancetype)init:(SessionInfo*)sesInfo
{
    [self init];
    self.SessionKey = [NSString stringWithUTF8String:sesInfo->_sessionKey.c_str()];
    return self;
}

@end
