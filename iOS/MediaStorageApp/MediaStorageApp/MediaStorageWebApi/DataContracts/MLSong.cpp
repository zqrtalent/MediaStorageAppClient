//
//  MLSong.cpp
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 5/1/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#include "MLSong.h"

MLSong::MLSong(){
}

BEGIN_DECLARE_VAR(MLSong, Serializable)
DECLARE_VAR(_T("MLPlaylistSong"), _T(""), VariableType::VT_None, 0, false, true, false)
DECLARE_VAR(_T("Id"),           _T(""), VariableType::VT_String, offsetof(MLSong,_id), false, false, false)
DECLARE_VAR(_T("Name"),         _T(""), VariableType::VT_String, offsetof(MLSong,_name), false, false, false)
DECLARE_VAR(_T("DurationSec"),         _T(""), VariableType::VT_Int32, offsetof(MLSong,_durationSec), false, false, false)
DECLARE_VAR(_T("Track"),			_T(""), VariableType::VT_Int32, offsetof(MLSong,_track), false, false, false)
END_DECLARE_VAR()

MLSong::~MLSong(){
}



