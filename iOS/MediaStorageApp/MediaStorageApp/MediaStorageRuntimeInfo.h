//
//  MediaStorageRuntimeInfo.h
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 5/7/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Player/Player.h"
#import "Player/PlayerDelegate.h"
#import "MediaStream/MediaInfo.h"
#import "PlayerQueueManager.h"
#import "MediaStreamingSessionInfo.h"

#include "MediaStorageWebApi/DataContracts/MediaLibraryInfo.h"

@interface MediaStorageRuntimeInfo : NSObject
{
@public
    // Media library info object.
    MediaLibraryInfo* _mlInfo;
}

@property (nonatomic, strong) MediaStreamingSessionInfo* sessionInfo;   // Current session info.
@property (nonatomic, strong) PlayerQueueManager* playlistQueue;         // Players queue manager.
@property (nonatomic, strong) Player* player;                           // Player instance.
@property (nonatomic, strong) MediaInfo* NowPlaying;                    // Now playing media info.

+(instancetype)sharedInstance;

-(void)updateMediaLibraryInfo:(MediaLibraryInfo*)mlInfo;

-(BOOL)scheduleMedia:(NSString*)mediaId FromArtist:(NSString*)artistId AndFromAlbum:(NSString*)albumId PlayInstantly:(BOOL)playInstantly;

-(BOOL)playNext;

-(BOOL)isNextAvailable;

-(BOOL)playPrev;

-(BOOL)isPrevAvailable;

-(void)cleanUp;

@end
