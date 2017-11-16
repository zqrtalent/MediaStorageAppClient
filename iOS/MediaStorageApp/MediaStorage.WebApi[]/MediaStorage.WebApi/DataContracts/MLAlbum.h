//
//  MLAlbum.hpp
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 5/1/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#ifndef MLAlbum_hpp
#define MLAlbum_hpp

#include "Serialize/Serializable.h"
#include "MLSong.h"

class MLAlbum : public Serializable
{
public:
    MLAlbum();
    ~MLAlbum();
    
public:
    _string _id;
    _string _name;
    int _year;
    _string _artworkImageId;
    EnumerableObject<MLSong> _songs;
    
public:
    virtual Serializable*	CreateSerializableObject(){return new MLAlbum();};
    
protected: // Serialization.
    INIT_RUNTIME_VARIABLE()
};


#endif /* MLAlbum_hpp */
