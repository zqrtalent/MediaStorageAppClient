#if !defined(PLATFORMEVENT_H_INCLUDED)
#define      PLATFORMEVENT_H_INCLUDED

#include "PlatformCompat.h"

class Event
{
public:
	Event(bool bManualReset = true, bool bInitialSignal = false);
	virtual ~Event();

	operator HANDLE	(){ return event_; };
	void			SetEvent	();
	void			ResetEvent	();
	DWORD			WaitForEvent(DWORD dwMilliSec);

protected:
	HANDLE	event_;
};

#endif // !defined(PLATFORMEVENT_H_INCLUDED)

