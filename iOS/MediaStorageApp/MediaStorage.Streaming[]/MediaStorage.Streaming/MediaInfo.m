//
//  MediaInfo.m
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 6/21/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "MediaInfo.h"

@implementation MediaInfo

-(instancetype)init:(MLSong*)song ByAlbumName:(_string*)pAlbumName ByArtistName:(_string*)pArtistName ByArtworkImageId:(_string*)pArtworkImageId
{
    self.mediaId = [NSString stringWithUTF8String:song->_id.c_str()];
    self.songName = [NSString stringWithUTF8String:song->_name.c_str()];
    self.artist = [NSString stringWithUTF8String:pArtistName->c_str()];
    self.album = [NSString stringWithUTF8String:pAlbumName->c_str()];
    self.artworkImageId = [NSString stringWithUTF8String:pArtworkImageId->c_str()];
    
    self->_durationInSec = (float)song->_durationSec;
    self.CurrentPositionInSec = 0.0f;
    
    return [self init];
}

-(float)getDurationInSec{
    return self->_durationInSec;
}

-(float)getCurrentPositionInSec{
    return self.CurrentPositionInSec;
}

@end
