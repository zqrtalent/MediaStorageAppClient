//
//  ApiRequestBase.h
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 9/30/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef void (^ResponseCallback)(void* decodedBodyData, NSData* data, NSString* errorString);

@interface ApiRequestBase : NSObject

@property (nonatomic, strong) NSString* host;

-(instancetype)init;

-(void)cancelTasksAndInvalidate;

-(void)finishTasksAndInvalidate;


-(void)httpGet:(NSString*)urlPathWithQuery WithCallback:(ResponseCallback)callback;

@end
