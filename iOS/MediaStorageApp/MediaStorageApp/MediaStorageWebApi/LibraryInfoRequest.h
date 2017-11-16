//
//  LibraryInfoRequest.h
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 10/1/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "ApiRequestBase.h"
#include "DataContracts/MediaLibraryInfo.h"

@interface LibraryInfoRequest : ApiRequestBase

-(instancetype)init:(NSString*)sessionKey;

-(void)makeRequest: (void (^)(MediaLibraryInfo* response))callback;

@end
