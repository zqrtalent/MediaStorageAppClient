//
//  WebApiAudioStreamReaderSettings.h
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 10/7/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface WebApiAudioStreamReaderSettings : NSObject

@property (nonatomic, assign) UInt32 memoryCacheSize;
@property (nonatomic, assign) BOOL allowFileCaching;

+(instancetype)defaultSettings;

@end
