#if !defined(PLATFORMRECT_H_INCLUDED)
#define      PLATFORMRECT_H_INCLUDED

#include "PlatformCompat.h"

class _Rect : public RECTDef
{
public:
	_Rect(){left = top = right = bottom = 0;};
	_Rect(int l, int t, int r, int b){left = l; top = t; right = r; bottom = b;};
	_Rect(const RECTDef& rc){ left = rc.left; right = rc.right; top = rc.top; bottom = rc.bottom; };
	_Rect(LPCRECTDef lprc){left = lprc->left; right = lprc->right; top = lprc->top; bottom = lprc->bottom;};
	virtual ~_Rect(){};

	// retrieves the width
	int Width() const {return (right - left);};
	// returns the height
	int Height() const {return (bottom - top);};
	// returns the size
//	_Size Size() const;
	// the geometric center point of the rectangle
//	_Point CenterPoint() const;

	// convert between _Rect and LPCRECT/LPRect (no need for &)
	operator LPRECTDef() { return this; };
	operator LPCRECTDef() const { return this; };

	// returns TRUE if rectangle has no area
	BOOL IsRectEmpty() const { return (left == right && top == bottom); };
	// returns TRUE if rectangle is at (0,0) and has no area
	BOOL IsRectNull() const { return (left == 0 && right == 0 && top == 0 && bottom == 0); };
	// returns TRUE if point is within rectangle
	BOOL PtInRect(POINTDef point) const
    {
        if( point.x >= left && point.x <= right )
        {
            if( point.y >= top && point.y <= bottom )
                return TRUE;
        }
        return FALSE;
    };

	// set rectangle from left, top, right, and bottom
	void SetRect(int x1, int y1, int x2, int y2) { left = x1; right = x2; top = y1; bottom = y2; };

	void SetRectEmpty() { left = 0; right = 0; top = 0; bottom = 0; };


	void operator=(const RECTDef& srRect) { left = srRect.left; right = srRect.right; top = srRect.top; bottom = srRect.bottom; };
	BOOL operator==(const RECTDef& rect) const
    {
        return( left == rect.left && top == rect.top && right == rect.right && bottom == rect.bottom);
    };
    
	BOOL operator!=(const RECTDef& rect) const
    {
        return !(left == rect.left && top == rect.top && right == rect.right && bottom == rect.bottom);
    };
    
	void operator+=(POINTDef point)
    {
        left += point.x;
        right += point.x;
        top += point.y;
        bottom += point.y;
    };
    
	void operator+=(SIZEDef size)
    {
        left += size.cx;
        right += size.cx;
        top += size.cy;
        bottom += size.cy;
    };
    
	void operator+=(LPCRECTDef lpRect) {InflateRect(lpRect);};
	void operator-=(POINTDef point)
    {
        left -= point.x;
        right -= point.x;
        top -= point.y;
        bottom -= point.y;
    };
    
	void operator-=(SIZEDef size)
    {
        left -= size.cx;
        right -= size.cx;
        top -= size.cy;
        bottom -= size.cy;
    };
    
	void operator-=(LPCRECTDef lpRect) {DeflateRect(lpRect);};
	void operator&=(const RECTDef& rect)
    {
        IntersectRect(this, this, &rect);
    };
    
	void operator|=(const RECTDef& rect)
    {
        UnionRect(this, this, &rect);
    };

// Operators returning _Rect values
	_Rect operator+(POINTDef pt) const
	{
		_Rect rect(*this);
		rect.OffsetRect(pt.x, pt.y);
		return rect;
	};

	_Rect operator-(POINTDef pt) const
	{
		_Rect rect(*this);
		rect.OffsetRect(-pt.x, -pt.y);
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
		rect.OffsetRect(size.cx, size.cy);
		return rect;
	};

	_Rect operator-(SIZE size) const
	{
		_Rect rect(*this);
		rect.OffsetRect(-size.cx, -size.cy);
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
		IntersectRect(&rect, this, &rect2);
		return rect;
	};

	_Rect operator|(const RECTDef& rect2) const
	{
		_Rect rect;
		UnionRect(&rect, this, &rect2);
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
    
    void OffsetRect(int x, int y)
	{
		left += x;
        right += x;
        top += y;
        bottom += y;
	}

protected:
    static void IntersectRect(RECTDef* pDest, const RECTDef* pSrc1, const RECTDef* pSrc2)
	{
//        
//       CGRect interSectRect =  CGRectIntersection(CGRectMake((pSrc1->left), (pSrc1->top), (pSrc1->right - pSrc1->left), (pSrc1->bottom - pSrc1->top)),
//                           CGRectMake((pSrc2->left), (pSrc2->top), (pSrc2->right - pSrc2->left), (pSrc2->bottom - pSrc2->top)));
//        
//        pDest->left = interSectRect.origin.x;
//        pDest->top = interSectRect.origin.y;
//        pDest->right = pDest->left + interSectRect.size.width;
//        pDest->bottom = pDest->top + interSectRect.size.height;
	}
    
    static void UnionRect(RECTDef* pDest, const RECTDef* pSrc1, const RECTDef* pSrc2)
	{
        CGRect unionRect =  CGRectUnion(CGRectMake((pSrc1->left), (pSrc1->top), (pSrc1->right - pSrc1->left), (pSrc1->bottom - pSrc1->top)),
                                                   CGRectMake((pSrc2->left), (pSrc2->top), (pSrc2->right - pSrc2->left), (pSrc2->bottom - pSrc2->top)));
        
        pDest->left = unionRect.origin.x;
        pDest->top = unionRect.origin.y;
        pDest->right = pDest->left + unionRect.size.width;
        pDest->bottom = pDest->top + unionRect.size.height;
	}
};

#endif // !defined(PLATFORMRECT_H_INCLUDED)

