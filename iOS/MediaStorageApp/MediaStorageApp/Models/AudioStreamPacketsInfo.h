//
//  AudioStreamPacketsInfo.h
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 10/22/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>

@interface AudioStreamPacketsInfo : NSObject

@property (nonatomic, assign) long offset;
@property (nonatomic, assign) UInt32 packetsCt;
@property (nonatomic, readonly, getter=retrievePacketsDataBuffer) const void* data;
@property (nonatomic, readonly, getter=retrievePacketsDataBufferSize) UInt32 dataSize;
@property (nonatomic, readonly, getter=retrievePacketsDescription) AudioStreamPacketDescription* streamPacketsDesc;

-(instancetype)init:(UInt32)bufferCapacityInBytes;

-(void)addAudioPacket:(const void*)packetData PacketDataSize:(UInt32)packetDataSize StreamPacketDesc:(AudioStreamPacketDescription&)streamPacketDesc;

-(void)clearPacketsData:(bool)disposeMemoryBuffers;

/*Returns number of packets read.*/
-(UInt32)read:(NSRange)range OutBuffer:(void*)pBuffer OutBufferSize:(UInt32)bufferSize OutPacketsDesc:(AudioStreamPacketDescription*)packetsDesc;

@end
