//
//  NSString+CString.m
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 10/7/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "NSString+MercuryString.h"

@implementation NSString(CString)

+(NSString*)stringFromMercuryCString:(_string*)pStr
{
    return [NSString stringWithCString:pStr->c_str() encoding:NSUTF8StringEncoding];
}

@end
