//
//  AudioMetadata.m
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 10/21/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "AudioMetadataInfo.h"

@interface AudioMetadataInfo()
{
    NSDictionary* _dicMetadata;
}

-(NSString*)retrieveMediaId;
-(NSString*)retrieveArtistId;
-(NSString*)retrieveAlbumId;
-(NSString*)retrieveArtistName;
-(NSString*)retrieveSongName;
-(NSString*)retrieveAlbumName;
-(NSInteger)retrieveAlbumYear;
-(NSInteger)retrieveDurationInSec;
-(NSString*)retrieveArtworkId;

-(id)retrieveByMetadataType:(NSInteger)metadataType;

@end

@implementation AudioMetadataInfo

-(instancetype)init:(NSDictionary*)dicMetadataInfo
{
    _dicMetadata = dicMetadataInfo;
    return [super init];
}

-(id)retrieveByMetadataType:(NSInteger)metadataType
{
    if(_dicMetadata == nil)
        return nil;
    return [_dicMetadata objectForKey:[NSString stringWithFormat:@"%ld", (long)metadataType]];
}

-(NSString*)retrieveMediaId
{
    return [self retrieveByMetadataType:Meta_MediaId];
}

-(NSString*)retrieveArtistId
{
    return [self retrieveByMetadataType:Meta_ArtistId];
}

-(NSString*)retrieveAlbumId
{
    return [self retrieveByMetadataType:Meta_AlbumId];
}

-(NSString*)retrieveArtistName
{
    return [self retrieveByMetadataType:Meta_ArtistName];
}

-(NSString*)retrieveSongName
{
    return [self retrieveByMetadataType:Meta_SongName];
}

-(NSString*)retrieveAlbumName
{
    return [self retrieveByMetadataType:Meta_AlbumName];
}

-(NSInteger)retrieveAlbumYear
{
    NSNumber* num = [self retrieveByMetadataType:Meta_AlbumYear];
    if(num == nil)
        return 0;
    return [num integerValue];
}

-(NSInteger)retrieveDurationInSec
{
    NSNumber* num = [self retrieveByMetadataType:Meta_DurationSec];
    if(num == nil)
        return 0;
    return [num integerValue];

}

-(NSInteger)retrieveTrackNumber
{
    NSNumber* num = [self retrieveByMetadataType:Meta_TrackNumber];
    if(num == nil)
        return 0;
    return [num integerValue];
}

-(NSString*)retrieveArtworkId
{
    return [self retrieveByMetadataType:Meta_ArtworkId];
}

@end
