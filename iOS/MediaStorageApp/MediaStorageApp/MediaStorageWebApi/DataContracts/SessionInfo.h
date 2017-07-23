//
//  SessionInfo.hpp
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 5/1/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#ifndef SessionInfo_hpp
#define SessionInfo_hpp

#include "Serialize/Serializable.h"

class SessionInfo : public Serializable
{
public:
    SessionInfo();
    ~SessionInfo();
    
public:
    _string _sessionKey;
    _string _playingMediaId;
    int _playingAtMSec;
    
public:
    virtual Serializable*	CreateSerializableObject(){return new SessionInfo();};
    
protected: // Serialization.
    INIT_RUNTIME_VARIABLE()
};

#endif /* SessionInfo_hpp */
