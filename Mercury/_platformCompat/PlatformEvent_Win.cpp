#include "PlatformEvent_Win.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Event::Event(bool bManualReset /*= true*/, bool bInitialSignal /*= false*/) {
	event_ = ::CreateEvent(NULL, bManualReset?TRUE:FALSE, bInitialSignal?TRUE:FALSE, NULL);
	}

Event::~Event(){
	::CloseHandle(event_);
	}

void
Event::SetEvent(){
	::SetEvent(event_);
	}

void
Event::ResetEvent(){
	::ResetEvent(event_);
	}

DWORD
Event::WaitForEvent(DWORD dwMilliSec){
	return ::WaitForSingleObject(event_, dwMilliSec);
	}