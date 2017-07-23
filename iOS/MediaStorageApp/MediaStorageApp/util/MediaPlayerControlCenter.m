//
//  MediaPlayerControlCenter.m
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 6/25/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "MediaPlayerControlCenter.h"
#import <AVFoundation/AVFoundation.h>

@interface MediaPlayerControlCenter()
{
}

@property (nonatomic, strong) NSMutableDictionary* nowPlayingInfo;
@property (nonatomic, assign) BOOL isActiveMPNowPlayingInfo;
@property (nonatomic, assign) BOOL isActiveRemoteCommands;
@property (nonatomic, strong) MPMediaItemArtwork* mediaArtwork;

-(void)initRemoteCommandCenter:(BOOL)init;

@end

@implementation MediaPlayerControlCenter

-(instancetype)init{
    [super init];
    
    self.isActiveMPNowPlayingInfo = NO;
    self.isActiveRemoteCommands = NO;
    self.remoteCommandsDelegate = nil;
    
    self.nowPlayingInfo = [[NSMutableDictionary alloc] initWithDictionary:@{ MPMediaItemPropertyTitle: @"",
                                                                        MPMediaItemPropertyArtist: @"",
                                                                        MPMediaItemPropertyAlbumTitle: @"",
                                                                        /*MPMediaItemPropertyArtwork: self.mediaArtwork,*/
                                                                        MPNowPlayingInfoPropertyElapsedPlaybackTime: [NSNumber numberWithDouble:0.0],
                                                                        MPNowPlayingInfoPropertyPlaybackRate: [NSNumber numberWithDouble:1.0],
                                                                        MPMediaItemPropertyPlaybackDuration: [NSNumber numberWithDouble:0.0],
                                                                        MPNowPlayingInfoPropertyMediaType: [NSNumber numberWithInt: MPNowPlayingInfoMediaTypeAudio]
                                                                            } copyItems:YES];
    
    NSError* error = [[NSError alloc] init];
    // Set playback category.
    if(![[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryPlayback error:&error])
        NSLog(@"Couldn't set category AVAudioSessionCategoryPlayback!");
    
    // Activate audio session.
    if(![[AVAudioSession sharedInstance] setActive:YES error:&error])
        NSLog(@"Couldn't activate AVAudioSession!");
    
    return self;
}

-(void)cleanUp{
    [self setActiveRemoteCommands:NO];
    [self setActiveNowPlayingInfo:NO];
    
    self.remoteCommandsDelegate = nil;
}

-(void)setActiveNowPlayingInfo:(BOOL)active{
    if(self.isActiveMPNowPlayingInfo != active){
        // Set isActive flag.
        self.isActiveMPNowPlayingInfo = active;
        
        if(!active)
            [MPNowPlayingInfoCenter defaultCenter].nowPlayingInfo = nil;
        else
            [self updateNowPlayingInfo];
    }
}

-(void)setActiveRemoteCommands:(BOOL)active{
    if(self.isActiveRemoteCommands != active)
        [self initRemoteCommandCenter:active];
    
    if(active)
        [[UIApplication sharedApplication] beginReceivingRemoteControlEvents];
    else
        [[UIApplication sharedApplication] endReceivingRemoteControlEvents];
        
    // Set isActive flag.
    self.isActiveRemoteCommands = active;
}

-(void)initRemoteCommandCenter:(BOOL)init{
    MPRemoteCommandCenter* commandCenter = [MPRemoteCommandCenter sharedCommandCenter];
    if(init){
        [commandCenter.playCommand addTarget:self action:@selector(onPlayTrack)];
        [commandCenter.pauseCommand addTarget:self action:@selector(onPauseTrack)];
        [commandCenter.togglePlayPauseCommand addTarget:self action:@selector(onTogglePlayPauseTrack)];
        [commandCenter.nextTrackCommand addTarget:self action:@selector(onNextTrack)];
    }
    else{
        [commandCenter.playCommand removeTarget:self action:@selector(onPlayTrack)];
        [commandCenter.pauseCommand removeTarget:self action:@selector(onPauseTrack)];
        [commandCenter.togglePlayPauseCommand removeTarget:self action:@selector(onTogglePlayPauseTrack)];
        [commandCenter.nextTrackCommand removeTarget:self action:@selector(onNextTrack)];
    }
}

#pragma mark - Remote command event handlers.

-(MPRemoteCommandHandlerStatus)onPlayTrack{
    if(self.remoteCommandsDelegate)
        return [self.remoteCommandsDelegate onPlayCommand];
    return MPRemoteCommandHandlerStatusNoActionableNowPlayingItem;
}

-(MPRemoteCommandHandlerStatus)onPauseTrack{
    if(self.remoteCommandsDelegate)
        return [self.remoteCommandsDelegate onPauseCommand];
    return MPRemoteCommandHandlerStatusNoActionableNowPlayingItem;
}

-(MPRemoteCommandHandlerStatus)onTogglePlayPauseTrack{
    if(self.remoteCommandsDelegate)
        return [self.remoteCommandsDelegate onTogglePlayPauseCommand];
    return MPRemoteCommandHandlerStatusNoActionableNowPlayingItem;
}

-(MPRemoteCommandHandlerStatus)onNextTrack{
    if(self.remoteCommandsDelegate)
        return [self.remoteCommandsDelegate onNextTrackCommand];
    return MPRemoteCommandHandlerStatusNoActionableNowPlayingItem;
}

-(MPRemoteCommandHandlerStatus)onPrevTrack{
    if(self.remoteCommandsDelegate)
        return [self.remoteCommandsDelegate onPrevTrackCommand];
    return MPRemoteCommandHandlerStatusNoActionableNowPlayingItem;
}

#pragma mark - Remote command center methods.

-(void)enablePlayCommand:(BOOL)enable{
    if(self.isActiveRemoteCommands){
        [MPRemoteCommandCenter sharedCommandCenter].playCommand.enabled = enable;
    }
}

-(void)enableNextTrackCommand:(BOOL)enable{
    if(self.isActiveRemoteCommands){
        [MPRemoteCommandCenter sharedCommandCenter].nextTrackCommand.enabled = enable;
    }
}

-(void)enablePrevTrackCommand:(BOOL)enable{
    if(self.isActiveRemoteCommands){
        [MPRemoteCommandCenter sharedCommandCenter].previousTrackCommand.enabled = enable;
    }
}

#pragma mark - Mediaplayer now playing info update methods.
-(void)setNowPlayingState:(BOOL)playing {
    if(self.isActiveMPNowPlayingInfo){
        if(playing){
            [self setNowPlayingPlaybackProgress:1.0];
            [self updateNowPlayingInfo];
        }
        else{
            [self setNowPlayingPlaybackProgress:0.0];
            [self updateNowPlayingInfo];
        }
    }
}

-(void)setNowPlayingArtistName:(NSString*)artistName{
    [self.nowPlayingInfo setValue:artistName forKey:MPMediaItemPropertyArtist];
}

-(void)setNowPlayingAlbumName:(NSString*)albumName{
    [self.nowPlayingInfo setValue:albumName forKey:MPMediaItemPropertyAlbumTitle];
}

-(void)setNowPlayingTitle:(NSString*)title{
    [self.nowPlayingInfo setValue:title forKey:MPMediaItemPropertyTitle];
}

-(void)setNowPlayingArtwork:(UIImage*)artworkImage{
    //[self.nowPlayingInfo setValue:nil forKey:MPMediaItemPropertyArtwork];
}

-(void)setNowPlayingDuration:(double)durationInSec{
    [self.nowPlayingInfo setValue:[NSNumber numberWithDouble:durationInSec] forKey:MPMediaItemPropertyPlaybackDuration];
}

-(void)setNowPlayingPlaybackProgress:(double)playbackProgressInSec{
    [self.nowPlayingInfo setValue:[NSNumber numberWithDouble:playbackProgressInSec] forKey:MPNowPlayingInfoPropertyPlaybackProgress];
}

-(void)setNowPlayingElapsedPlaybackTime:(double)elapsedPlaybackTimeInSec{
    [self.nowPlayingInfo setValue:[NSNumber numberWithDouble:elapsedPlaybackTimeInSec] forKey:MPNowPlayingInfoPropertyElapsedPlaybackTime];
}

-(void)setNowPlayingPlaybackRate:(double)playbackRate{
    [self.nowPlayingInfo setValue:[NSNumber numberWithFloat:(float)playbackRate] forKey:MPNowPlayingInfoPropertyPlaybackRate];
}

-(void)updateNowPlayingInfo{
    if(!self.isActiveMPNowPlayingInfo)
        return;
    // Update mediaplayer now playing info.
    [MPNowPlayingInfoCenter defaultCenter].nowPlayingInfo = self.nowPlayingInfo;
}
@end
