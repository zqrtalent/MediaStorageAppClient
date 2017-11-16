//
//  AudioMetadataInfo+DataContracts.h
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 10/21/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "../Models/AudioMetadataInfo.h"

class MLSong;
class MLAlbum;
class MLArtist;

@interface AudioMetadataInfo(DataContracts)

+(AudioMetadataInfo*)metadataInfoFromDataContracts:(MLSong*)pSong Artist:(MLArtist*)pArtist Album:(MLAlbum*)pAlbum;

@end
