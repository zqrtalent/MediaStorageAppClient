//
//  RemoteCommandsProtocol.h
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 6/26/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import <MediaPlayer/MediaPlayer.h>

@protocol RemoteCommandsProtocol
-(MPRemoteCommandHandlerStatus)onPlayCommand;
-(MPRemoteCommandHandlerStatus)onPauseCommand;
-(MPRemoteCommandHandlerStatus)onTogglePlayPauseCommand;
-(MPRemoteCommandHandlerStatus)onNextTrackCommand;
-(MPRemoteCommandHandlerStatus)onPrevTrackCommand;
@end
