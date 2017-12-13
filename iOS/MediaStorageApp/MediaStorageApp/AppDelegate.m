//
//  AppDelegate.m
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 6/6/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "AppDelegate.h"
#import "MediaPlayerControlCenter/MediaPlayerControlCenter.h"
#import "MediaPlayerControlCenter/RemoteCommandsProtocol.h"
#import "Streaming/StreamingSession.h"
#import "StreamPlayer/StreamPlayerManager.h"
#import "StreamPlayer/PlayerDelegate.h"
#import "Streaming/StreamingSessionSettings.h"
#include "MediaStorageWebApi/DataContracts/MediaLibraryInfo.h"

@interface AppDelegate()<PlayerDelegate, RemoteCommandsProtocol>
{
    std::unique_ptr<MediaLibraryInfo> _mlInfo;
}

@property (nonatomic, strong) MediaPlayerControlCenter* mpControlCenter;
@property (nonatomic, assign) unsigned int currentPlayTimeSec;
@property (nonatomic, assign) float currentPlayTimeSecFloat;

@end

@implementation AppDelegate

+(instancetype)sharedInstance
{
    return (AppDelegate*)[UIApplication sharedApplication].delegate;
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    // Initialize streaming session instance.
    self.streamingSession = [[StreamingSession alloc] init:[StreamingSessionSettings sharedSettings]];
    
    self.playerManager = [StreamPlayerManager sharedInstance];
    [self.playerManager updateStreamingSession:self.streamingSession];
    
    
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
    
    // Destroy media library info object.
    _mlInfo.reset();
    
    [self.playerManager cleanUp];
    
    // Destroy media player control center object.
    [self.mpControlCenter cleanUp];
    
    // Saves changes in the application's managed object context before the application terminates.
    [self saveContext];
}

-(void)setMediaLibraryInfo:(MediaLibraryInfo*)pInfo
{
    [self.playerManager updateMediaLibraryInfo:pInfo];
    _mlInfo = std::unique_ptr<MediaLibraryInfo>(pInfo);
}

-(MediaLibraryInfo*)getMediaLibraryInfo
{
    return _mlInfo.get();
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

-(void)onPlayStarted:(BOOL)resumed
{
    void (^playStartedBlock)();
    
    __typeof__(self) __weak weakSelf = self;
    playStartedBlock = ^{
        if(!resumed) // Start play
        {
            [weakSelf.mpControlCenter setActiveRemoteCommands:YES];
            weakSelf.mpControlCenter.remoteCommandsDelegate = weakSelf;
            
            AudioMetadataInfo* playingMedia = weakSelf.playerManager.nowPlaying;
            
            // Initialize now playing info.
            [weakSelf.mpControlCenter setNowPlayingTitle:playingMedia.songName];
            [weakSelf.mpControlCenter setNowPlayingArtistName:playingMedia.artistName];
            [weakSelf.mpControlCenter setNowPlayingAlbumName:playingMedia.albumName];
            [weakSelf.mpControlCenter setNowPlayingDuration:(double)playingMedia.durationSec];
            [weakSelf.mpControlCenter setNowPlayingElapsedPlaybackTime:weakSelf.playerManager.elapsedPlaybackTimeSec];
            
            [weakSelf.mpControlCenter setNowPlayingState:YES WithElapsedPlaybackTimeSec:(double)weakSelf.playerManager.elapsedPlaybackTimeSec];
            [weakSelf.mpControlCenter setActiveNowPlayingInfo:YES];
        }
        else
        {
            [weakSelf.mpControlCenter setNowPlayingElapsedPlaybackTime:weakSelf.playerManager.elapsedPlaybackTimeSec];
            [weakSelf.mpControlCenter setNowPlayingState:YES WithElapsedPlaybackTimeSec:(double)weakSelf.playerManager.elapsedPlaybackTimeSec];
        }
    };
    
    if([NSThread isMainThread])
        playStartedBlock();
    else
        dispatch_async(dispatch_get_main_queue(), playStartedBlock);
}

-(void)onPlayEnded
{
    void (^playEndedBlock)();
    __typeof__(self) __weak weakSelf = self;
    playEndedBlock = ^{
        [weakSelf.mpControlCenter setActiveNowPlayingInfo:NO];
    };
    
    if([NSThread isMainThread])
        playEndedBlock();
    else
        dispatch_async(dispatch_get_main_queue(), playEndedBlock);
}

-(void)onPaused
{
    void (^playPausedBlock)();
    __typeof__(self) __weak weakSelf = self;
    playPausedBlock = ^{
        [weakSelf.mpControlCenter setNowPlayingElapsedPlaybackTime: weakSelf.playerManager.elapsedPlaybackTimeSec];
        [weakSelf.mpControlCenter setNowPlayingState:NO];
    };
    
    if([NSThread isMainThread])
        playPausedBlock();
    else
        dispatch_async(dispatch_get_main_queue(), playPausedBlock);
}

-(void)onBufferingStarted
{
    //    dispatch_async(dispatch_get_main_queue(), ^(){
    //        NSLog(@"onBufferingStarted");
    //        [[NowPlayInfo sharedInstance].miniPlayerView onBufferingStarted];
    //    });
}

-(void)onBufferingEnded
{
    //    dispatch_async(dispatch_get_main_queue(), ^(){
    //        NSLog(@"onBufferingEnded");
    //        [[NowPlayInfo sharedInstance].miniPlayerView onBufferingEnded];
    //    });
}

-(void)onPlayTimeUpdate:(unsigned int)msec
{
    self.currentPlayTimeSecFloat = (msec/1000.0);
    int currentTimeSec = (int)self.currentPlayTimeSecFloat;
    if(currentTimeSec != self.currentPlayTimeSec) //  Dont update time change less than second.
    {
        int currTimeSecOld = self.currentPlayTimeSec;
        self.currentPlayTimeSec = currentTimeSec;
        
        // Update now playing info.
        if(self.mpControlCenter)
        {
            // If seek backwards or replay was performed.
            if(currTimeSecOld > currentTimeSec || (currentTimeSec -  currTimeSecOld) > 1)
            {
                void (^playTimeUpdatelock)();
                __typeof__(self) __weak weakSelf = self;
                playTimeUpdatelock = ^{
                    [weakSelf.mpControlCenter setNowPlayingElapsedPlaybackTime:(double)currentTimeSec];
                    [weakSelf.mpControlCenter updateNowPlayingInfo];
                };
                
                if([NSThread isMainThread])
                    playTimeUpdatelock();
                else
                    dispatch_async(dispatch_get_main_queue(), playTimeUpdatelock);
            }
        }
    }
}

#pragma mark - RemoteCommandsProtocol methods.
-(MPRemoteCommandHandlerStatus)onPlayCommand
{
    if([self.playerManager play])
        return MPRemoteCommandHandlerStatusSuccess;
    return MPRemoteCommandHandlerStatusCommandFailed;
}

-(MPRemoteCommandHandlerStatus)onPauseCommand
{
    if([self.playerManager pause])
        return MPRemoteCommandHandlerStatusSuccess;
    return MPRemoteCommandHandlerStatusCommandFailed;
}

-(MPRemoteCommandHandlerStatus)onTogglePlayPauseCommand
{
    if([self.playerManager playPauseToggle])
        return MPRemoteCommandHandlerStatusSuccess;
    return MPRemoteCommandHandlerStatusCommandFailed;
}

-(MPRemoteCommandHandlerStatus)onNextTrackCommand
{
    if([self.playerManager playNext])
        return MPRemoteCommandHandlerStatusSuccess;
    return MPRemoteCommandHandlerStatusCommandFailed;
}

-(MPRemoteCommandHandlerStatus)onPrevTrackCommand
{
    if([self.playerManager playPrev])
        return MPRemoteCommandHandlerStatusSuccess;
    return MPRemoteCommandHandlerStatusCommandFailed;
}

-(MPRemoteCommandHandlerStatus)onChangePlaybackPosition:(NSTimeInterval)position
{
    if(position < 0.0 || position > (float)self.playerManager.nowPlaying.durationSec)
        return MPRemoteCommandHandlerStatusCommandFailed;
    
    [self.playerManager.player seekAtTime:(float)position];
    return MPRemoteCommandHandlerStatusSuccess;
}

#pragma mark - Streaming session protocol methods

-(void)streamingSession:(StreamingSession*)sess Authenticated:(BOOL)status
{
}

@end

@implementation NSURLRequest(DataController)
+ (BOOL)allowsAnyHTTPSCertificateForHost:(NSString *)host
{
    return YES;
}
@end
