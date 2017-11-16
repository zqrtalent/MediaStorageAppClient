//
//  StreamingSessionSettings.h
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 10/4/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface StreamingSessionSettings : NSObject

@property (nonatomic, strong) NSString* webApiHost;
@property (nonatomic, strong) NSString* webApiKey;
@property (nonatomic, strong) NSString* webApiVer;

+(instancetype)sharedSettings;

@end
