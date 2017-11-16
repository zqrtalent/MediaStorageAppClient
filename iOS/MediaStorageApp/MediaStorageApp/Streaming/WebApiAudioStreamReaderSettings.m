//
//  WebApiAudioStreamReaderSettings.m
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 10/7/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "WebApiAudioStreamReaderSettings.h"

@implementation WebApiAudioStreamReaderSettings

+(instancetype)defaultSettings
{
    WebApiAudioStreamReaderSettings * settings = [[WebApiAudioStreamReaderSettings alloc] init];
    settings.allowFileCaching = false;
    settings.memoryCacheSize = 1000*1024;
    return settings;
}

@end
