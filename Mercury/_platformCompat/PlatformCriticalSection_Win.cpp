#include "PlatformCriticalSection_Win.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CriticalSection::CriticalSection() {
	InitializeCriticalSection(&section_);
	}

CriticalSection::~CriticalSection(){
	::DeleteCriticalSection(&section_);
	}

void
CriticalSection::Lock(){
	::EnterCriticalSection(&section_);
	}

void
CriticalSection::Unlock(){
	::LeaveCriticalSection(&section_);
	}