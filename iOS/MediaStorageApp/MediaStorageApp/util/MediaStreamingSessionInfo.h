//
//  MediaStreamingSessionInfo.h
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 6/21/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "DataContracts/SessionInfo.h"

@interface MediaStreamingSessionInfo : NSObject

@property (nonatomic, strong) NSString* SessionKey;

-(instancetype)init:(SessionInfo*)sesInfo;

@end
