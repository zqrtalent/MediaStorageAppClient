//
//  MediaPacket.cpp
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 5/1/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#include "MediaPacket.h"

MediaPacket::MediaPacket(){
}

BEGIN_DECLARE_VAR(MediaPacket, Serializable)
DECLARE_VAR(_T("MediaPacket"),	_T(""), VariableType::VT_None, 0, false, true, false)
DECLARE_VAR(_T("Data"),			_T(""), VariableType::VT_Binary, offsetof(MediaPacket,_data), false, false, false)
DECLARE_VAR(_T("SamplePerFrame"),			_T(""), VariableType::VT_Int32, offsetof(MediaPacket,_samplePerFrame), false, false, false)
END_DECLARE_VAR()

MediaPacket::~MediaPacket(){
}
