//
//  MediaURLStream.h
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 2/11/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MediaStreamProtocol.h"

@interface MediaURLStream<MediaStreamProtocol> : NSObject
{
}

-(void)init;
-(void*)open:(NSString*)sessionKey Media:(NSString*)mediaId WithCallback:(StreamReadCallbackProc)readStreamCallback;
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

