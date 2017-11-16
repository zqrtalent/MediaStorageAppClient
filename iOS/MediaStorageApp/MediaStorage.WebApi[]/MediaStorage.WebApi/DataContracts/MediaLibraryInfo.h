//
//  MediaLibraryInfo.hpp
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 5/1/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#ifndef MediaLibraryInfo_h
#define MediaLibraryInfo_h

#include "Serialize/Serializable.h"
#include "MLPlaylist.h"
#include "MLArtist.h"

class MediaLibraryInfo : public Serializable
{
public:
    MediaLibraryInfo();
    ~MediaLibraryInfo();
    
public:
    int                             _lastUpdated;
    EnumerableObject<MLArtist>      _artists;
    EnumerableObject<MLPlaylist>    _playlists;
    
    MLArtist*   GetArtistById(_string* pArtistId);
    MLAlbum*    GetAlbumById(MLArtist* pArtist, _string* pAlbumId);
    
    // Set current album/artist/playlist to get access to the next/prev songs with shuffle option as well.
    bool SetCurrentAlbum(MLAlbum* pAlbum);
    bool SetCurrentArtist(MLArtist* pArtist);
    bool SetCurrentPlaylist(MLPlaylist* pPlaylist);
    
    // Access current song/artist/playlist
    int GetCurrentSongsNum();
    
    // Sets current song by index based on current album/artist/playlist and retrieves song object.
    MLSong* SetCurrentSongByIndex(int index);
    
    // Retrieves current artist object.
    MLArtist* GetCurrentArtist();
    
    // Retrieves current album object.
    MLAlbum* GetCurrentAlbum();
    
    // Retrievs current playlist object.
    MLPlaylist* GetCurrentPlaylist();
    
protected:
    MLArtist*                 _currentArtist;
    MLAlbum*                  _currentAlbum;
    MLPlaylist*               _currentPlaylist;
    
public:
    virtual Serializable*	CreateSerializableObject(){return new MediaLibraryInfo();};
    
protected: // Serialization.
    INIT_RUNTIME_VARIABLE()
};

#endif /* MediaLibraryInfo_h */
