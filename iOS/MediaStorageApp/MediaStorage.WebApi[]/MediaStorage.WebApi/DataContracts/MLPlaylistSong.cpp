//
//  MLPlaylistSong.cpp
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 5/1/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#include "MLPlaylistSong.h"


MLPlaylistSong::MLPlaylistSong(){
}

BEGIN_DECLARE_VAR(MLPlaylistSong, Serializable)
DECLARE_VAR(_T("MLPlaylistSong"),		_T(""), VariableType::VT_None, 0, false, true, false)
DECLARE_VAR(_T("Id"),			_T(""), VariableType::VT_String, offsetof(MLPlaylistSong,_id), false, false, false)
DECLARE_VAR(_T("Song"),		_T(""), VariableType::VT_String, offsetof(MLPlaylistSong,_song), false, false, false)
DECLARE_VAR(_T("Track"),         _T(""), VariableType::VT_Int32, offsetof(MLPlaylistSong,_track), false, false, false)
DECLARE_VAR(_T("Artist"),			_T(""), VariableType::VT_String, offsetof(MLPlaylistSong,_artist), false, false, false)
DECLARE_VAR(_T("Album"),			_T(""), VariableType::VT_String, offsetof(MLPlaylistSong,_album), false, false, false)
DECLARE_VAR(_T("Year"),         _T(""), VariableType::VT_Int32, offsetof(MLPlaylistSong,_year), false, false, false)
DECLARE_VAR(_T("Genre"),			_T(""), VariableType::VT_String, offsetof(MLPlaylistSong,_genre), false, false, false)
END_DECLARE_VAR()

MLPlaylistSong::~MLPlaylistSong(){
}
