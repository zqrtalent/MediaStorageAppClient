//
//  MediaPacket.hpp
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 5/1/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#ifndef MediaPacket_hpp
#define MediaPacket_hpp

#include "Serialize/Serializable.h"

class MediaPacket : public Serializable
{
public:
    MediaPacket();
    ~MediaPacket();
    
public:
    LongBinary _data;
    int _samplePerFrame; // Used only for VBR
public:
    virtual Serializable*	CreateSerializableObject(){return new MediaPacket();};
    
protected: // Serialization.
    INIT_RUNTIME_VARIABLE()
};

#endif /* MediaPacket_hpp */
