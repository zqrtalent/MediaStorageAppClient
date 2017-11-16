//
//  MLAlbum.cpp
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 5/1/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#include "MLAlbum.h"

MLAlbum::MLAlbum(){
}

BEGIN_DECLARE_VAR(MLAlbum, Serializable)
DECLARE_VAR(_T("MLPlaylistSong"),		_T(""), VariableType::VT_None, 0, false, true, false)
DECLARE_VAR(_T("Id"),			_T(""), VariableType::VT_String, offsetof(MLAlbum,_id), false, false, false)
DECLARE_VAR(_T("Name"),		_T(""), VariableType::VT_String, offsetof(MLAlbum,_name), false, false, false)
DECLARE_VAR(_T("Year"),         _T(""), VariableType::VT_Int32, offsetof(MLAlbum,_year), false, false, false)
DECLARE_VAR(_T("ArtworkImageId"),			_T(""), VariableType::VT_String, offsetof(MLAlbum,_artworkImageId), false, false, false)
DECLARE_VAR(_T("Songs"),			_T(""), VariableType::VT_None, offsetof(MLAlbum,_songs), false, true, true)
END_DECLARE_VAR()

MLAlbum::~MLAlbum(){
}
