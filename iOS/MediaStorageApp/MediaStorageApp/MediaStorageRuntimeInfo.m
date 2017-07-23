//
//  NowPlayInfo.m
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 5/7/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "MediaStorageRuntimeInfo.h"
#import "PlayerQueueManager.h"

@interface MediaStorageRuntimeInfo()
@end

@implementation MediaStorageRuntimeInfo

+(instancetype)sharedInstance{
    static dispatch_once_t onceToken;
    static MediaStorageRuntimeInfo* sharedInstance;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[MediaStorageRuntimeInfo alloc] init];
    });
    return sharedInstance;
}

-(instancetype)init{
    self.playlistQueue = [[PlayerQueueManager alloc] init];
    return [super init];
}

-(void)updateMediaLibraryInfo:(MediaLibraryInfo*)mlInfo{
    self->_mlInfo = mlInfo;
}

-(BOOL)scheduleMedia:(NSString*)mediaId FromArtist:(NSString*)artistId AndFromAlbum:(NSString*)albumId PlayInstantly:(BOOL)playInstantly{
    if(!self->_mlInfo)
        return NO;
    
    _string strMediaId([mediaId UTF8String]);
    _string strArtistId([artistId UTF8String]);
    _string strAlbumId([albumId UTF8String]);
    MLArtist* pArtist = _mlInfo->GetArtistById(&strArtistId);
    if(!pArtist)
        return NO;
    
    MLAlbum* pAlbum = _mlInfo->GetAlbumById(pArtist, &strAlbumId);
    if(!pAlbum)
        return NO;
    
    // Clear current playlist;
    if(playInstantly){
        [self.playlistQueue clear];
    }
    
    // Retrive play media index by media id.
    int playMediaIndex = -1;
    for(int i=0; i<pAlbum->_songs.GetCount(); i++){
        if(!pAlbum->_songs[i])
            continue;
        
        MediaInfo *media = [[MediaInfo alloc] init:pAlbum->_songs[i] ByAlbumName:&pAlbum->_name ByArtistName:&pArtist->_name ByArtworkImageId:&pAlbum->_artworkImageId];
        [self.playlistQueue add:media];
        
        if(playMediaIndex == -1 && !strMediaId.compare(pAlbum->_songs[i]->_id)){
            playMediaIndex = i;
        }
    }
    
    if(playInstantly){
        MediaInfo* playMedia = [self.playlistQueue mediaByIndex:playMediaIndex UseAsCurrent:YES];
        self.NowPlaying = playMedia;
        [self.player play:playMedia.mediaId At:0];
        
        // Play song.
        //    if(info.NowPlaying == nil || [info.NowPlaying.mediaId compare:self.media.mediaId options:NSCaseInsensitiveSearch] != NSOrderedSame ){
        //        /*Stops if already playing*/
        //        [self.player play:self.media.mediaId At:0/*Play at second*/];
        //    }
    }
    
    return YES;
}

-(BOOL)playNext{
    if(![self isNextAvailable])
        return NO;
    
    MediaInfo* playMedia = [self.playlistQueue nextMedia:YES];
    self.NowPlaying = playMedia;
    [self.player play:playMedia.mediaId At:0];
    return YES;
}

-(BOOL)isNextAvailable{
    MediaInfo* nextMedia = [self.playlistQueue nextMedia:NO];
    return nextMedia;
}

-(BOOL)playPrev{
    if(![self isPrevAvailable])
        return NO;
    
    MediaInfo* playMedia = [self.playlistQueue prevMedia:YES];
    self.NowPlaying = playMedia;
    [self.player play:playMedia.mediaId At:0];
    return YES;
}

-(BOOL)isPrevAvailable{
    MediaInfo* prevMedia = [self.playlistQueue prevMedia:NO];
    return prevMedia;
}

-(void)cleanUp{
    self->_mlInfo = nullptr;
    self.playlistQueue = nil;
}

@end
