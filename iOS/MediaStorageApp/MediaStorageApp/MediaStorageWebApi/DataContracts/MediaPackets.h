//
//  MediaPackets.hpp
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 5/1/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#ifndef MediaPackets_hpp
#define MediaPackets_hpp

#include "Serialize/Serializable.h"
#include "MediaPacket.h"

#ifndef MP3_FRAME_DURATION_MSEC
#define MP3_FRAME_DURATION_MSEC 26.122449
#endif


class MediaPackets : public Serializable
{
public:
    MediaPackets();
    ~MediaPackets();
    
public:
    unsigned long   _offset;
    int             _numPackets;
    int             _samplesPerFrame; // // Samples per frame for CBR
    EnumerableObject<MediaPacket>  _packets;
    bool            _isEof;
    unsigned long   _framesCt; // Number of frames in media file.
    BOOL            _isVbr;
    
public:
    virtual Serializable*	CreateSerializableObject	(){return new MediaPackets();};
    
protected: // Serialization.
    INIT_RUNTIME_VARIABLE()
};

/*
 [DataMember]
 public ulong Offset { get; set; }
 
 [DataMember]
 public int NumPackets { get; set; }
 
 [DataMember]
 public int SamplesPerFrame { get; set; } // Samples per frame for CBR
 
 [DataMember]
 public List<MediaPacket> Packets { get; set; }
 
 [DataMember]
 public bool IsEof { get; set; }
 
 [DataMember]
 public ulong FramesCt { get; set; } // Frames count in entire media file
 
 [DataMember]
 public bool IsVBR { get; set; }
 */

#endif /* MediaPackets_hpp */
