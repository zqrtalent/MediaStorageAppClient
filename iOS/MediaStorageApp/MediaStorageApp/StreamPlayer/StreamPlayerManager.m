//
//  NowPlayInfo.m
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 5/7/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "StreamPlayerManager.h"
#import "../Streaming/StreamingSession.h"
#include "../MediaStorageWebApi/DataContracts/MediaLibraryInfo.h"
#import "../Extensions/AudioMetadataInfo+DataContracts.h"

@interface StreamPlayerManager()

@property (nonatomic, strong) StreamingSession* session;

-(AudioPlayerState)retrievePlayerState;

@end

@implementation StreamPlayerManager

+(instancetype)sharedInstance
{
    static dispatch_once_t onceToken;
    static StreamPlayerManager* sharedInstance;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[StreamPlayerManager alloc] init];
    });
    return sharedInstance;
}

-(instancetype)init
{
    self.playlist = [[StreamPlayerPlaylist alloc] init];
    self.player = [[Player alloc] init];
    return [super init];
}

-(void)updateMediaLibraryInfo:(MediaLibraryInfo*)mlInfo
{
    self->_mlInfo = mlInfo;
}

-(void)updateStreamingSession:(StreamingSession*)session
{
    self.session = session;
}

-(BOOL)scheduleMedia:(NSString*)mediaId FromArtist:(NSString*)artistId AndFromAlbum:(NSString*)albumId ClearPlaylist:(bool)clear;
{
    if(!self->_mlInfo)
        return NO;
    
    // Clear playlist.
    if(clear)
       [self.playlist clear];
    
    _string strMediaId([mediaId UTF8String]);
    _string strArtistId([artistId UTF8String]);
    _string strAlbumId([albumId UTF8String]);
    
    MLArtist* pArtist = _mlInfo->GetArtistById(&strArtistId);
    if(!pArtist)
        return NO;
    
    MLAlbum* pAlbum = _mlInfo->GetAlbumById(pArtist, &strAlbumId);
    if(!pAlbum)
        return NO;
    
    // Retrive play media index by media id.
    int playMediaIndex = -1;
    for(int i=0; i<pAlbum->_songs.GetCount(); i++)
    {
        if(!pAlbum->_songs[i])
            continue;
        
        [self.playlist add:[AudioMetadataInfo metadataInfoFromDataContracts:pAlbum->_songs[i] Artist:pArtist Album:pAlbum]];
        if(playMediaIndex == -1 && !strMediaId.compare(pAlbum->_songs[i]->_id))
        {
            playMediaIndex = i;
        }
    }
    
    return YES;
}

-(BOOL)playByMetadataInfo:(AudioMetadataInfo*)metadata
{
    id<MediaStreamSourceProtocol> stream = [self.session getMediaStream:metadata.mediaId];
    if(stream)
    {
        self.NowPlaying = metadata;
        [self.player play:stream AudioMetadata:metadata At:0];
        return YES;
    }
    return NO;
}

-(void)play:(int)index
{
    assert(self.session);
    [self playByMetadataInfo: [self.playlist mediaByIndex:index UseAsCurrent:YES]];
}

-(BOOL)playPauseToggle
{
    assert(self.session);
    
    if(self.player == nil || self.player.audioStream == nil)
        return NO;
    
    // Start play/resume
    if([self.player getPlayerState] == AudioPlayer_Stopped || [_player getPlayerState] == AudioPlayer_Paused)
        [self.player resume];
    else
        [self.player pause: nil];
    return YES;
}

-(BOOL)playNext
{
    assert(self.session);
    if(![self isNextAvailable])
        return NO;
    return [self playByMetadataInfo: [self.playlist nextMedia:YES]];
}

-(BOOL)isNextAvailable
{
    AudioMetadataInfo* nextMedia = [self.playlist nextMedia:NO];
    return nextMedia != nil;
}

-(BOOL)playPrev
{
    assert(self.session);
    if(![self isPrevAvailable])
        return NO;
    return [self playByMetadataInfo: [self.playlist prevMedia:YES]];
}

-(BOOL)isPrevAvailable
{
    AudioMetadataInfo* prevMedia = [self.playlist prevMedia:NO];
    return prevMedia != nil;
}

-(void)cleanUp
{
    self->_mlInfo = nullptr;
    self.playlist = nil;
    self.session = nil;
}

-(AudioPlayerState)retrievePlayerState
{
    return [self.player getPlayerState];
}

@end
