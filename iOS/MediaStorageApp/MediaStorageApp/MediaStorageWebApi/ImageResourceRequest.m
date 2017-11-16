//
//  ImageResourceRequest.m
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 10/1/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "ImageResourceRequest.h"

@interface ImageResourceRequest()

@property (nonatomic, strong) NSString* requestUrl;

@end

@implementation ImageResourceRequest

-(instancetype)init:(NSString*)sessionKey ImageId:(NSString*)imageId SizeType:(NSString*)sizeType;
{
    self.requestUrl = [NSString stringWithFormat:@"/streaming/api/v1/%@/image/%@/%@", sessionKey, imageId, sizeType];
    return [super init];
}

-(void)makeRequest:(void (^)(NSData* __strong __nullable imageData))callback;
{
    [self httpGet:self.requestUrl WithCallback:^(void* pObject, NSData* data, NSString* errorString)
    {
        if(errorString == nil)
            (callback)(data);
    }];
}

@end
