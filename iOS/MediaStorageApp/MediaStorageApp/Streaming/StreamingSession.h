//
//  StreamingSession.h
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 10/4/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "StreamingSessionSettings.h"
#import "MediaStreamSource.h"

class MediaLibraryInfo;
@protocol StreamingSessionProtocol;

@interface StreamingSession : NSObject

-(instancetype)init:(StreamingSessionSettings*)settings;

-(void)authenticate:(NSString*)userName Password:(NSString*)password;
-(void)getAllMediaLibraryMetadata;
-(void)getMediaArtworkImage:(NSString*)mediaId;

-(id<MediaStreamSourceProtocol>)getMediaStream:(NSString*)mediaId;

@property (nonatomic, weak) id<StreamingSessionProtocol> delegate;
@property (nonatomic, strong, readonly) StreamingSessionSettings* settings;
@property (nonatomic, strong, readonly) NSString* sessionId;
@end

@protocol StreamingSessionProtocol
@optional
-(void)streamingSession:(StreamingSession*)sess Authenticated:(BOOL)status;
-(void)streamingSession:(StreamingSession*)sess AllMediaLibraryMetadata:(MediaLibraryInfo*)pInfo;
-(void)streamingSession:(StreamingSession*)sess MediaArtworkImage:(UIImage*)image;
@end
