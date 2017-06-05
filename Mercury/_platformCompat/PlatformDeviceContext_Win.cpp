#include "PlatformDeviceContext_Win.h"
//#include "../EmbedeedFont/EmbedeedFontMan.h"
#include "../Utility/StringHelper.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

_DC::_DC() : context_(NULL), image_(NULL){
	}

_DC::~_DC(){
	DeleteDC();
	}

BOOL
_DC::Attach(DCDef dc){
	ASSERT(context_ == NULL);
	if( context_ )
		return FALSE;
	context_ = dc;
	return TRUE;
	}

DCDef
_DC::Detach(){
	if( !context_ )
		return NULL;
	DCDef ret = context_;
	context_ = NULL;
	return ret;
	}

void
_DC::DeleteDC(){
	if( context_ ){
		::DeleteDC(context_);
		context_ = NULL;
		}
	if (image_) {
		::DeleteObject(image_);
		image_ = NULL;
		}
	}

ImageDef
_DC::SelectObject(ImageDef image){
	if( !context_ )
		return NULL;
	return (ImageDef)::SelectObject(context_, image);
	}

FONTDef
_DC::SelectObject(FONTDef font){
	if( !context_ )
		return NULL;
	return (FONTDef)::SelectObject(context_, font);
	}

void
_DC::SetTextColor(COLORREF crText){
	if( !context_ ) return;
	::SetTextColor(context_, crText);
	}

COLORREF
_DC::GetTextColor(){
	if( !context_ ) return 0L;
	return ::GetTextColor(context_);
	}

void
_DC::SetBkColor(COLORREF crBk){
	if( !context_ ) return;
	::SetBkColor(context_, crBk);
	}

COLORREF
_DC::GetBkColor(){
	if( !context_ ) return 0;
	return ::GetBkColor(context_);
	}

int
_DC::SetBkMode(int nBkMode){
	if( !context_ ) return 0;
	return ::SetBkMode(context_, nBkMode);
	}

int
_DC::GetBkMode(){
	if( !context_ ) return 0;
	return ::GetBkMode(context_);
	}

int
_DC::GetDeviceCaps(int index){
	if( !context_ ) return 0;
	return ::GetDeviceCaps(context_, index);
	}

int
_DC::FillSolidRect(RECTDef* pRect, COLORREF crFillColor){
	if( !context_ ) return 0;
	HBRUSH	hBrush	= CreateSolidBrush(crFillColor);
	int		nRet	=  ::FillRect(context_, pRect, hBrush);
	::DeleteObject(hBrush);
	return nRet;
	}

void
_DC::DrawFocusRect(RECTDef* pRect){
	if( !context_ ) return;
	::DrawFocusRect(context_, pRect);
	}

void
_DC::DrawPath(RECTDef* pRect, int nLineWidth, COLORREF crPath){
	if( !context_ ) return;
	}

BOOL
_DC::CreateCompatibleDC(DCDef dc){
	ASSERT(!context_);
	if( context_ )
		return FALSE;
	context_ = ::CreateCompatibleDC(dc);
	return (context_ != NULL);
	}

bool
_DC::CreateMemoryBitmapDC(int nBPP, UINT width, UINT height){
	if (context_ != NULL)
		return false;

	BITMAP bmp;
	_Image img;
	if (!img.CreateDIBBitmap(24, RGB(0, 0, 0), width, height, &bmp))
		return false;

	image_ = img.Detach();
	context_ = ::CreateCompatibleDC(NULL);
	::SelectObject(context_, image_);
	return true;
}

BOOL
_DC::GetDibImageFromDC(int nBPP, _Image& bmImage, UINT xSrc, UINT ySrc, UINT nWidth, UINT nHeight, _DC** pDCBitmap /*= NULL*/){
	if( !context_ ){
		ASSERT(FALSE);
		return FALSE;
		}

	HDC memDC = ::CreateCompatibleDC(NULL);

	BITMAPINFOHEADER bmpInfo32;
	memset(&bmpInfo32, 0, sizeof(BITMAPINFOHEADER));
	bmpInfo32.biBitCount    = nBPP;
	bmpInfo32.biCompression = BI_RGB;
	bmpInfo32.biPlanes      = 1;
	bmpInfo32.biHeight      = nHeight;
	bmpInfo32.biSize        = sizeof(BITMAPINFOHEADER);
	bmpInfo32.biWidth       = nWidth;
	
	void*   lpMap = NULL;
	HBITMAP hDib  = ::CreateDIBSection(memDC, (BITMAPINFO*)&bmpInfo32, DIB_RGB_COLORS, &lpMap, NULL, 0);
	if( !hDib ){
		::DeleteDC(memDC);
		return FALSE;
		}

	::SelectObject	(memDC, hDib);
	::BitBlt		(memDC, 0, 0, nWidth, nHeight, context_, xSrc, ySrc, SRCCOPY);

	if( pDCBitmap ){
		*pDCBitmap = new _DC();
		(*pDCBitmap)->Attach(memDC);
		}
	else{
		::DeleteDC(memDC);
		memDC = NULL;
		}

	bmImage.Attach(hDib);
	return TRUE;
	}

BOOL
_DC::GetSelectedDibImageFromDC(_Image& imageDib){
	if( !context_ ){
		ASSERT(FALSE);
		return FALSE;
		}

	_Image*		pTemp		= GetTempImage();
	ImageDef	hDibImage	= (HBITMAP)::SelectObject(context_, *pTemp);
	if( hDibImage != NULL ){
		BITMAP bmInfo;
		if( GetObject(hDibImage, sizeof(BITMAP), &bmInfo) == sizeof(BITMAP) && bmInfo.bmBits != NULL ){
			::SelectObject(context_, hDibImage);
			imageDib.Attach(hDibImage);
			return true;
			}
		else{
			::SelectObject(context_, hDibImage);
			hDibImage = NULL;
			}
		}
	return false;
	}

BOOL
_DC::BitBlt(int nXDest, int nYDest, int nWidth, int nHeight, DCDef dcSrc, int nXSrc, int nYSrc, DWORD dwRop){
	if( !context_ ) return FALSE;
	return ::BitBlt(context_, nXDest, nYDest, nWidth, nHeight, dcSrc, nXSrc, nYSrc, dwRop);
	}

BOOL
_DC::DrawDC(DCDef dcDest, int nXDest, int nYDest, int nWidth, int nHeight, _DC *pDCSrc, int nXSrc, int nYSrc, DWORD dwRop) {
	if (!dcDest || pDCSrc == NULL || pDCSrc->IsNull())
		return FALSE;
	return ::BitBlt(dcDest, nXDest, nYDest, nWidth, nHeight, *pDCSrc, nXSrc, nYSrc, dwRop);
	}

int
_DC::GetClipBox(LPRECTDef rect){
	if( !context_ )
		return 0;
	return ::GetClipBox(context_, rect);
	}

BOOL
_DC::ExtTextOut(int x, int y, UINT options, LPCRECTDef lprect, const char* pszText, const int * lpDx){
	if( !context_ || !pszText)
		return FALSE;

	// Invalid rect area.
	if( lprect && (lprect->right <= 0 || lprect->bottom <= 0) )
		return FALSE;

	wchar_t		wszTemp[256];
	int			nLen		= StringHelper::UTF8ToUnicode(pszText, wszTemp, 255);
	BOOL		bRet		= FALSE;

	LOGFONT lf;
	FONTDef pRealFont = (FONTDef)SelectObject(*GetFontTemp());
	_Font fontReal;
	fontReal.Attach	(pRealFont);
	fontReal.GetLogFont(&lf);
	SelectObject	(pRealFont);
	fontReal.Detach();

	bRet = (TRUE == ::ExtTextOutW(context_, x, y, options, lprect, wszTemp, nLen, NULL));
	/*
	EmbedeedFontMan* pEmbedeedFontMan = EmbedeedFontMan::GetInstance();
	int nFontId = pEmbedeedFontMan->GetEmbedeedFontId(lf.lfFaceName);
	if( nFontId == -1 )
		bRet = (TRUE == ::ExtTextOutW(context_, x, y, options, lprect, wszTemp, nLen, NULL));
	else
		bRet = pEmbedeedFontMan->DrawTextByEmbedeedFont(nFontId, this, wszTemp, nLen, x, y, lprect);*/
	return	bRet;
	}

BOOL
_DC::ExtTextOutW(int x, int y, UINT options, LPCRECTDef lprect, const wchar_t* lpString,  UINT c, const int * lpDx){
	if( !context_ || !c ) 
		return FALSE;

	// Invalid rect area.
	if( lprect && (lprect->right <= 0 || lprect->bottom <= 0) )
		return FALSE;

	BOOL		bRet	= FALSE;
	LOGFONT lf;
	FONTDef pRealFont	= (FONTDef)SelectObject(*GetFontTemp());
	_Font fontReal;
	fontReal.Attach	(pRealFont);
	fontReal.GetLogFont(&lf);
	SelectObject	(pRealFont);
	fontReal.Detach();

	bRet = (TRUE == ::ExtTextOutW(context_, x, y, options, lprect, lpString, c, NULL));
	/*
	EmbedeedFontMan* pEmbedeedFontMan = EmbedeedFontMan::GetInstance();
	int nFontId = pEmbedeedFontMan->GetEmbedeedFontId(lf.lfFaceName);
	if( nFontId == -1 )
		bRet = (TRUE == ::ExtTextOutW(context_, x, y, options, lprect, lpString, c, NULL));
	else
		bRet = pEmbedeedFontMan->DrawTextByEmbedeedFont(nFontId, this, lpString, c, x, y, lprect);
		*/
	return	bRet;
	}

BOOL
_DC::DrawText(FONTDef pFont, int x, int y, int flag, _Rect rcClipRect, std::string* pszText, float fStretchCX, float fStretchCY){
	if( !context_ || !pFont ) 
		return FALSE;

	_Font font;
	LOGFONT lf;
	font.Attach(pFont);
	font.GetLogFont(&lf);
	font.Detach();

	lf.lfWidth	= (lf.lfWidth * fStretchCX);
	lf.lfHeight = (lf.lfHeight * fStretchCY);

	_Font fontNew;
	if( fontNew.CreateFontIndirect(&lf) ){
		FONTDef pFontOld = SelectObject(fontNew);
		ExtTextOut(x, y, flag, rcClipRect, pszText->c_str());
		SelectObject(pFontOld);
		fontNew.DeleteObject();
		}
	else{
		FONTDef pFontOld = SelectObject(pFont);
		ExtTextOut(x, y, flag, rcClipRect, pszText->c_str());
		SelectObject(pFontOld);
		}
	return FALSE;
	}

BOOL
_DC::GetTextSizeEx(std::string* pStr, _Size& szText){
	if( !context_ )
		return FALSE;

	LOGFONT lf;
	FONTDef pRealFont = SelectObject(*GetFontTemp());
	_Font realFont;
	realFont.Attach(pRealFont);
	realFont.GetLogFont(&lf);
	SelectObject(pRealFont);
	realFont.Detach();

	wchar_t		wszTemp[256];
	int			nLen = StringHelper::UTF8ToUnicode(pStr->c_str(), wszTemp, 255);
	BOOL		bRet = FALSE;

	bRet = ::GetTextExtentPoint32W(context_, wszTemp, nLen, &szText);
	/*
	EmbedeedFontMan* pEmbedeedFontMan = EmbedeedFontMan::GetInstance();
	int nFontId = pEmbedeedFontMan->GetEmbedeedFontId(lf.lfFaceName);
	if( nFontId == -1 )
		bRet = ::GetTextExtentPoint32W(context_, wszTemp, nLen, &szText);
	else
		bRet = pEmbedeedFontMan->GetTextSize(this, wszTemp, nLen, nFontId, szText);
		*/
	return bRet;
	}

BOOL
_DC::GetTextSizeW(wchar_t* pwszText, int nLen, _Size& szText){
	if( !context_ )
		return FALSE;

	LOGFONT lf;
	FONTDef pRealFont = SelectObject(*GetFontTemp());
	_Font realFont;
	realFont.Attach(pRealFont);
	realFont.GetLogFont(&lf);
	SelectObject(pRealFont);
	realFont.Detach();
	::GetTextExtentPoint32W(context_, pwszText, nLen, &szText);
	/*
	EmbedeedFontMan* pEmbedeedFontMan = EmbedeedFontMan::GetInstance();
	int nFontId = pEmbedeedFontMan->GetEmbedeedFontId(lf.lfFaceName);
	if( nFontId == -1 )
		::GetTextExtentPoint32W(context_, pwszText, nLen, &szText);
	else
		pEmbedeedFontMan->GetTextSize(this, pwszText, nLen, nFontId, szText);*/
	return true;
	}

BOOL
_DC::GetTextExtentPoint32(std::string* pStr, _Size& szText){
	if( !context_ )
		return FALSE;
	wchar_t		wszTemp[256];
	int			nLen		= StringHelper::UTF8ToUnicode(pStr->c_str(), wszTemp, 255);
	return GetTextExtentPoint32W(wszTemp, nLen, szText);
	}

BOOL
_DC::GetTextExtentPoint32W(const wchar_t* lpString, int length, _Size& szText){
	if( !context_ )
		return FALSE;
	return ::GetTextExtentPoint32W(context_, lpString, length, szText);
	}