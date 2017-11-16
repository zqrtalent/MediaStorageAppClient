//
//  LibraryInfoRequest.m
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 10/1/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "LibraryInfoRequest.h"

@interface LibraryInfoRequest()

@property (nonatomic, strong) NSString* requestUrl;

@end

@implementation LibraryInfoRequest

-(instancetype)init:(NSString*)sessionKey
{
    self.requestUrl = [NSString stringWithFormat:@"/streaming/api/v1/%@/library/info", sessionKey];
    return [super init];
}

-(void)makeRequest:(void (^)(MediaLibraryInfo* response))callback;
{
    [self httpGet:self.requestUrl WithCallback:^(void* libraryInfo, NSData* data, NSString* errorString)
    {
        if(errorString == nil)
            (callback)((MediaLibraryInfo*)libraryInfo);
    }];
}

#pragma mark - Create HTTP response body object to be deserialized as.

-(Serializable*)createResponseBodyObject
{
    return new MediaLibraryInfo();
}

@end
