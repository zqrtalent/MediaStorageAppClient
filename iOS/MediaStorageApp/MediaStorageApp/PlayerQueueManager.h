//
//  PlayerQueueManager.h
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 6/27/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

/*
 
 */

#import <Foundation/Foundation.h>
#import "MediaInfo.h"

@interface PlayerQueueManager : NSObject

@property (nonatomic, readonly, getter=getNumberItems) NSInteger itemsNum;
@property (nonatomic, readonly, getter=getPlayingMedia) MediaInfo* playingMedia;

-(instancetype)init;

-(void)add:(MediaInfo*)media;

-(bool)removeByMedia:(MediaInfo*)media;

-(bool)removeByIndex:(int)index;

-(void)clear;

-(MediaInfo*)mediaByIndex:(int)index UseAsCurrent:(BOOL)useAsCurrent;

-(MediaInfo*)nextMedia:(BOOL)useAsCurrent;

-(MediaInfo*)prevMedia:(BOOL)useAsCurrent;

@end
