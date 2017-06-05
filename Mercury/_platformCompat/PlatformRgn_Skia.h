#if !defined(PLATFORMRGN_H_INCLUDED)
#define      PLATFORMRGN_H_INCLUDED

#include "PlatformCompat.h"

class Rgn
{
public:
	Rgn();
	virtual ~Rgn();
	operator RGNDef(){return rgn_;};

	BOOL	Attach					(RGNDef rgn);
	RGNDef	Detach					();
	bool	IsNull					(){ return (rgn_ == NULL); };
	BOOL	DeleteObject			();

	BOOL	CopyRegion				(Rgn* pCopyInto);

protected:
	RGNDef	rgn_;
};

#endif // !defined(PLATFORMRGN_H_INCLUDED)

