//
//  AudioMetadata.h
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 10/21/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef NS_ENUM(NSInteger, MediaMetadataType)
{
    Meta_ArtistName,
    Meta_AlbumName,
    Meta_SongName,
    Meta_DurationSec,
    Meta_AlbumYear,
    Meta_ArtistId,
    Meta_AlbumId,
    Meta_MediaId,
    Meta_TrackNumber,
    Meta_ArtworkId
};

@interface AudioMetadataInfo : NSObject

-(instancetype __nonnull)init:(NSDictionary* __nonnull)dicMetadataInfo;

@property(readonly, nonatomic, strong, getter=retrieveMediaId) NSString* __nullable mediaId;
@property(readonly, nonatomic, strong, getter=retrieveArtistId) NSString* __nullable artistId;
@property(readonly, nonatomic, strong, getter=retrieveAlbumId) NSString* __nullable albumId;
@property(readonly, nonatomic, strong, getter=retrieveArtistName) NSString* __nullable artistName;
@property(readonly, nonatomic, strong, getter=retrieveSongName) NSString* __nullable songName;
@property(readonly, nonatomic, strong, getter=retrieveAlbumName) NSString* __nullable albumName;
@property(readonly, nonatomic, getter=retrieveAlbumYear) NSInteger albumYear;
@property(readonly, nonatomic, getter=retrieveDurationInSec) NSInteger durationSec;
@property(readonly, nonatomic, getter=retrieveTrackNumber) NSInteger trackNumber;
@property(readonly, nonatomic, strong, getter=retrieveArtworkId) NSString* __nullable artworkId;

@end
