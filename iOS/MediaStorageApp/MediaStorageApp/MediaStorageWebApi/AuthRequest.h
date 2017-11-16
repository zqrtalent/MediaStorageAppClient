//
//  AuthRequest.h
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 9/30/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "ApiRequestBase.h"
#include "DataContracts/SessionInfo.h"

@interface AuthRequest : ApiRequestBase

-(instancetype)init:(NSString*)userName Pass:(NSString*)password Hash:(NSString*)hash;

-(void)makeRequest: (void (^)(SessionInfo* response))callback;

@end
