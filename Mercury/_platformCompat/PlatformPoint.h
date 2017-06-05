#if !defined(PLATFORMPOINT_H_INCLUDED)
#define      PLATFORMPOINT_H_INCLUDED

#include "PlatformCompat.h"

class _Point : public POINTDef
{
public:
	_Point(){x = y = 0;};
	_Point(int initX, int initY){x = initX; y = initY;};
	_Point(const POINTDef initPoint){x = initPoint.x; y = initPoint.y;};
	virtual ~_Point(){};

	operator LPPOINTDef() { return this; };
	operator LPCPOINTDef() const { return this; };

	void SetPoint(int X, int Y) { x = X; y = Y;};

	void operator=(const POINTDef& srPoint) { x = srPoint.x; y = srPoint.y; };
	BOOL operator==(POINTDef point) const { return (x == point.x && y == point.y); };
	BOOL operator!=(POINTDef point) const { return (x != point.x || y != point.y); };
	void operator+=(POINTDef point) { x += point.x; y += point.y; };
	void operator-=(POINTDef point) { x -= point.x; y -= point.y; };
	_Point operator+(POINTDef point) {return _Point(x + point.x, y + point.y);};
	_Point operator-(POINTDef point) {return _Point(x - point.x, y - point.y);};
};

#endif // !defined(PLATFORMPOINT_H_INCLUDED)

