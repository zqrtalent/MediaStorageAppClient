//
//  ModiaStorageAppModelTests.m
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 10/21/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import <XCTest/XCTest.h>

#include "../MediaStorageApp/MediaStorageWebApi/DataContracts/MLArtist.h"
#include "../MediaStorageApp/MediaStorageWebApi/DataContracts/MLArtist.h"
#include "../MediaStorageApp/MediaStorageWebApi/DataContracts/MLSong.h"
#include "../MediaStorageApp/Models/AudioMetadataInfo.h"
#include "../MediaStorageApp/Extensions/AudioMetadataInfo+DataContracts.h"
#include "../MediaStorageApp/Extensions/NSString+MercuryString.h"

@interface ModiaStorageAppModelTests : XCTestCase

@end

@implementation ModiaStorageAppModelTests

//- (void)setUp {
//    [super setUp];
//    // Put setup code here. This method is called before the invocation of each test method in the class.
//}
//
//- (void)tearDown {
//    // Put teardown code here. This method is called after the invocation of each test method in the class.
//    [super tearDown];
//}

- (void)testExample {
    MLSong song;
    song._id = _T("song_id");
    song._name = _T("song_id");
    song._durationSec = 120;
    song._track = 12;
    
    MLArtist artist;
    artist._id = _T("artist_id");
    artist._name = _T("artist");
    
    MLAlbum album;
    album._id = _T("album_id");
    album._name = _T("album");
    album._year = 1998;
    
    AudioMetadataInfo* metadataInfo = [AudioMetadataInfo metadataInfoFromDataContracts:&song Artist:&artist Album:&album];
    XCTAssert(metadataInfo != nil);
    XCTAssert([metadataInfo.mediaId compare:[NSString stringFromMercuryCString:&song._id]] == NSOrderedSame);
    XCTAssert([metadataInfo.songName compare:[NSString stringFromMercuryCString:&song._name]] == NSOrderedSame);
    XCTAssert(metadataInfo.durationSec == song._durationSec);
    XCTAssert(metadataInfo.trackNumber == song._track);
    
    XCTAssert([metadataInfo.artistId compare:[NSString stringFromMercuryCString:&artist._id]] == NSOrderedSame);
    XCTAssert([metadataInfo.artistName compare:[NSString stringFromMercuryCString:&artist._name]] == NSOrderedSame);
    
    XCTAssert([metadataInfo.albumId compare:[NSString stringFromMercuryCString:&album._id]] == NSOrderedSame);
    XCTAssert([metadataInfo.albumName compare:[NSString stringFromMercuryCString:&album._name]] == NSOrderedSame);
    XCTAssert(metadataInfo.albumYear == album._year);
}

//
//- (void)testPerformanceExample {
//    // This is an example of a performance test case.
//    [self measureBlock:^{
//        // Put the code you want to measure the time of here.
//    }];
//}

@end
