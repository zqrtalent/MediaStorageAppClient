//
//  Header.h
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 5/1/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#ifndef MPLAYLISTSONG_H
#define MPLAYLISTSONG_H

#include "Serialize/Serializable.h"

class MLPlaylistSong : public Serializable
{
public:
    MLPlaylistSong();
    ~MLPlaylistSong();
    
public:
    _string _id;
    _string _song;
    int _track;
    _string _artist;
    _string _album;
    int _year;
    _string _genre;
    
public:
    virtual Serializable*	CreateSerializableObject(){return new MLPlaylistSong();};
    
protected: // Serialization.
    INIT_RUNTIME_VARIABLE()
};

#endif /* MPLAYLISTSONG_H */
