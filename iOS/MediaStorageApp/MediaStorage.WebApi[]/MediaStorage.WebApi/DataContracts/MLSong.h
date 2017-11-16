//
//  MLSong.hpp
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 5/1/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#ifndef MLSong_hpp
#define MLSong_hpp

#include "Serialize/Serializable.h"

class MLSong : public Serializable
{
public:
    MLSong();
    ~MLSong();
    
public:
    _string _id;
    _string _name;
    int _durationSec;
    int _track;
    
public:
    virtual Serializable*	CreateSerializableObject(){return new MLSong();};
    
protected: // Serialization.
    INIT_RUNTIME_VARIABLE()
};

#endif /* MLSong_hpp */
