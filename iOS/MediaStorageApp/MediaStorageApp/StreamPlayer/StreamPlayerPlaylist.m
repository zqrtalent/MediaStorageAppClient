//
//  PlayerQueueManager.m
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 6/27/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "StreamPlayerPlaylist.h"

@interface StreamPlayerPlaylist()

@property (nonatomic, strong) NSMutableArray* playlist;
@property (nonatomic, assign) int currentMediaIndex;

-(NSInteger)getNumberItems;
-(AudioMetadataInfo*)getPlayingMedia;

@end

@implementation StreamPlayerPlaylist

-(instancetype)init
{
    self.playlist = [[NSMutableArray alloc] init];
    self.currentMediaIndex = -1;
    return [super init];
}

-(void)add:(AudioMetadataInfo*)mediaInfo
{
    [self.playlist addObject:mediaInfo];
}

-(void)clear
{
    [self.playlist removeAllObjects];
    self.currentMediaIndex = -1;
}

-(AudioMetadataInfo*)mediaByIndex:(int)index UseAsCurrent:(BOOL)useAsCurrent
{
    if(self.playlist == nil || index < 0 || index >= self.playlist.count)
        return nil;
    if(useAsCurrent)
        self.currentMediaIndex = index;
    return self.playlist[index];
}

-(AudioMetadataInfo*)nextMedia:(BOOL)useAsCurrent
{
    if(self.playlist == nil || self.currentMediaIndex >= self.playlist.count)
        return nil;
    if(!useAsCurrent)
        return self.playlist[self.currentMediaIndex+1];
    self.currentMediaIndex ++;
    return self.playlist[self.currentMediaIndex];
}

-(AudioMetadataInfo*)prevMedia:(BOOL)useAsCurrent
{
    if(self.playlist == nil || self.currentMediaIndex <= 0)
        return nil;
    if(!useAsCurrent)
        return self.playlist[self.currentMediaIndex-1];
    self.currentMediaIndex --;
    return self.playlist[self.currentMediaIndex];
}

-(NSInteger)getNumberItems
{
    return [self.playlist count];
}

-(AudioMetadataInfo*)getPlayingMedia
{
    return (self.currentMediaIndex > -1) ? self.playlist[self.currentMediaIndex] : nil;
}

@end
