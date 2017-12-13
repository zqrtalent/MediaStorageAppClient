//
//  AudioFileSourceTests.m
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 12/9/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import <XCTest/XCTest.h>
#import "../MediaStorageApp/Streaming/AudioFileSource.h"

@interface AudioFileSourceTests : XCTestCase

@end

@implementation AudioFileSourceTests

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
    // This is an example of a functional test case.
    // Use XCTAssert and related functions to verify your tests produce the correct results.
    
    AudioFileSource* audioFile = [[AudioFileSource alloc] init:
                                  [NSURL URLWithString:@"/Users/ZqrTalent/Desktop/01_blink_182_dumpweed_myzuka.me.mp3"]
                                                      FileType:kAudioFileMP3Type];
    
    long offset = [audioFile getByteByPacket:1];
    XCTAssert(offset > 0);
    XCTAssert([audioFile getPacketSizeInBytes] > 0);
    XCTAssert([audioFile getNumberOfPackets] > 0);
}

//- (void)testPerformanceExample {
//    // This is an example of a performance test case.
//    [self measureBlock:^{
//        // Put the code you want to measure the time of here.
//    }];
//}

@end
