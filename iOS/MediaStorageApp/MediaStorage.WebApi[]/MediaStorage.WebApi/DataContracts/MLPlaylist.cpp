//
//  MLPlaylist.cpp
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 5/1/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#include "MLPlaylist.h"

MLPlaylist::MLPlaylist(){
}

BEGIN_DECLARE_VAR(MLPlaylist, Serializable)
DECLARE_VAR(_T("MLPlaylist"),		_T(""), VariableType::VT_None, 0, false, true, false)
DECLARE_VAR(_T("Id"),			_T(""), VariableType::VT_String, offsetof(MLPlaylist,_id), false, false, false)
DECLARE_VAR(_T("Name"),		_T(""), VariableType::VT_String, offsetof(MLPlaylist,_name), false, false, false)
DECLARE_VAR(_T("ArtworkImageId"),			_T(""), VariableType::VT_String, offsetof(MLPlaylist,_artworkImageId), false, false, false)
DECLARE_VAR(_T("Songs"),			_T(""), VariableType::VT_None, offsetof(MLPlaylist,_songs), false, true, true)
END_DECLARE_VAR()

MLPlaylist::~MLPlaylist(){
}
