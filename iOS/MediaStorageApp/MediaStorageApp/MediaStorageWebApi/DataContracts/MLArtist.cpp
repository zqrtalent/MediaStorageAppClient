//
//  MLArtist.cpp
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 5/1/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#include "MLArtist.h"

MLArtist::MLArtist(){
}

BEGIN_DECLARE_VAR(MLArtist, Serializable)
DECLARE_VAR(_T("MLArtist"),		_T(""), VariableType::VT_None, 0, false, true, false)
DECLARE_VAR(_T("Id"),			_T(""), VariableType::VT_String, offsetof(MLArtist,_id), false, false, false)
DECLARE_VAR(_T("Name"),		_T(""), VariableType::VT_String, offsetof(MLArtist,_name), false, false, false)
DECLARE_VAR(_T("Genre"),         _T(""), VariableType::VT_String, offsetof(MLArtist,_genre), false, false, false)
DECLARE_VAR(_T("ArtworkImageId"),			_T(""), VariableType::VT_String, offsetof(MLArtist,_artworkImageId), false, false, false)
DECLARE_VAR(_T("Albums"),			_T(""), VariableType::VT_None, offsetof(MLArtist,_albums), false, true, true)
END_DECLARE_VAR()

MLArtist::~MLArtist(){
}
