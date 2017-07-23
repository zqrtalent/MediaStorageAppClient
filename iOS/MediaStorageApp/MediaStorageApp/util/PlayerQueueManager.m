//
//  PlayerQueueManager.m
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 6/27/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "PlayerQueueManager.h"
@interface PlayerQueueManager()

@property (nonatomic, strong) NSMutableArray* playlist;
@property (nonatomic, assign) int currentMediaIndex;

@end

@implementation PlayerQueueManager

-(instancetype)init{
    [super init];
    
    self.playlist = [[NSMutableArray alloc] init];
    self.currentMediaIndex = -1;
    return self;
}

-(void)add:(MediaInfo*)media{
    [self.playlist addObject:media];
}

-(void)clear{
    [self.playlist removeAllObjects];
    self.currentMediaIndex = -1;
}

-(MediaInfo*)mediaByIndex:(int)index UseAsCurrent:(BOOL)useAsCurrent{
    if(self.playlist == nil || index < 0 || index >= self.playlist.count)
        return nil;
    if(useAsCurrent)
        self.currentMediaIndex = index;
    return self.playlist[index];
}

-(MediaInfo*)nextMedia:(BOOL)useAsCurrent{
    if(self.playlist == nil || self.currentMediaIndex >= self.playlist.count)
        return nil;
    if(!useAsCurrent)
        return self.playlist[self.currentMediaIndex+1];
    self.currentMediaIndex ++;
    return self.playlist[self.currentMediaIndex];
}

-(MediaInfo*)prevMedia:(BOOL)useAsCurrent{
    if(self.playlist == nil || self.currentMediaIndex <= 0)
        return nil;
    if(!useAsCurrent)
        return self.playlist[self.currentMediaIndex-1];
    self.currentMediaIndex --;
    return self.playlist[self.currentMediaIndex];
}

@end
