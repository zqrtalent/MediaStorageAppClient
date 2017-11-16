//
//  NSString+CString.h
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 10/7/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "_platformCompat/PlatformCompat.h"

@interface NSString(CString)

+(NSString*)stringFromMercuryCString:(_string*)pStr;

@end
