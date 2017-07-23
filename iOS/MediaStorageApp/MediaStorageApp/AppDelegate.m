//
//  AppDelegate.m
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 6/6/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "AppDelegate.h"
#include "MediaStorageRuntimeInfo.h"
#import "MediaPlayerControlCenter.h"
#import "RemoteCommandsProtocol.h"

@interface AppDelegate()<PlayerDelegate, RemoteCommandsProtocol>

@property (nonatomic, strong) MediaPlayerControlCenter* mpControlCenter;
@property (nonatomic, assign) unsigned int currentPlayTimeSec;
@property (nonatomic, assign) float currentPlayTimeSecFloat;

@end

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Override point for customization after application launch.
    self.mpControlCenter = [[MediaPlayerControlCenter alloc] init];
    self.currentPlayTimeSec = 0;
    self.currentPlayTimeSecFloat = 0.0f;
    
    return YES;
}


- (void)applicationWillResignActive:(UIApplication *)application {
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and invalidate graphics rendering callbacks. Games should use this method to pause the game.
}


- (void)applicationDidEnterBackground:(UIApplication *)application {
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}


- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Called as part of the transition from the background to the active state; here you can undo many of the changes made on entering the background.
}


- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}


- (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
    
    // Cleanup runtime info object.
    [[MediaStorageRuntimeInfo sharedInstance] cleanUp];
    // Destroy media player control center object.
    [self.mpControlCenter cleanUp];
    
    // Saves changes in the application's managed object context before the application terminates.
    [self saveContext];
}


#pragma mark - Core Data stack

@synthesize persistentContainer = _persistentContainer;

- (NSPersistentContainer *)persistentContainer {
    // The persistent container for the application. This implementation creates and returns a container, having loaded the store for the application to it.
    @synchronized (self) {
        if (_persistentContainer == nil) {
            _persistentContainer = [[NSPersistentContainer alloc] initWithName:@"MediaStorageApp"];
            [_persistentContainer loadPersistentStoresWithCompletionHandler:^(NSPersistentStoreDescription *storeDescription, NSError *error) {
                if (error != nil) {
                    // Replace this implementation with code to handle the error appropriately.
                    // abort() causes the application to generate a crash log and terminate. You should not use this function in a shipping application, although it may be useful during development.
                    
                    /*
                     Typical reasons for an error here include:
                     * The parent directory does not exist, cannot be created, or disallows writing.
                     * The persistent store is not accessible, due to permissions or data protection when the device is locked.
                     * The device is out of space.
                     * The store could not be migrated to the current model version.
                     Check the error message to determine what the actual problem was.
                    */
                    NSLog(@"Unresolved error %@, %@", error, error.userInfo);
                    abort();
                }
            }];
        }
    }
    
    return _persistentContainer;
}

#pragma mark - Core Data Saving support

- (void)saveContext {
    NSManagedObjectContext *context = self.persistentContainer.viewContext;
    NSError *error = nil;
    if ([context hasChanges] && ![context save:&error]) {
        // Replace this implementation with code to handle the error appropriately.
        // abort() causes the application to generate a crash log and terminate. You should not use this function in a shipping application, although it may be useful during development.
        NSLog(@"Unresolved error %@, %@", error, error.userInfo);
        abort();
    }
}

#pragma mark - PlayerDelegate methods

-(void)onPlayStarted:(BOOL)resumed{
    void (^playStartedBlock)();
    playStartedBlock = ^{
        if(!resumed){ // Start play
            [self.mpControlCenter setActiveRemoteCommands:YES];
            self.mpControlCenter.remoteCommandsDelegate = self;
            
            MediaInfo* playingMedia = [MediaStorageRuntimeInfo sharedInstance].NowPlaying;
            // Initialize now playing info.
            [self.mpControlCenter setNowPlayingTitle:playingMedia.songName];
            [self.mpControlCenter setNowPlayingArtistName:playingMedia.artist];
            [self.mpControlCenter setNowPlayingAlbumName:playingMedia.album];
            [self.mpControlCenter setNowPlayingDuration:(double)playingMedia.DurationInSec];
            [self.mpControlCenter setNowPlayingPlaybackProgress:(double)playingMedia.CurrentPositionInSec];
            [self.mpControlCenter setNowPlayingElapsedPlaybackTime:(double)playingMedia.CurrentPositionInSec];
            [self.mpControlCenter setNowPlayingState:YES];
            [self.mpControlCenter setActiveNowPlayingInfo:YES];
        }
        else{
            [self.mpControlCenter setNowPlayingState:YES];
        }
    };
    
    if([NSThread isMainThread])
        playStartedBlock();
    else
        dispatch_async(dispatch_get_main_queue(), playStartedBlock);
}

-(void)onPlayEnded{
    void (^playEndedBlock)();
    playEndedBlock = ^{
        [self.mpControlCenter setActiveNowPlayingInfo:NO];
    };
    
    if([NSThread isMainThread])
        playEndedBlock();
    else
        dispatch_async(dispatch_get_main_queue(), playEndedBlock);
}

-(void)onPaused{
    void (^playPausedBlock)();
    playPausedBlock = ^{
        [self.mpControlCenter setNowPlayingState:NO];
    };
    
    if([NSThread isMainThread])
        playPausedBlock();
    else
        dispatch_async(dispatch_get_main_queue(), playPausedBlock);
}

-(void)onBufferingStarted{
    //    dispatch_async(dispatch_get_main_queue(), ^(){
    //        NSLog(@"onBufferingStarted");
    //        [[NowPlayInfo sharedInstance].miniPlayerView onBufferingStarted];
    //    });
}

-(void)onBufferingEnded{
    //    dispatch_async(dispatch_get_main_queue(), ^(){
    //        NSLog(@"onBufferingEnded");
    //        [[NowPlayInfo sharedInstance].miniPlayerView onBufferingEnded];
    //    });
}

-(void)onPlayTimeUpdate:(unsigned int)msec{
    self.currentPlayTimeSecFloat = (msec/1000.0);
    int currentTimeSec = (int)self.currentPlayTimeSecFloat;
    if(currentTimeSec != self.currentPlayTimeSec){ //  Dont update time change less than second.
        self.currentPlayTimeSec = currentTimeSec;
        
        // Update now playing info.
        if(self.mpControlCenter){
            void (^playTimeUpdatelock)();
            playTimeUpdatelock = ^{
                //[self.mpControlCenter setNowPlayingPlaybackProgress:(double)currentTimeSec];
                [self.mpControlCenter setNowPlayingElapsedPlaybackTime:(double)currentTimeSec];
                [self.mpControlCenter updateNowPlayingInfo];
            };
            
            if([NSThread isMainThread])
                playTimeUpdatelock();
            else
                dispatch_async(dispatch_get_main_queue(), playTimeUpdatelock);
        }
    }
}

#pragma mark - RemoteCommandsProtocol methods.
-(MPRemoteCommandHandlerStatus)onPlayCommand{
    return MPRemoteCommandHandlerStatusSuccess;
}

-(MPRemoteCommandHandlerStatus)onPauseCommand{
    return MPRemoteCommandHandlerStatusSuccess;
}

-(MPRemoteCommandHandlerStatus)onTogglePlayPauseCommand{
    return MPRemoteCommandHandlerStatusSuccess;
}

-(MPRemoteCommandHandlerStatus)onNextTrackCommand{
    return MPRemoteCommandHandlerStatusSuccess;
}

-(MPRemoteCommandHandlerStatus)onPrevTrackCommand{
    return MPRemoteCommandHandlerStatusSuccess;
}

@end
