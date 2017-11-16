//
//  MediaInfo.h
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 6/21/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "../MediaStorageWebApi/DataContracts/MLSong.h"

@interface MediaInfo : NSObject
{
@private
    float _durationInSec;           // Duration of media.
}

@property (nonatomic, strong) NSString* mediaId;
@property (nonatomic, strong) NSString* songName;
@property (nonatomic, strong) NSString* artist;
@property (nonatomic, strong) NSString* album;
@property (nonatomic, strong) NSString* artworkImageId;
@property (nonatomic, getter=getDurationInSec, readonly) float DurationInSec;
@property (nonatomic, assign) float CurrentPositionInSec;

-(instancetype)init:(MLSong*)song ByAlbumName:(_string*)pAlbumName ByArtistName:(_string*)pArtistName ByArtworkImageId:(_string*)pArtworkImageId;
-(float)getDurationInSec;

@end
