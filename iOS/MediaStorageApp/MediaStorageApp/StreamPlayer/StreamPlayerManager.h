//
//  MediaStorageRuntimeInfo.h
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 5/7/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Player.h"
#import "StreamPlayerPlaylist.h"

@class StreamingSession;
class MediaLibraryInfo;

@interface StreamPlayerManager : NSObject
{
@protected
    // Media library info object.
    MediaLibraryInfo* _mlInfo;
}

@property (nonatomic, strong) StreamPlayerPlaylist* playlist;
@property (nonatomic, strong) Player* player;
@property (nonatomic, readonly, getter=retrievePlayerState) AudioPlayerState playerState;
@property (nonatomic, readonly, getter=retrieveElapsedPlaybackTimeInSec) double elapsedPlaybackTimeSec;
@property (nonatomic, strong) AudioMetadataInfo* nowPlaying;

+(instancetype)sharedInstance;

-(BOOL)scheduleMedia:(NSString*)mediaId FromArtist:(NSString*)artistId AndFromAlbum:(NSString*)albumId ClearPlaylist:(bool)clear;

-(void)play:(int)index;

-(BOOL)playPauseToggle;

-(BOOL)play;

-(BOOL)pause;

-(BOOL)replay;

-(BOOL)playNext;

-(BOOL)isNextAvailable;

-(BOOL)playPrev;

-(BOOL)isPrevAvailable;

-(void)cleanUp;

-(void)updateMediaLibraryInfo:(MediaLibraryInfo*)mlInfo;

-(void)updateStreamingSession:(StreamingSession*)session;

@end
