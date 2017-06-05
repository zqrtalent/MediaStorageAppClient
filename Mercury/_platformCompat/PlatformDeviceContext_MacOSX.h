#if !defined(PLATFORMDEVICECONTEXT_H_INCLUDED)
#define      PLATFORMDEVICECONTEXT_H_INCLUDED

#include "PlatformCompat.h"
#include "PlatformImage_MacOSX.h"

class _Font;
class _DC
{
public:
	_DC();
	virtual ~_DC();
	//operator NSGraphicsContext* (){ return _context; };
    //operator CGContext* (){ return _context ? [_context CGContext] : NULL; };

	BOOL		Attach						(DCDef dc);
	DCDef		Detach						();
	void		DeleteDC					();
    
    //bool        IsNull                      (){ return (_context == nil ? true : false); };

	BOOL		CreateCompatibleDC			(DCDef dc);
    bool        CreateMemoryBitmapDC        (int nBPP, UINT width, UINT height);
	BOOL		GetDibImageFromDC			(int nBPP, _Image& bmImage, UINT xSrc, UINT ySrc, UINT nWidth, UINT nHeight, _DC** pDCBitmap = NULL);
	BOOL		GetSelectedDibImageFromDC	(_Image& imageDib);
    
    void  GetDibImageInfo(BITMAP& bmInfo){
        memcpy(&bmInfo, _dibImageInfo, sizeof(BITMAP));
    };
    

	BOOL		BitBlt						(int nXDest, int nYDest, int nWidth, int nHeight, _DC *pDCSrc, int nXSrc, int nYSrc, DWORD dwRop);
	int			GetClipBox					(LPRECTDef rect);
    static  BOOL DrawDC                     (DCDef dcDest, int nXDest, int nYDest, int nWidth, int nHeight, _DC *pDCSrc, int nXSrc, int nYSrc, DWORD dwRop);

	ImageDef	SelectObject				(ImageDef image);
	_Font*		SelectObject				(_Font* pFont);
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
	BOOL		ExtTextOutW					(int x, int y, UINT options, LPCRECTDef lprect, const wchar_t* lpString, const int * lpDx = NULL);
	BOOL		DrawText					(FONTDef pFont, int x, int y, int flag, _Rect rcClipRect, _string* pszText, float fStretchCX, float fStretchCY);

	BOOL		GetTextSizeEx				(_string* pStr, _Size& szText);
	BOOL		GetTextSizeW				(wchar_t* pwszText, int nLen, _Size& szText);
	BOOL		GetTextExtentPoint32		(_string* pStr, _Size& szText);
	BOOL		GetTextExtentPoint32W		(const wchar_t* lpString, int length, _Size& szText);

protected:
	static _Image*	GetTempImage(){
        /*
		static Image _imgTemp;
		if( _imgTemp.IsNull() ){
			BITMAPINFOHEADER	bmpInfo32;
			memset(&bmpInfo32, 0, sizeof(BITMAPINFOHEADER));
			bmpInfo32.biBitCount		= 24;
			bmpInfo32.biCompression		= BI_RGB;
			bmpInfo32.biPlanes			= 1;
			bmpInfo32.biHeight			= 1;
			bmpInfo32.biSize			= sizeof(BITMAPINFOHEADER);
			bmpInfo32.biWidth			= 1;
		
			HDC	memDC;
			memDC = ::CreateCompatibleDC(NULL);
			UINT*	lpMap				= NULL;
			HBITMAP hDib				= CreateDIBSection(memDC, (BITMAPINFO*)&bmpInfo32, DIB_RGB_COLORS, (void**)&lpMap, NULL, 0);
			ASSERT( lpMap );
			::DeleteDC(memDC);

			if( hDib )
				_imgTemp.Attach(hDib);
			}	
		return &_imgTemp;*/
        return NULL;
		}

	// Usefull to retreive selected font object from DC.
	static _Font*	GetFontTemp(){/*
		static Font _fontTemp;
		if( _fontTemp.IsNull() ){
			LOGFONT lf;
			memset(&lf, 0, sizeof(LOGFONT));
			lf.lfHeight			= 10;
			lf.lfWeight			= 400;
			lf.lfItalic			= 0;
			lf.lfUnderline		= 0;
			lf.lfStrikeOut		= 0;
			lf.lfCharSet		= ANSI_CHARSET;
			lf.lfQuality		= PROOF_QUALITY;
			lf.lfPitchAndFamily = DEFAULT_PITCH;
			strcpy_s(lf.lfFaceName, _T("Tahoma"));
			_fontTemp.CreateFontIndirect(&lf);
			}
		return &_fontTemp;*/
        return NULL;
		}

protected:
    ImageDef                _imageSelected;

	//NSGraphicsContext*      _context;
    BITMAP*                  _dibImageInfo;
    
    _Font*                   _fontSelected;
    NSMutableDictionary*    _fontAttributes;
    
    //NSColor*                _crFillColor;
    //NSColor*                _crTextColor;
    int                     _bkMode;
};

#endif // !defined(PLATFORMDEVICECONTEXT_H_INCLUDED)

