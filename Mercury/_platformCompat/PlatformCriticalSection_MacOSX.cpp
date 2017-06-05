#include "PlatformCriticalSection_MacOSX.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CriticalSection::CriticalSection() {
    //pthread_mutex_init(&section_, NULL);
	}

CriticalSection::~CriticalSection(){
	//pthread_mutex_destroy(&section_);
	}

void
CriticalSection::Lock(){
	//pthread_mutex_lock(&section_);
	}

void
CriticalSection::Unlock(){
	//pthread_mutex_unlock(&section_);
	}