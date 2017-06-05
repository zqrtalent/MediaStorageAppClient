#if !defined(PLATFORMCRITICALSECTION_H_INCLUDED)
#define      PLATFORMCRITICALSECTION_H_INCLUDED

#include "PlatformCompat.h"

class CriticalSection
{
public:
	CriticalSection();
	virtual ~CriticalSection();

	operator CRITICAL_SECTION	(){ return section_; };
	void						Lock	();
	void						Unlock	();

protected:
	CRITICAL_SECTION	section_;
};

#endif // !defined(PLATFORMCRITICALSECTION_H_INCLUDED)

