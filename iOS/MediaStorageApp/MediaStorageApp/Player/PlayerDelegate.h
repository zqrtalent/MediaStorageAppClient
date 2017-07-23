//
//  PlayerDelegate.h
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 6/18/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#ifndef PlayerDelegate_h
#define PlayerDelegate_h

@protocol PlayerDelegate
@optional
-(void)onPlayStarted:(BOOL)resumed;
-(void)onPlayEnded;
-(void)onPaused;
-(void)onBufferingStarted;
-(void)onBufferingEnded;
-(void)onPlayTimeUpdate:(unsigned int)msec;
@end


#endif /* PlayerDelegate_h */
