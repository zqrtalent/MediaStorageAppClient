//
//  MediaStreamProtocol.h
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 2/11/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>

typedef void (*StreamReadCallbackProc)(void* streamData, UInt32 dataSize, int durationMS, bool eof, void* lpUserData);

@protocol MediaStreamProtocol <NSObject>

@required
-(void*)open:(NSString*)sessionKey Media:(NSString*)mediaId WithCallback:(StreamReadCallbackProc)readStreamCallback;

-(UInt64)getNumberPackets:(float*)packetDurationMS;

-(float)getMediaDurationInSeconds;

-(NSString*)getSongName;

-(NSString*)getArtistName;

-(NSString*)getAlbumName;

-(BOOL)getStreamDescription:(AudioStreamBasicDescription*)streamDesc;

-(void)close:(NSString*)token;

-(bool)readPacketData:(UInt32)packetSize
         PacketOffset:(UInt32)packetOffset
               Buffer:(void*)buffer
            GivenSize:(UInt32)bufferSize
           PacketDesc:(AudioStreamPacketDescription*)streamPacketsDesc
              OutSize:(UInt32*)outDataSize;

-(UInt32)getPacketSizeInBytes;
@end

