//
//  MediaPlayerControlCenter.h
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 6/25/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "RemoteCommandsProtocol.h"
#import <UIKit/UIImage.h>

@interface MediaPlayerControlCenter : NSObject

@property (nonatomic, strong) id<RemoteCommandsProtocol> remoteCommandsDelegate;

-(instancetype)init;
-(void)cleanUp;
-(void)setActiveNowPlayingInfo:(BOOL)active;
-(void)setActiveRemoteCommands:(BOOL)active;

// Remote command center.
-(void)enablePlayCommand:(BOOL)enable;
-(void)enableNextTrackCommand:(BOOL)enable;
-(void)enablePrevTrackCommand:(BOOL)enable;

// MPNowPlayingInfo attributes.
-(void)setNowPlayingState:(BOOL)playing;
-(void)setNowPlayingArtistName:(NSString*)artistName;
-(void)setNowPlayingAlbumName:(NSString*)albumName;
-(void)setNowPlayingTitle:(NSString*)title;
-(void)setNowPlayingArtwork:(UIImage*)artworkImage;
-(void)setNowPlayingDuration:(double)durationInSec;
-(void)setNowPlayingPlaybackProgress:(double)playbackProgressInSec;
-(void)setNowPlayingElapsedPlaybackTime:(double)elapsedPlaybackTimeInSec;
-(void)setNowPlayingPlaybackRate:(double)playbackRate;
-(void)updateNowPlayingInfo;

@end
