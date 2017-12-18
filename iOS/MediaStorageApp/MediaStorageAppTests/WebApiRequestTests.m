//
//  WebApiRequestTests.m
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 12/15/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import <XCTest/XCTest.h>


#import "../MediaStorageApp/Streaming/StreamingSessionSettings.h"
#import "../MediaStorageApp/Extensions/StreamingSession+ApiRequests.h"

#import "../MediaStorageApp/MediaStorageWebApi/AuthRequest.h"
#import "../MediaStorageApp/MediaStorageWebApi/LibraryInfoRequest.h"
#import "../MediaStorageApp/MediaStorageWebApi/ImageResourceRequest.h"
#import "../MediaStorageApp/MediaStorageWebApi/AudioPacketsByOffsetRequest.h"

@interface WebApiRequestTests : XCTestCase
{
    NSString* _sessionId;
    NSString* _mediaId;
}

@end

@implementation WebApiRequestTests

- (void)setUp {
    [super setUp];
    
    _sessionId = @"4fa7efdb-3d12-4419-ba5f-c62a9f68785e";
    _mediaId = @"62c7b3b7-b895-48be-be71-03cc57a546ce";
    
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

-(MediaPackets*)requestPacketsByOffset:(AudioPacketsByOffsetRequest*)req CompletedEvent:(dispatch_semaphore_t)completed
{
    MediaPackets* __block packets = nullptr;
    [req makeRequest:^(MediaPackets* respPackets)
    {
        packets = respPackets;
        // Signal download sync.
        dispatch_semaphore_signal(completed);
    }];
    
    dispatch_semaphore_wait(completed, DISPATCH_TIME_FOREVER);
    return packets;
}

- (void)testAudioPacketsByOffset
{
    NSRange range;
    range.location = 0;
    range.length = 50;
    
    AudioPacketsByOffsetRequest* req = [[AudioPacketsByOffsetRequest alloc] init:_sessionId SongId:_mediaId Range:range];
    req.host = [StreamingSessionSettings sharedSettings].webApiHost;
    
    MediaPackets* packets = nullptr;
    dispatch_semaphore_t __block completed = dispatch_semaphore_create(0);
    
    for(int i=0; i<5; i++)
    {
        packets = [self requestPacketsByOffset:req CompletedEvent:completed];
        
        XCTAssert(packets != nil);
        XCTAssert(packets->_offset == range.location + range.length);
        XCTAssert(packets->_packets.GetCount() == range.length);
        
        delete packets;
        range.location += range.length;
        [req setQueryParams:_sessionId SongId:_mediaId Range:range];
    }
}

//-(void)testPerformanceExample {
//    // This is an example of a performance test case.
//    [self measureBlock:^{
//        // Put the code you want to measure the time of here.
//    }];
//}

@end
