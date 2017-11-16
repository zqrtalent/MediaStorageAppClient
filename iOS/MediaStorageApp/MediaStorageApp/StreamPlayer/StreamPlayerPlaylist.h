//
//  PlayerQueueManager.h
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 6/27/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "../Models/AudioMetadataInfo.h"

@interface StreamPlayerPlaylist : NSObject

@property (nonatomic, readonly, getter=getNumberItems) NSInteger itemsNum;
@property (nonatomic, readonly, getter=getPlayingMedia) AudioMetadataInfo* playingMedia;

-(instancetype)init;
-(void)add:(AudioMetadataInfo*)mediaInfo;
-(void)clear;
-(AudioMetadataInfo*)mediaByIndex:(int)index UseAsCurrent:(BOOL)useAsCurrent;
-(AudioMetadataInfo*)nextMedia:(BOOL)useAsCurrent;
-(AudioMetadataInfo*)prevMedia:(BOOL)useAsCurrent;

@end
