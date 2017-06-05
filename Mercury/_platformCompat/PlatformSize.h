#if !defined(PLATFORMSIZE_H_INCLUDED)
#define      PLATFORMSIZE_H_INCLUDED

#include "PlatformCompat.h"

class _Size : public SIZEDef
{
public:
	_Size(){cx = cy = 0;};
	_Size(int initCX, int initCY){cx = initCX; cy = initCY;};
	_Size(const SIZEDef initSize){cx = initSize.cx; cy = initSize.cy;};
	virtual ~_Size(){};

	operator LPSIZEDef() { return this; };
	operator LPCSIZEDef() const { return this; };

	void SetSize(int CX, int CY) { cx = CX; cy = CY;};

	void operator=(const SIZEDef& srSize) { cx = srSize.cx; cy = srSize.cy; };
	BOOL operator==(SIZEDef size) const { return (cx == size.cx && cy == size.cy); };
	BOOL operator!=(SIZEDef size) const { return (cx != size.cx || cy != size.cy); };
	void operator+=(SIZEDef size) { cx += size.cx; cy += size.cy; };
	void operator-=(SIZEDef size) { cx -= size.cx; cy -= size.cy; };
	_Size operator+(SIZEDef size) {return _Size(cx + size.cx, cy + size.cy);};
	_Size operator-(SIZEDef size) {return _Size(cx - size.cx, cy - size.cy);};
};

#endif // !defined(PLATFORMSIZE_H_INCLUDED)

