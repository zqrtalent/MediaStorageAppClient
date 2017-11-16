//
//  AppDelegate.h
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 6/6/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <CoreData/CoreData.h>
#import "Streaming/StreamingSessionProtocol.h"

class MediaLibraryInfo;
@class StreamPlayerManager;
@class StreamingSession;

@interface AppDelegate : UIResponder <UIApplicationDelegate, StreamingSessionProtocol>

@property (strong, nonatomic) UIWindow *window;

@property (strong, nonatomic) StreamingSession* streamingSession;

@property (strong, nonatomic) StreamPlayerManager* playerManager;

@property (readonly, strong) NSPersistentContainer *persistentContainer;

+(instancetype)sharedInstance;

-(void)saveContext;

-(void)setMediaLibraryInfo:(MediaLibraryInfo*)pInfo;

-(MediaLibraryInfo*)getMediaLibraryInfo;

@end

