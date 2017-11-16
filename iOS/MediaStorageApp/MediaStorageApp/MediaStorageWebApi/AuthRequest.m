//
//  AuthRequest.m
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 9/30/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "AuthRequest.h"

@interface AuthRequest()

@property (nonatomic, strong) NSString* requestUrl;

@end

@implementation AuthRequest

-(instancetype)init:(NSString*)userName Pass:(NSString*)password Hash:(NSString*)hash
{
    self.requestUrl = [NSString stringWithFormat:@"/streaming/api/v1/auth/%@/%@/%@", userName, password, hash];
    return [super init];
}

-(void)makeRequest:(void (^)(SessionInfo* response))callback;
{
    [self httpGet:self.requestUrl WithCallback:^(void* sessionInfo, NSData* data, NSString* errorString)
    {
        if(errorString == nil)
        {
            (callback)((SessionInfo*)sessionInfo);
        }
    }];
}

#pragma mark - Create HTTP response body object to be deserialized as.

-(Serializable*)createResponseBodyObject
{
    return new SessionInfo();
}

@end
