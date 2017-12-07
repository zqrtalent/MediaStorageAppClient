//
//  AudioStreamPacketsInfo.m
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 10/22/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "AudioStreamPacketsInfo.h"

@interface AudioStreamPacketsInfo()

@property (nonatomic, strong) NSMutableData* packetsData;
@property (nonatomic, assign) UInt32 packetsDataSizeUsed;

@property (nonatomic, strong) NSMutableData* packetsDescData;
@property (nonatomic, assign) UInt32 packetsDescDataSizeUsed;
@property (nonatomic, assign) AudioStreamPacketDescription* packetsDesc;

@end

@implementation AudioStreamPacketsInfo

-(instancetype)init:(UInt32)bufferCapacityInBytes
{
    self.packetsData = [NSMutableData dataWithLength:bufferCapacityInBytes];
    self.packetsDataSizeUsed = 0;
    self.packetsDescData = nil;
    self.packetsDescDataSizeUsed = 0;
    self.offset = 0;
    self.packetsCt = 0;
    
    return [super init];
}

-(void)addAudioPacket:(const void*)packetData PacketDataSize:(UInt32)packetDataSize StreamPacketDesc:(AudioStreamPacketDescription&)streamPacketDesc
{
    // Validate input parameters.
    assert(packetData != nullptr);
    assert(packetDataSize > 0);
    
    // Copy packet data.
    void* pDest = nullptr;
    if(self.packetsData != nil)
    {
        if(self.packetsDataSizeUsed + packetDataSize > self.packetsData.length)
            [self.packetsData appendBytes:packetData length:packetDataSize];
        else
            pDest = ((Byte*)self.packetsData.mutableBytes) + self.packetsDataSizeUsed;
    }
    else
    {
        self.packetsData = [NSMutableData dataWithLength:packetDataSize];
        pDest = self.packetsData.mutableBytes;
    }
    
    if(pDest != nullptr)
        memcpy(pDest, packetData, packetDataSize);
    
    // Copy packet description info.
    if( self.packetsDescData != nil )
    {
        if( self.packetsDescDataSizeUsed + sizeof(AudioStreamPacketDescription) > self.packetsDescData.length )
        {
            AudioStreamPacketDescription packetsDescTemp[10];
            memset(packetsDescTemp, 0, sizeof(packetsDescTemp));
            [self.packetsDescData appendBytes:packetsDescTemp length:sizeof(packetsDescTemp)];
        }
    }
    else
        self.packetsDescData = [NSMutableData dataWithLength:sizeof(AudioStreamPacketDescription)*10];
    
    self.packetsDesc = ((AudioStreamPacketDescription*)self.packetsDescData.mutableBytes);
    self.packetsDesc[self.packetsCt].mStartOffset = self.packetsDataSizeUsed;
    self.packetsDesc[self.packetsCt].mDataByteSize = streamPacketDesc.mDataByteSize;
    self.packetsDesc[self.packetsCt].mVariableFramesInPacket = streamPacketDesc.mVariableFramesInPacket;
    
    
    self.packetsDataSizeUsed += packetDataSize;
    self.packetsDescDataSizeUsed += sizeof(AudioStreamPacketDescription);
    self.packetsCt ++;
}

-(void)clearPacketsData:(bool)disposeMemoryBuffers
{
    self.offset = 0;
    self.packetsCt = 0;
    self.packetsDataSizeUsed = 0;
    self.packetsDescDataSizeUsed = 0;
    
    if(disposeMemoryBuffers)
    {
        self.packetsData = nil;
        self.packetsDescData = nil;
    }
}

-(const void*)getAudioPacketDataByIndex:(UInt32)index
{
    UInt32 offset = 0;
    if(index >= self.packetsCt)
        return nullptr;
    
    int loop=0;
    while(loop < index)
    {
        offset += self.packetsDesc[loop++].mDataByteSize;
    }
    return (const void*)&((Byte*)self.data)[offset];
}

-(UInt32)read:(NSRange)range OutBuffer:(void*)pBuffer OutBufferSize:(UInt32)bufferSize OutPacketsDesc:(AudioStreamPacketDescription*)packetsDesc;
{
    if(range.location < self.offset || (range.location + range.length) > (self.offset + self.packetsCt))
        return 0;
    
    long offsetSrc = range.location, offsetDest = 0;
    long packetStartOffset = 0;
    UInt32 dataByteSize = 0;
    
    const Byte* pDataBufferSrc = (const Byte*)self.packetsData.bytes;
    Byte* pDataBufferDest = (Byte*)pBuffer;
    
    while(offsetSrc < (range.location + range.length))
    {
        dataByteSize = self.packetsDesc[offsetSrc].mDataByteSize;
        packetsDesc[offsetDest].mStartOffset = packetStartOffset;
        packetsDesc[offsetDest].mDataByteSize = dataByteSize;
        packetsDesc[offsetDest].mVariableFramesInPacket = self.packetsDesc[offsetSrc].mVariableFramesInPacket;
        offsetSrc ++;
        
        if(bufferSize < packetStartOffset + dataByteSize)
            break;
        
        memcpy(pDataBufferDest, pDataBufferSrc, dataByteSize);
        
        packetStartOffset += dataByteSize;
        pDataBufferSrc += dataByteSize;
        pDataBufferDest += dataByteSize;
        offsetDest ++;
    }
    
    return (UInt32)offsetDest;
}

-(const void*)retrievePacketsDataBuffer
{
    return self.packetsData.bytes;
}

-(UInt32)retrievePacketsDataBufferSize
{
    return (UInt32)self.packetsData.length;
}

-(UInt32)retrievePacketsDataBufferSizeUsed
{
    return self.packetsDataSizeUsed;
}

-(AudioStreamPacketDescription*)retrievePacketsDescription
{
    return self.packetsDesc;
}
@end
