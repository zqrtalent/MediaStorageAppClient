//
//  MLPlaylist.hpp
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 5/1/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#ifndef MLPlaylist_hpp
#define MLPlaylist_hpp

#include "Serialize/Serializable.h"
#include "MLSong.h"

class MLPlaylist : public Serializable
{
public:
    MLPlaylist();
    ~MLPlaylist();
    
public:
    _string _id;
    _string _name;
    _string _artworkImageId;
    EnumerableObject<MLSong> _songs;
    
public:
    virtual Serializable*	CreateSerializableObject(){return new MLPlaylist();};
    
protected: // Serialization.
    INIT_RUNTIME_VARIABLE()
};


#endif /* MLPlaylist_hpp */
