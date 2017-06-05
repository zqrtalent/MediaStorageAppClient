#if !defined(PLATFORMRECT_H_INCLUDED)
#define      PLATFORMRECT_H_INCLUDED

#include "PlatformCompat.h"

class _Rect : public RECTDef
{
public:
	_Rect(){left = top = right = bottom = 0;};
	_Rect(int l, int t, int r, int b){left = l; top = t; right = r; bottom = b;};
	_Rect(const RECTDef& rc){::CopyRect(this, &rc);};
	_Rect(LPCRECTDef lprc){::CopyRect(this, lprc);};
	virtual ~_Rect(){};

	// retrieves the width
	int Width() const {return (right - left);};
	// returns the height
	int Height() const {return (bottom - top);};
	// returns the size
//	Size Size() const;
	// the geometric center point of the rectangle
//	Point CenterPoint() const;

	// convert between Rect and LPCRECT/LPRect (no need for &)
	operator LPRECTDef() { return this; };
	operator LPCRECTDef() const { return this; };

	// returns TRUE if rectangle has no area
	BOOL IsRectEmpty() const { return ::IsRectEmpty(this); };
	// returns TRUE if rectangle is at (0,0) and has no area
	BOOL IsRectNull() const { return (left == 0 && right == 0 && top == 0 && bottom == 0); };
	// returns TRUE if point is within rectangle
	BOOL PtInRect(POINTDef point) const { return ::PtInRect(this, point); };

	// set rectangle from left, top, right, and bottom
	void SetRect(int x1, int y1, int x2, int y2) { ::SetRect(this, x1, y1, x2, y2); };

	void SetRectEmpty() { ::SetRect(this, 0, 0, 0, 0); };


	void operator=(const RECTDef& srRect) { ::CopyRect(this, &srRect); };
	BOOL operator==(const RECTDef& rect) const { return ::EqualRect(this, &rect); };
	BOOL operator!=(const RECTDef& rect) const { return !::EqualRect(this, &rect); };
	void operator+=(POINTDef point) { ::OffsetRect(this, point.x, point.y); };
	void operator+=(SIZEDef size) { ::OffsetRect(this, size.cx, size.cy); };
	void operator+=(LPCRECTDef lpRect) {InflateRect(lpRect);};
	void operator-=(POINTDef point) { ::OffsetRect(this, -point.x, -point.y); };
	void operator-=(SIZEDef size) { ::OffsetRect(this, -size.cx, -size.cy); };
	void operator-=(LPCRECTDef lpRect) {DeflateRect(lpRect);};
	void operator&=(const RECTDef& rect) { ::IntersectRect(this, this, &rect); };
	void operator|=(const RECTDef& rect) { ::UnionRect(this, this, &rect); };

// Operators returning Rect values
	_Rect operator+(POINTDef pt) const
	{
		_Rect rect(*this);
		::OffsetRect(&rect, pt.x, pt.y);
		return rect;
	};

	_Rect operator-(POINTDef pt) const
	{
		_Rect rect(*this);
		::OffsetRect(&rect, -pt.x, -pt.y);
		return rect;
	};

	_Rect operator+(LPCRECTDef lpRect) const
	{
		_Rect rect(this);
		rect.InflateRect(lpRect);
		return rect;
	};

	_Rect operator+(SIZE size) const
	{
		_Rect rect(*this);
		::OffsetRect(&rect, size.cx, size.cy);
		return rect;
	};

	_Rect operator-(SIZE size) const
	{
		_Rect rect(*this);
		::OffsetRect(&rect, -size.cx, -size.cy);
		return rect;
	};

	_Rect operator-(LPCRECTDef lpRect) const
	{
		_Rect rect(this);
		rect.DeflateRect(lpRect);
		return rect;
	};
	
	_Rect operator&(const RECTDef& rect2) const
	{
		_Rect rect;
		::IntersectRect(&rect, this, &rect2);
		return rect;
	};

	_Rect operator|(const RECTDef& rect2) const
	{
		_Rect rect;
		::UnionRect(&rect, this, &rect2);
		return rect;
	};

	void InflateRect(LPCRECTDef lpRect)
	{
		left -= lpRect->left;
		top -= lpRect->top;
		right += lpRect->right;
		bottom += lpRect->bottom;
	};

	void InflateRect(int l, int t, int r, int b) 
	{
		left -= l;
		top -= t;
		right += r;
		bottom += b;
	}

	void DeflateRect(LPCRECTDef lpRect)
	{
		left += lpRect->left;
		top += lpRect->top;
		right -= lpRect->right;
		bottom -= lpRect->bottom;
	}

	void DeflateRect(int l, int t, int r, int b)
	{
		left += l;
		top += t;
		right -= r;
		bottom -= b;
	}

	void NormalizeRect()
	{
		int nTemp;
		if (left > right){
			nTemp = left;
			left = right;
			right = nTemp;
			}

		if (top > bottom){
			nTemp = top;
			top = bottom;
			bottom = nTemp;
			}
	}

protected:
};

#endif // !defined(PLATFORMRECT_H_INCLUDED)

