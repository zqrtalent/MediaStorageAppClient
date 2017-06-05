#if !defined(PLATFORMEVENT_H_INCLUDED)
#define      PLATFORMEVENT_H_INCLUDED

#include "PlatformCompat.h"

class Event
{
public:
	Event(bool bManualReset = true, bool bInitialSignal = false);
	virtual ~Event();

	operator HANDLE	(){ return 0; };
	void			SetEvent	();
	void			ResetEvent	();
	DWORD			WaitForEvent(DWORD dwMilliSec);

protected:
};

#endif // !defined(PLATFORMEVENT_H_INCLUDED)

