//
//  MediaFileStream.h
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 12/26/16.
//  Copyright © 2016 zaqro butskrikidze. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>
#import "MediaStreamProtocol.h"

@interface MediaFileStream<MediaStreamProtocol> : NSObject
{
}

-(void*)open:(NSString*)token Media:(NSString*)mediaId WithCallback:(StreamReadCallbackProc)readStreamCallback;
-(UInt64)getNumberPackets:(float*)packetDurationMS;
-(float)getMediaDurationInSeconds;
-(NSString*)getSongName;
-(NSString*)getArtistName;
-(NSString*)getAlbumName;

-(BOOL)getStreamDescription:(AudioStreamBasicDescription*)streamDesc;
-(void)close:(NSString*)token;
-(bool)readPacketData:(UInt32)packetSize PacketOffset:(UInt32)packetOffset Buffer:(void*)buffer GivenSize:(UInt32)bufferSize PacketDesc:(AudioStreamPacketDescription*)streamPacketsDesc OutSize:(UInt32*)outDataSize;
-(AudioFileID)getFileId;
-(UInt32)getPacketSizeInBytes;
@end
