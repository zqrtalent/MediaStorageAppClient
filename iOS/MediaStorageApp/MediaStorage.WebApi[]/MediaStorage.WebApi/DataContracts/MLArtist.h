//
//  MLArtist.hpp
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 5/1/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

//#pragma once;
#ifndef MLArtist_hpp
#define MLArtist_hpp

#include "Serialize/Serializable.h"
#include "MLAlbum.h"

class MLArtist : public Serializable
{
public:
    MLArtist();
    ~MLArtist();
    
public:
    _string _id;
    _string _name;
    _string _genre;
    _string _artworkImageId;
    EnumerableObject<MLAlbum> _albums;
    
public:
    virtual Serializable*	CreateSerializableObject(){return new MLArtist();};
    
protected: // Serialization.
    INIT_RUNTIME_VARIABLE()
};

#endif /* MLArtist_hpp */
