//
//  ImageResourceRequest.h
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 10/1/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "ApiRequestBase.h"

@interface ImageResourceRequest : ApiRequestBase

-(instancetype)init:(NSString*)sessionKey ImageId:(NSString*)imageId SizeType:(NSString*)sizeType;

-(void)makeRequest: (void (^)(NSData* __strong imageData))callback;

@end
