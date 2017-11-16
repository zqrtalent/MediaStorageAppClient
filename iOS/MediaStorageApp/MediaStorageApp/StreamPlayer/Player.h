//
//  Player.h
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 2/11/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <AudioToolbox/AudioFormat.h>
#import "PlayerDelegate.h"

@class AudioMetadataInfo;
@protocol MediaStreamSourceProtocol;

typedef NS_ENUM(NSInteger, AudioPlayerState)
{
    AudioPlayer_Stopped,
    AudioPlayer_Playing,
    AudioPlayer_Paused,
    AudioPlayer_Buffering
};

typedef void (^PlayerEventCompletionHandler)();

@interface Player : NSObject
{
}

@property (nonatomic, strong) id<PlayerDelegate> __nullable delegate;
@property (nonatomic, strong) id<MediaStreamSourceProtocol> __nullable audioStream;
@property (nonatomic, strong) AudioMetadataInfo* __nullable audioMetadata;
@property (nonatomic, getter=getDurationInSec, readonly) float DurationInSec;
@property (nonatomic, getter=getCurrentTimeInSec, readonly) float CurrentTimeInSec;
@property (nonatomic, getter=getCurrentTimeInMSec, readonly) float CurrentTimeInMSec;

-(instancetype __nonnull)init;

-(void)play:(id<MediaStreamSourceProtocol> __nonnull)audioStream AudioMetadata:(AudioMetadataInfo* __nullable)metadata At:(int)msecStartAt;
-(void)setVolume:(float)volume;
-(void)stop:(PlayerEventCompletionHandler __nullable)completionHandler;
-(void)pause:(PlayerEventCompletionHandler __nullable)completionHandler;
-(void)resume;
-(void)seekAtTime:(float)seconds;

-(AudioPlayerState)getPlayerState;

-(float)getDurationInSec;
-(float)getCurrentTimeInSec;
-(float)getCurrentTimeInMSec;

@end
