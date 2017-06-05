#if !defined(PLATFORMDEVICECONTEXT_H_INCLUDED)
#define      PLATFORMDEVICECONTEXT_H_INCLUDED

#include "PlatformCompat.h"
#include "PlatformImage_Win.h"

// Skia
#include "SkData.h"
#include "SkImage.h"
#include "SkStream.h"
#include "SkSurface.h"
#include "SkCanvas.h"

class _DC
{
public:
	_DC();
	virtual ~_DC();
	operator DCDef (){ return context_; };

	BOOL		Attach						(DCDef dc);
	DCDef		Detach						();
	void		DeleteDC					();

	bool		IsNull() { return context_ == NULL; }

	BOOL		CreateCompatibleDC			(DCDef dc);
	bool        CreateMemoryBitmapDC		(int nBPP, UINT width, UINT height);
	BOOL		GetDibImageFromDC			(int nBPP, _Image& bmImage, UINT xSrc, UINT ySrc, UINT nWidth, UINT nHeight, _DC** pDCBitmap = NULL);
	BOOL		GetSelectedDibImageFromDC	(_Image& imageDib);

	BOOL		BitBlt						(int nXDest, int nYDest, int nWidth, int nHeight, DCDef dcSrc, int nXSrc, int nYSrc, DWORD dwRop);	
	static BOOL DrawDC						(DCDef dcDest, int nXDest, int nYDest, int nWidth, int nHeight, _DC *pDCSrc, int nXSrc, int nYSrc, DWORD dwRop);
	int			GetClipBox					(LPRECTDef rect);

	ImageDef	SelectObject				(ImageDef image);
	FONTDef		SelectObject				(FONTDef font);
	void		SetTextColor				(COLORREF crText);
	COLORREF	GetTextColor				();
	void		SetBkColor					(COLORREF crBk);
	COLORREF	GetBkColor					();
	int			SetBkMode					(int nBkMode);
	int			GetBkMode					();
	int			GetDeviceCaps				(int index);

	int			FillSolidRect				(RECTDef* pRect, COLORREF crFillColor);
	void		DrawFocusRect				(RECTDef* pRect);
	void		DrawPath					(RECTDef* pRect, int nLineWidth, COLORREF crPath);

	BOOL		ExtTextOut					(int x, int y, UINT options, LPCRECTDef lprect, const char* pszText, const int * lpDx = NULL);
	BOOL		ExtTextOutW					(int x, int y, UINT options, LPCRECTDef lprect, const wchar_t* lpString,  UINT c, const int * lpDx = NULL);
	BOOL		DrawText					(FONTDef pFont, int x, int y, int flag, _Rect rcClipRect, std::string* pszText, float fStretchCX, float fStretchCY);

	BOOL		GetTextSizeEx				(std::string* pStr, _Size& szText);
	BOOL		GetTextSizeW				(wchar_t* pwszText, int nLen, _Size& szText);
	BOOL		GetTextExtentPoint32		(std::string* pStr, _Size& szText);
	BOOL		GetTextExtentPoint32W		(const wchar_t* lpString, int length, _Size& szText);

protected:
	// Usefull to retreive selected font object from DC.
	static _Font*	GetFontTemp() {
		static _Font _fontTemp;
		if (_fontTemp.IsNull()) {
			LOGFONT lf;
			memset(&lf, 0, sizeof(LOGFONT));
			lf.lfHeight = 10;
			lf.lfWeight = 400;
			lf.lfItalic = 0;
			lf.lfUnderline = 0;
			lf.lfStrikeOut = 0;
			lf.lfCharSet = ANSI_CHARSET;
			lf.lfQuality = PROOF_QUALITY;
			lf.lfPitchAndFamily = DEFAULT_PITCH;
			strcpy_s(lf.lfFaceName, _T("Tahoma"));
			_fontTemp.CreateFontIndirect(&lf);
		}
		return &_fontTemp;
	}

protected:
	SkPaint		_skPaintText;
	SkSurface*	_surface;
	SkCanvas*	_canvas;
	DCDef		context_;
	ImageDef	image_; // When memory bitmap.
};

#endif // !defined(PLATFORMDEVICECONTEXT_H_INCLUDED)

