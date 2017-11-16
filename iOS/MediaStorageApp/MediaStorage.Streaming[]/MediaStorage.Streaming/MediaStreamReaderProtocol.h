//
//  MediaStreamReaderProtocol.h
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 5/11/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import <Foundation/Foundation.h>

@protocol MediaStreamReaderProtocol

@required
-(void)mediaPacketsDownloadStarted:(bool)resumed;
-(void)mediaPacketsDownloadStopped;
-(void)mediaPacketsDownloadPaused;
-(void)mediaPacketsDownloadProgress:(long)packetOffset packetsCt:(int)packetsCt isEof:(bool)isEof;
@end
