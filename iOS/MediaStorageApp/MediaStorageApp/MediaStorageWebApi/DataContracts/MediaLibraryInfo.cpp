//
//  MediaLibraryInfo.cpp
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 5/1/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#include "MediaLibraryInfo.h"

MediaLibraryInfo::MediaLibraryInfo()
{
    _artists.CreateVariableIndex(_T("Id"), Compare_String);
}

BEGIN_DECLARE_VAR(MediaLibraryInfo, Serializable)
DECLARE_VAR(_T("MLPlaylist"),		_T(""), VariableType::VT_None, 0, false, true, false)
DECLARE_VAR(_T("LastUpdated"),			_T(""), VariableType::VT_Int32, offsetof(MediaLibraryInfo,_lastUpdated), false, false, false)
DECLARE_VAR(_T("Artists"),			_T(""), VariableType::VT_None, offsetof(MediaLibraryInfo,_artists), false, true, true)
DECLARE_VAR(_T("Playlists"),			_T(""), VariableType::VT_None, offsetof(MediaLibraryInfo,_playlists), false, true, true)
END_DECLARE_VAR()

MediaLibraryInfo::~MediaLibraryInfo(){
}


MLArtist*
MediaLibraryInfo::GetArtistById(_string* pArtistId){
    return _artists.FindOneVariable(_T("Id"), pArtistId);
}

MLAlbum*
MediaLibraryInfo::GetAlbumById(MLArtist* pArtist, _string* pAlbumId){
    if(!pArtist)
        return nullptr;
    
    int loop = 0;
    while(loop < pArtist->_albums.GetCount()){
        if(pArtist->_albums[loop] && pArtist->_albums.GetAt(loop)->_id.compare(*pAlbumId) == 0)
            return pArtist->_albums[loop];
        loop ++;
    }
    return nullptr;
}


// Set current album/artist/playlist to get access to the next/prev songs with shuffle option as well.
bool
MediaLibraryInfo::SetCurrentAlbum(MLAlbum* pAlbum){
    this->_currentAlbum = pAlbum;
    this->_currentPlaylist = nullptr;
    this->_currentArtist = nullptr;
    return NO;
}

bool
MediaLibraryInfo::SetCurrentArtist(MLArtist* pArtist){
    this->_currentAlbum = nullptr;
    this->_currentPlaylist = nullptr;
    this->_currentArtist = pArtist;
    return NO;
}

bool
MediaLibraryInfo::SetCurrentPlaylist(MLPlaylist* pPlaylist){
    this->_currentAlbum = nullptr;
    this->_currentPlaylist = pPlaylist;
    this->_currentArtist = nullptr;
    return NO;
}

// Access current song/artist/playlist
int
MediaLibraryInfo::GetCurrentSongsNum(){
    return 0;
}

// Sets current song by index based on current album/artist/playlist and retrieves song object.
MLSong*
MediaLibraryInfo::SetCurrentSongByIndex(int index){
    return nullptr;
}

// Retrieves current artist object.
MLArtist*
MediaLibraryInfo::GetCurrentArtist(){
    return nullptr;
}

// Retrieves current album object.
MLAlbum*
MediaLibraryInfo::GetCurrentAlbum(){
    return nullptr;
}

// Retrievs current playlist object.
MLPlaylist*
MediaLibraryInfo::GetCurrentPlaylist(){
    return nullptr;
}
