//
//  AudioMetadataInfo+DataContracts.m
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 10/21/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "AudioMetadataInfo+DataContracts.h"
#import "NSString+MercuryString.h"
#include "../MediaStorageWebApi/DataContracts/MLArtist.h"
#include "../MediaStorageWebApi/DataContracts/MLAlbum.h"

@implementation AudioMetadataInfo(DataContracts)

+(AudioMetadataInfo*)metadataInfoFromDataContracts:(MLSong*)pSong Artist:(MLArtist*)pArtist Album:(MLAlbum*)pAlbum
{
    NSMutableArray* arrKeys = [[NSMutableArray alloc] init];
    NSMutableArray* arrObjects = [[NSMutableArray alloc] init];
    
    if(pSong)
    {
        // MediaId
        [arrKeys addObject:[NSString stringWithFormat:@"%ld", Meta_MediaId]];
        [arrObjects addObject:[NSString stringFromMercuryCString:&pSong->_id]];
        
        // Song name
        [arrKeys addObject:[NSString stringWithFormat:@"%ld", Meta_SongName]];
        [arrObjects addObject:[NSString stringFromMercuryCString:&pSong->_name]];
        
        // Duration in sec
        if(pSong->_durationSec > 0)
        {
            [arrKeys addObject:[NSString stringWithFormat:@"%ld", Meta_DurationSec]];
            [arrObjects addObject:[NSNumber numberWithInt:pSong->_durationSec]];
        }
        
        // Track number
        if(pSong->_track > 0)
        {
            [arrKeys addObject:[NSString stringWithFormat:@"%ld", Meta_TrackNumber]];
            [arrObjects addObject:[NSNumber numberWithInt:pSong->_track]];
        }
    }
    
    if(pAlbum)
    {
        // Album id
        [arrKeys addObject:[NSString stringWithFormat:@"%ld", Meta_AlbumId]];
        [arrObjects addObject:[NSString stringFromMercuryCString:&pAlbum->_id]];
        
        // Album name
        [arrKeys addObject:[NSString stringWithFormat:@"%ld", Meta_AlbumName]];
        [arrObjects addObject:[NSString stringFromMercuryCString:&pAlbum->_name]];
        
        // Album year.
        if(pAlbum->_year > 1000)
        {
            [arrKeys addObject:[NSString stringWithFormat:@"%ld", Meta_AlbumYear]];
            [arrObjects addObject:[NSNumber numberWithInt:pAlbum->_year]];
        }
    }
    
    if(pArtist)
    {
        // Artist id
        [arrKeys addObject:[NSString stringWithFormat:@"%ld", Meta_ArtistId]];
        [arrObjects addObject:[NSString stringFromMercuryCString:&pArtist->_id]];
        
        // Artist name
        [arrKeys addObject:[NSString stringWithFormat:@"%ld", Meta_ArtistName]];
        [arrObjects addObject:[NSString stringFromMercuryCString:&pArtist->_name]];
    }
    
    return [[AudioMetadataInfo alloc] init:[NSDictionary dictionaryWithObjects:arrObjects forKeys:arrKeys]];
}

@end
