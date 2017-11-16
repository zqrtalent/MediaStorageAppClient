//
//  AudioPacketsDownloaderProtocol.h
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 10/8/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

@protocol AudioPacketsDownloaderProtocol

-(void)audioPacketsDownloadStarted:(BOOL)resumed;
-(void)audioPacketsDownloadStopped;
-(void)audioPacketsDownloadPaused;
-(void)audioPacketsDownloadProgress:(long)packetOffset PacketsCt:(int)packetsCt IsEof:(bool)isEof;

@end
