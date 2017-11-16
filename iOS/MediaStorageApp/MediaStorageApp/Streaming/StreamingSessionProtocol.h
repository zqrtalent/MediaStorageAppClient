//
//  StreamingSessionDelegate.h
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 10/7/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "StreamingSession.h"

@protocol StreamingSessionProtocol1

@optional
-(void)streamingSession:(StreamingSession*)sess Authenticated:(BOOL)status;
-(void)streamingSession:(StreamingSession*)sess AllMediaLibraryMetadata:(BOOL)status;
-(void)streamingSession:(StreamingSession*)sess MediaArtworkImage:(UIImage*)image;

@end
