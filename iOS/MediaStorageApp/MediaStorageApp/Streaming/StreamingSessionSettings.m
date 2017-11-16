//
//  StreamingSessionSettings.m
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 10/4/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "StreamingSessionSettings.h"

@implementation StreamingSessionSettings

-(instancetype)init
{
    //self.webApiHost = @"http://45.35.50.10:81";
    self.webApiHost = @"https://localhost:5001";
    self.webApiKey = @"nokey";
    self.webApiVer = @"1.0";
    return self;
}

+(instancetype)sharedSettings
{
    static StreamingSessionSettings* settings;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        settings = [[StreamingSessionSettings alloc] init];
    });
    return settings;
}

@end

