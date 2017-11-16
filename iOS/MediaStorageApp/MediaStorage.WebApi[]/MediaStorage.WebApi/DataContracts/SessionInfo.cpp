//
//  SessionInfo.cpp
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 5/1/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#include "SessionInfo.h"

SessionInfo::SessionInfo(){
}

BEGIN_DECLARE_VAR(SessionInfo, Serializable)
DECLARE_VAR(_T("SessionInfo"),	_T(""), VariableType::VT_None, 0, false, true, false)
DECLARE_VAR(_T("SessionKey"),			_T(""), VariableType::VT_String, offsetof(SessionInfo,_sessionKey), false, false, false)
DECLARE_VAR(_T("PlayingMediaId"),			_T(""), VariableType::VT_String, offsetof(SessionInfo,_playingMediaId), false, false, false)
DECLARE_VAR(_T("PLayingAtMSec"),			_T(""), VariableType::VT_Int32, offsetof(SessionInfo,_playingAtMSec), false, false, false)
END_DECLARE_VAR()

SessionInfo::~SessionInfo(){
}
