//
//  MediaPackets.cpp
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 5/1/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#include "MediaPackets.h"

MediaPackets::MediaPackets(){
}

BEGIN_DECLARE_VAR(MediaPackets, Serializable)
DECLARE_VAR(_T("MediaPackets"),		_T(""), VariableType::VT_None, 0, false, true, false)
DECLARE_VAR(_T("Offset"),			_T(""), VariableType::VT_UInt64, offsetof(MediaPackets,_offset), false, false, false)
DECLARE_VAR(_T("NumPackets"),		_T(""), VariableType::VT_Int32, offsetof(MediaPackets,_numPackets), false, false, false)
DECLARE_VAR(_T("SamplesPerFrame"),		_T(""), VariableType::VT_Int32, offsetof(MediaPackets,_samplesPerFrame), false, false, false)
DECLARE_VAR(_T("Packets"),			_T(""), VariableType::VT_None, offsetof(MediaPackets,_packets), false, true, true)
DECLARE_VAR(_T("IsEof"),			_T(""), VariableType::VT_Bool, offsetof(MediaPackets,_isEof), false, false, false)
DECLARE_VAR(_T("FramesCt"),		_T(""), VariableType::VT_UInt64, offsetof(MediaPackets,_framesCt), false, false, false)
DECLARE_VAR(_T("IsVbr"),		_T(""), VariableType::VT_Bool, offsetof(MediaPackets,_isVbr), false, false, false)
END_DECLARE_VAR()

MediaPackets::~MediaPackets(){
    
}
