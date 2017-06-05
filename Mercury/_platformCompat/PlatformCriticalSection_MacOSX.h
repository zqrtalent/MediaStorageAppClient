#if !defined(PLATFORMCRITICALSECTION_H_INCLUDED)
#define      PLATFORMCRITICALSECTION_H_INCLUDED

#include "PlatformCompat.h"
#include <pthread.h>

class CriticalSection
{
public:
	CriticalSection();
	virtual ~CriticalSection();

	void						Lock	();
	void						Unlock	();

protected:
    pthread_mutex_t section_;
};

#endif // !defined(PLATFORMCRITICALSECTION_H_INCLUDED)

