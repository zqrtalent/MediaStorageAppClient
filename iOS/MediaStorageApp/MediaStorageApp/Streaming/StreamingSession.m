//
//  StreamingSession.m
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 10/4/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "StreamingSession.h"
#import "WebApiAudioStreamSource.h"

#import "../Extensions/NSString+MercuryString.h"
#import "../Extensions/StreamingSession+ApiRequests.h"
#import "../MediaStorageWebApi/AuthRequest.h"
#import "../MediaStorageWebApi/LibraryInfoRequest.h"
#import "../MediaStorageWebApi/ImageResourceRequest.h"
#import "../MediaStorageWebApi/AudioPacketsByOffsetRequest.h"

@interface StreamingSession()
{
}

@property (nonatomic, strong) StreamingSessionSettings* settings;
@property (nonatomic, strong) NSString* sessionId;

-(StreamingSessionSettings*)getSettings;

@end

@implementation StreamingSession

-(StreamingSessionSettings*)getSettings
{
    return self.settings;
}

-(instancetype)init:(StreamingSessionSettings*)settings
{
    self.settings = settings;
    return self;
}

-(void)authenticate:(NSString*)userName Password:(NSString*)password
{
    __typeof__(self) __weak weakSelf = self;
    AuthRequest* __block req = [self authRequest:userName Pass:password Hash:@"temphash"];
    [req makeRequest:^(SessionInfo* pSessInfo)
    {
        weakSelf.sessionId = [NSString stringFromMercuryCString:&pSessInfo->_sessionKey];
        if(weakSelf.delegate)
        {
            [weakSelf.delegate streamingSession:weakSelf Authenticated:YES];
        }
        [req cancelTasksAndInvalidate];
        req = nil;
    }];
}

-(void)getAllMediaLibraryMetadata
{
    // Request library info.
    __typeof__(self) __weak weakSelf = self;
    LibraryInfoRequest* __block req = [self libraryInfoRequest];
    [req makeRequest:^(MediaLibraryInfo* pInfo)
    {
        if(weakSelf.delegate)
        {
            [weakSelf.delegate streamingSession:weakSelf AllMediaLibraryMetadata:pInfo];
        }
        [req cancelTasksAndInvalidate];
        req = nil;
    }];
}

-(void)getMediaArtworkImage:(NSString*)mediaId
{
    // Request for image resource.
    __typeof__(self) __weak weakSelf = self;
    ImageResourceRequest* __block req = [self imageResourceRequest:mediaId SizeType:@"medium"];
    [req makeRequest:^(NSData* imageData)
    {
        if(weakSelf.delegate)
        {
            [weakSelf.delegate streamingSession:weakSelf MediaArtworkImage:[UIImage imageWithData:imageData]];
        }
        [req cancelTasksAndInvalidate];
        req = nil;
    }];
}

-(id<MediaStreamSourceProtocol>)getMediaStream:(NSString*)mediaId
{
    // Stream basic description data should be initialized based on data received from streaming api and not like this, only supporting
    // mp3 formats with fixed parameters.
    AudioStreamBasicDescription streamDesc = {};
    streamDesc.mSampleRate = 44100;
    streamDesc.mFormatID = kAudioFormatMPEGLayer3;//778924083
    streamDesc.mChannelsPerFrame = 2;
    streamDesc.mBitsPerChannel = 0;
    streamDesc.mBytesPerPacket = 0;
    //streamDesc.mFramesPerPacket = 1152;
    //streamDesc.mFormatFlags = kLinearPCMFormatFlagIsPacked | kLinearPCMFormatFlagIsSignedInteger; // little-endian
    //streamDesc.mFormatFlags = kLinearPCMFormatFlagIsFloat; // little-endian
    streamDesc.mReserved = 0;
    
    return [[WebApiAudioStreamSource alloc] init:self MediaId:mediaId StreamDescription:&streamDesc WithSettings:[WebApiAudioStreamReaderSettings defaultSettings]];
}

@end
