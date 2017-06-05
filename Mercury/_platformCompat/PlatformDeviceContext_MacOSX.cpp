#include "PlatformDeviceContext_MacOSX.h"
#include "../Utility/StringHelper.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

_DC::_DC() {
    //_context = nil;
    //_dibImage = nil; // Causes unexpected error !!!!!!!
    //_dibImage = nullptr;
    //_imageSelected = nil;
    //_crFillColor = [NSColor blackColor];
    //_crTextColor = [NSColor whiteColor];
    _bkMode = 0;
    
    _dibImageInfo = new BITMAP;
    memset(_dibImageInfo, 0, sizeof(BITMAP));
	}

_DC::~_DC(){
	DeleteDC();
    
    delete _dibImageInfo;
    _dibImageInfo = nullptr;
	}

BOOL
_DC::Attach(DCDef dc){
	//ASSERT(_context == NULL);
	//if( _context )
		return FALSE;
	//_context = dc;
	return TRUE;
	}

DCDef
_DC::Detach(){
	//if( !_context )
	//	return NULL;
    /*
	DCDef ret = context_;
	context_ = NULL;
     */
	return nil;
	}

void
_DC::DeleteDC(){
	//if( _context ){
    //    CGContextRelease([_context CGContext]);
	//	_context = NULL;
	//	}
    
    _imageSelected = NULL;
    memset(&_dibImageInfo, 0, sizeof(BITMAP));
    
    _fontSelected = NULL;
    _fontAttributes = nil;
	}

ImageDef
_DC::SelectObject(ImageDef image){
    return NULL;
	//if( !_context )
	//	return NULL;
//	return (ImageDef)::SelectObject(context_, image);
	}

_Font*
_DC::SelectObject(_Font* pFont){
    /*
	if( !_context )
		return NULL;
    
    _Font* pFontOld = _fontSelected;
    _fontSelected = pFont;
    
    if( _fontAttributes )
        _fontAttributes = nil;
    
    if( pFont ){
        _fontAttributes = [[NSMutableDictionary alloc] initWithDictionary:pFont->GetAttributesDictionary() copyItems:YES];
        // Text color.
        [_fontAttributes setObject:_crTextColor forKey:NSForegroundColorAttributeName];
        // transparent background.
        //[_fontAttributes setObject:nil forKey:NSBackgroundColorAttributeName];
        }
    
     return pFontOld;*/ return nil;
	}

void
_DC::SetTextColor(COLORREF crText){
    /*
	if( !_context ) return;
    
    _crTextColor = [NSColor colorWithSRGBRed:((CGFloat)R_FROM_COLORREF(crText)) / 255.0
                                       green:((CGFloat)G_FROM_COLORREF(crText)) / 255.0
                                        blue:((CGFloat)B_FROM_COLORREF(crText)) / 255.0
                                       alpha:1.0];
    
    if( _fontAttributes ){
        // Text color.
        [_fontAttributes setObject:_crTextColor forKey:NSForegroundColorAttributeName];
        }*/
	}

COLORREF
_DC::GetTextColor(){
    /*
	if( !_context ) return 0L;
    return ARGB((int)(_crTextColor.redComponent * 255.0),
         (int)(_crTextColor.greenComponent * 255.0),
         (int)(_crTextColor.blueComponent * 255.0),
         (int)(_crTextColor.alphaComponent * 255.0));*/
    return 0;
	}

void
_DC::SetBkColor(COLORREF crBk){
    /*
	if( !_context ) return;
    
    //CGContextSetRGBFillColor(_context,
    //                         ((CGFloat)R_FROM_COLORREF(crBk))/255.0,
    //                         ((CGFloat)G_FROM_COLORREF(crBk))/255.0,
    //                         ((CGFloat)B_FROM_COLORREF(crBk))/255.0,
    //                         1.0);
    
    _crFillColor = [NSColor colorWithSRGBRed:((CGFloat)R_FROM_COLORREF(crBk)) / 255.0
                            green:((CGFloat)G_FROM_COLORREF(crBk)) / 255.0
                            blue:((CGFloat)B_FROM_COLORREF(crBk)) / 255.0
                            alpha:1.0];*/
	}

COLORREF
_DC::GetBkColor(){
    /*
	if( !_context )
        return 0;
    
    return ARGB((int)(_crFillColor.redComponent * 255.0),
                (int)(_crFillColor.greenComponent * 255.0),
                (int)(_crFillColor.blueComponent * 255.0),
                (int)(_crFillColor.alphaComponent * 255.0));*/
    return 0;
	}

int
_DC::SetBkMode(int nBkMode){
	//if( !_context ) return 0;
    _bkMode = nBkMode;
//	return ::SetBkMode(_context, nBkMode);
    return 0;
	}

int
_DC::GetBkMode(){
	//if( !_context ) return 0;
//	return ::GetBkMode(_context);
    return _bkMode;
	}

int
_DC::GetDeviceCaps(int index){
	//if( !_context )
        return 0;
//	return ::GetDeviceCaps(_context, index);
    return 0;
	}

int
_DC::FillSolidRect(RECTDef* pRect, COLORREF crFillColor){
	/*if( !_context )
        return 0;
    
    CGContextSetRGBFillColor([_context CGContext],
                             ((CGFloat)R_FROM_COLORREF(crFillColor))/255.0,
                             ((CGFloat)G_FROM_COLORREF(crFillColor))/255.0,
                             ((CGFloat)B_FROM_COLORREF(crFillColor))/255.0,
                             1.0
;
    
    CGContextFillRect([_context CGContext], CGRectMake((CGFloat)pRect->left, (CGFloat)pRect->top,
                                           (CGFloat)(pRect->right - pRect->left), (CGFloat)(pRect->bottom - pRect->top)));
    */
    return 1;
	}

void
_DC::DrawFocusRect(RECTDef* pRect){
	//if( !_context ) return;
//	::DrawFocusRect(_context, pRect);
	}

void
_DC::DrawPath(RECTDef* pRect, int nLineWidth, COLORREF crPath){
	//if( !_context ) return;
	}

BOOL
_DC::CreateCompatibleDC(DCDef dc){
    return NO;
//	ASSERT(!_context);
//	if( _context )
//		return FALSE;
////	context_ = ::CreateCompatibleDC(dc);
//	return (_context != NULL);
	}

bool
_DC::CreateMemoryBitmapDC(int nBPP, UINT width, UINT height){
    /*
    if( _context )
        return false;
    
    size_t bytesPerRow = width * (nBPP/8);
    CGColorSpaceRef colorSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
    
    CGContextRef context = NULL;
    if( nBPP == 32 )
        context = CGBitmapContextCreate(nil, width, height, 8, bytesPerRow, colorSpace, kCGImageAlphaPremultipliedLast);
    else if ( nBPP == 24 )
        context = CGBitmapContextCreate(nil, width, height, 8, bytesPerRow, colorSpace, kCGImageAlphaNone);
    
    CGColorSpaceRelease(colorSpace);
    if( context == NULL ){
        return false;
        }
    
    //_dibImage = [[NSBitmapImageRep alloc] initWithCGImage:refImg];
    //CGImageGetBitmapInfo(refImg);
    //_context = [NSGraphicsContext graphicsContextWithBitmapImageRep:_dibImage];
    //_dibImage = [[NSBitmapImageRep alloc] initWithData:[[NSData alloc] initWithBytesNoCopy:bitmapData length:bytesPerRow*height freeWhenDone:NO]];
    
    _context = [NSGraphicsContext graphicsContextWithCGContext:context flipped:YES];
    //_dibImage = CGBitmapContextCreateImage(context);
    
    memset(_dibImageInfo, 0, sizeof(BITMAP));
    _dibImageInfo->bmBitsPixel = nBPP;
    _dibImageInfo->bmWidthBytes = (int)bytesPerRow;
    _dibImageInfo->bmWidth = (int)width;
    _dibImageInfo->bmHeight = (int)height;
    _dibImageInfo->bmBits = CGBitmapContextGetData(context);*/
    return  true;
    }

BOOL
_DC::GetDibImageFromDC(int nBPP, _Image& bmImage, UINT xSrc, UINT ySrc, UINT nWidth, UINT nHeight, _DC** pDCBitmap /*= NULL*/){
    /*
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

	bmImage.Attach(hDib);*/
	return TRUE;
	}

BOOL
_DC::GetSelectedDibImageFromDC(_Image& imageDib){
    /*
	if( !_context || !_imageSelected ){
		return false;
		}

    if( imageDib.IsNull() ){
        imageDib.Attach(_imageSelected);
        return true;
        }*/
	return false;
	}

BOOL
_DC::BitBlt(int nXDest, int nYDest, int nWidth, int nHeight, _DC *pDCSrc, int nXSrc, int nYSrc, DWORD dwRop){
    /*
	if( !_context || !pDCSrc || pDCSrc->IsNull() )
        return FALSE;
    
    _Image dibImageSrc;
    if( pDCSrc->GetSelectedDibImageFromDC(dibImageSrc) ){
        CGContextDrawImage([_context CGContext], CGRectMake(nXSrc, nYSrc, nWidth, nHeight), dibImageSrc);
        dibImageSrc.Detach();
        return TRUE;
    }*/
    return FALSE;
	}

int
_DC::GetClipBox(LPRECTDef rect){
    /*
	if( !context_ )
		return 0;
	return ::GetClipBox(context_, rect);*/
    return 0;
	}

BOOL
_DC::DrawDC(DCDef dcDest, int nXDest, int nYDest, int nWidth, int nHeight, _DC *pDCSrc, int nXSrc, int nYSrc, DWORD dwRop){
    
//    if( !dcDest || !pDCSrc || pDCSrc->IsNull() )
//        return FALSE;
//    
//    ImageDef image = CGBitmapContextCreateImage(*pDCSrc);
//    CGContextDrawImage(dcDest, CGRectMake(nXDest, nYDest, nWidth, nHeight), image);
//    CGImageRelease(image);
    return TRUE;    
}

BOOL
_DC::ExtTextOut(int x, int y, UINT options, LPCRECTDef lprect, const char* pszText, const int * lpDx){
//	if( !_context || !pszText )
//		return FALSE;
//	// Invalid rect area.
//	if( lprect && (lprect->right <= 0 || lprect->bottom <= 0) )
//		return FALSE;
//    
//    [NSGraphicsContext saveGraphicsState];
//    [NSGraphicsContext setCurrentContext:_context];
//    //CGContextSetTextDrawingMode(memBitmap, kCGTextFillClip);
//    if( _fontSelected )
//        [*_fontSelected setInContext:_context];
//    NSString* drawStr = [NSString stringWithUTF8String:pszText];
//    //NSSize sz1 = [drawStr sizeWithAttributes:_fontAttributes];
//    //CGContextSetTextPosition([_context CGContext], (CGFloat)lprect->left, (CGFloat)lprect->top);
//    [drawStr drawAtPoint:CGPointMake(lprect->left, lprect->top) withAttributes:_fontAttributes];
//    //[drawStr drawInRect:CGRectMake(lprect->left, lprect->top, (lprect->right - lprect->left), (lprect->bottom - lprect->top)) withAttributes:_fontAttributes];
//    [NSGraphicsContext restoreGraphicsState];
    return FALSE;
	}

BOOL
_DC::ExtTextOutW(int x, int y, UINT options, LPCRECTDef lprect, const wchar_t* lpString, const int * lpDx){
//    if( !_context || !lpString )
//		return FALSE;
//	// Invalid rect area.
//	if( lprect && (lprect->right <= 0 || lprect->bottom <= 0) )
//		return FALSE;
//    
//    [NSGraphicsContext saveGraphicsState];
//    [NSGraphicsContext setCurrentContext:_context];
//    //CGContextSetTextDrawingMode([_context CGContext], kCGTextFillClip);
//    if( _fontSelected )
//        [*_fontSelected setInContext:_context];
//    NSString* drawStr = [NSString stringWithCString:(const char*)lpString encoding: NSUTF8StringEncoding];
//    //NSSize sz1 = [drawStr sizeWithAttributes:_fontAttributes];
//    [drawStr drawInRect:CGRectMake(lprect->left, lprect->top, (lprect->right - lprect->left), (lprect->bottom - lprect->top)) withAttributes:_fontAttributes];
//    
//   
//    [NSGraphicsContext restoreGraphicsState];
    return FALSE;
	}

BOOL
_DC::DrawText(FONTDef pFont, int x, int y, int flag, _Rect rcClipRect, _string* pszText, float fStretchCX, float fStretchCY){
	//if( _context || !pFont )
		return FALSE;
/*
	Font font;
	LOGFONT lf;
	font.Attach(pFont);
	font.GetLogFont(&lf);
	font.Detach();

	lf.lfWidth	= (lf.lfWidth * fStretchCX);
	lf.lfHeight = (lf.lfHeight * fStretchCY);

	Font fontNew;
	if( fontNew.CreateFontIndirect(&lf) ){
		FONTDef pFontOld = SelectObject(fontNew);
		ExtTextOut(x, y, flag, rcClipRect, *pszText);
		SelectObject(pFontOld);
		fontNew.DeleteObject();
		}
	else{
		FONTDef pFontOld = SelectObject(pFont);
		ExtTextOut(x, y, flag, rcClipRect, *pszText);
		SelectObject(pFontOld);
		}*/
	return FALSE;
	}

BOOL
_DC::GetTextSizeEx(_string* pStr, _Size& szText){
//    if( !_context || _fontSelected == nullptr || pStr->length() == 0)
//        return FALSE;
//    
//    [NSGraphicsContext saveGraphicsState];
//    [NSGraphicsContext setCurrentContext:_context];
//    if( _fontSelected )
//        [*_fontSelected setInContext:_context];
//    NSString* drawStr = [NSString stringWithCString:(const char*)pStr->c_str() encoding: NSUTF8StringEncoding];
//    NSSize sz1 = [drawStr sizeWithAttributes:_fontAttributes];
//    [NSGraphicsContext restoreGraphicsState];
//    
//    szText.cx = sz1.width;
//    szText.cy = sz1.height;
    return TRUE;
	}

BOOL
_DC::GetTextSizeW(wchar_t* pwszText, int nLen, _Size& szText){
//	if( !_context || _fontSelected == nullptr || nLen <= 0)
//		return FALSE;
//    
//    [NSGraphicsContext saveGraphicsState];
//    [NSGraphicsContext setCurrentContext:_context];
//    if( _fontSelected )
//        [*_fontSelected setInContext:_context];
//    NSString* drawStr = [NSString stringWithCString:(const char*)pwszText encoding: NSUTF8StringEncoding];
//    NSSize sz1 = [drawStr sizeWithAttributes:_fontAttributes];
//    [NSGraphicsContext restoreGraphicsState];
//    
//    szText.cx = sz1.width;
//    szText.cy = sz1.height;
	return TRUE;
	}

BOOL
_DC::GetTextExtentPoint32(_string* pStr, _Size& szText){/*
	if( !context_ )
		return FALSE;
	wchar_t		wszTemp[256];
	int			nLen		= StringHelper::UTF8ToUnicode(pStr, wszTemp, 255);
	return GetTextExtentPoint32W(wszTemp, nLen, szText);*/
    return FALSE;
	}

BOOL
_DC::GetTextExtentPoint32W(const wchar_t* lpString, int length, _Size& szText){
    /*
	if( !context_ )
		return FALSE;
	return ::GetTextExtentPoint32W(context_, lpString, length, szText);*/
    return FALSE;
	}
