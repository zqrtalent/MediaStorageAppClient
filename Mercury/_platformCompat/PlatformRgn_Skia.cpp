#include "PlatformRgn_Win.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Rgn::Rgn() : rgn_(NULL) {
	}

Rgn::~Rgn() {
	DeleteObject();
	}

BOOL
Rgn::Attach(RGNDef rgn){
	if( !IsNull() ){
		ASSERT(FALSE);
		return FALSE;
		}
	rgn_ = rgn;
	return TRUE;
	}

RGNDef
Rgn::Detach(){
	if( IsNull() ){
		ASSERT(FALSE);
		return FALSE;
		}
	RGNDef ret = rgn_;
	rgn_ = NULL;
	return ret;
	}

BOOL
Rgn::DeleteObject(){
	if( !IsNull() ){
		::DeleteObject(rgn_);
		rgn_ = NULL;
		}
	return TRUE;
	}

BOOL
Rgn::CopyRegion(Rgn* pCopyInto){
	if( IsNull() || !pCopyInto )
		return FALSE;
	return FALSE;
	}
