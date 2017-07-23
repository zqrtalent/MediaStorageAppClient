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
#import "MediaStreamingService.h"
#import "PlayerDelegate.h"

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

@property (nonatomic, strong) id<PlayerDelegate> delegate;
@property (nonatomic, getter=getDurationInSec, readonly) float DurationInSec;
@property (nonatomic, getter=getCurrentTimeInSec, readonly) float CurrentTimeInSec;
@property (nonatomic, getter=getCurrentTimeInMSec, readonly) float CurrentTimeInMSec;

-(Player*)init:(NSString*)sessionKey;
-(void)play:(NSString*)mediaId At:(int)msecStartAt;
-(void)stop:(PlayerEventCompletionHandler __nullable)completionHandler;
-(void)pause:(PlayerEventCompletionHandler __nullable)completionHandler;
-(void)resume;
-(void)seekAtTime:(float)seconds;
-(AudioPlayerState)getPlayerState;
-(float)getDurationInSec;
-(float)getCurrentTimeInSec;
-(float)getCurrentTimeInMSec;
-(void)setVolume:(float)volume;

-(NSString*)getMediaId;
-(NSString*)getSongName;
-(NSString*)getArtistName;
-(NSString*)getAlbumName;

@end
