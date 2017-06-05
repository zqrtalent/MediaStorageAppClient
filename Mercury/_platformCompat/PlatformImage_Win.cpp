#include "PlatformImage_Win.h"
#include "PlatformDeviceContext_Win.h"
#include "PlatformLongBinary.h"
#include "../Path/Path.h"
#include "../Utility/StringHelper.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

_Image::_Image() : image_(NULL) {
	memset(&bmpInfo_, 0, sizeof(BITMAP));
	}

_Image::_Image(ImageDef img) {
	memset(&bmpInfo_, 0, sizeof(BITMAP));
	Attach(img);
	}

_Image::~_Image(){
	Destroy();
	}

bool
_Image::Attach(ImageDef image){
	ASSERT(IsNull());
	if( !IsNull() || !image )
		return false;

	BITMAP bmInfo;
	if( ::GetObject(image, sizeof(BITMAP), &bmInfo) != sizeof(BITMAP) || !bmInfo.bmBits )
		return false;

	memcpy(&bmpInfo_, &bmInfo, sizeof(BITMAP));
	image_ = image;
	return true;
	}

ImageDef
_Image::Detach(){
	//ASSERT( !IsNull() );
	if( IsNull() )
		return NULL;
	ImageDef ret = image_;
	memset(&bmpInfo_, 0, sizeof(BITMAP));
	image_ = NULL;
	return ret;
	}

void
_Image::Destroy(){
	if( !IsNull() ){
		::DeleteObject(image_);
		memset(&bmpInfo_, 0, sizeof(BITMAP));
		image_ = NULL;
		}
	}

BOOL
_Image::LoadImage32bpp(const std::string sImageFile){
	if( !IsNull() ){
		ASSERT(FALSE);
		return FALSE;
		}
	
	// Initialize gdi plus.
	InitGDIPlus(false);

	ImageDef imgLoaded = NULL;
	wchar_t wszFileName[128];
	StringHelper::UTF8ToUnicode(sImageFile.c_str(), wszFileName, 127);

	try{
		Gdiplus::Bitmap bmSrc(wszFileName);
		if( bmSrc.GetLastStatus() != Gdiplus::Ok )
			return FALSE;
		/*
		Gdiplus::Color crBkgnd(0, 0, 0);
		bmSrc.GetHBITMAP(crBkgnd, &imgLoaded);*/

		Gdiplus::PixelFormat eSrcPixelFormat = bmSrc.GetPixelFormat();
		UINT nBPP = 32;
		Gdiplus::PixelFormat eDestPixelFormat = PixelFormat32bppRGB;
		if( eSrcPixelFormat&PixelFormatGDI ){
			nBPP = Gdiplus::GetPixelFormatSize( eSrcPixelFormat );
			eDestPixelFormat = eSrcPixelFormat;
			}

		if( Gdiplus::IsAlphaPixelFormat( eSrcPixelFormat ) ){
			nBPP = 32;
			eDestPixelFormat = PixelFormat32bppARGB;
			}

		_Image imgDib;
		if( !imgDib.CreateDIBBitmap(nBPP, RGB(0, 0, 0), bmSrc.GetWidth(), bmSrc.GetHeight(), NULL, NULL) )
			return FALSE;

		if( eDestPixelFormat == eSrcPixelFormat ){
			// The pixel formats are identical, so just memcpy the rows.
			Gdiplus::BitmapData data;
			Gdiplus::Rect rect( 0, 0, imgDib.GetWidth(), imgDib.GetHeight() );
			if( bmSrc.LockBits( &rect, Gdiplus::ImageLockModeRead, eSrcPixelFormat, &data )!=Gdiplus::Ok ){
				return FALSE;
				}

#define AlignUp(N, n, nAlign) N( (n+(nAlign-1))&~(N( nAlign )-1) )

			size_t	nBytesPerRow	= AlignUp(size_t, nBPP*imgDib.GetWidth(), 8)/8;
			BYTE*	pbDestRow		= (BYTE*)(imgDib.GetBitsUpsidedown());
			BYTE*	pbSrcRow		= static_cast< BYTE* >(data.Scan0);
			int		nPitchDest		= -1*imgDib.GetWidthBytes();
			int		nDestHeight		= imgDib.GetHeight();

			for( int y = 0; y < nDestHeight; y++ ){
				memcpy(pbDestRow, pbSrcRow, nBytesPerRow);
				pbDestRow += nPitchDest;
				pbSrcRow += data.Stride;
				}

			bmSrc.UnlockBits( &data );
			}
		else{
			/* Not working !!!
			// Let GDI+ work its magic
			Gdiplus::Bitmap		bmDest( (INT)imgDib.GetWidth(), (INT)imgDib.GetHeight(), imgDib.GetWidthBytes(), eDestPixelFormat, (BYTE*)imgDib.GetBits() );
			Gdiplus::Graphics	gDest( &bmDest );
			gDest.DrawImage	(&bmSrc, 0, 0);
			*/
			}
		imgLoaded = imgDib.Detach();
		}
	catch(...){
		return FALSE;
		}

	if( !imgLoaded )
		return FALSE;

	Attach(imgLoaded);
	return TRUE;
	}

BOOL
_Image::LoadFromBinary(LongBinary* pBinary){
	if( !IsNull() || !pBinary || !pBinary->GetBinarySize() )
		return FALSE;

	// Initialize gdi plus.
	InitGDIPlus(false);

	// 0xFF, 0xD8 - JPG image header.
	LPSTREAM lpStream = NULL;
	HRESULT hr = ::CreateStreamOnHGlobal(*pBinary, FALSE, &lpStream);
	if( hr != S_OK )
		return FALSE;

	ImageDef imgLoaded = NULL;
	try{
		Gdiplus::Bitmap bmSrc(lpStream);
		if( bmSrc.GetLastStatus() != Gdiplus::Ok )
			return FALSE;
		Gdiplus::Color crBkgnd(0, 0, 0);
		bmSrc.GetHBITMAP(crBkgnd, &imgLoaded);
		}
	catch(...){
		lpStream->Release();
		return FALSE;
		}

	lpStream->Release();
	if( !imgLoaded )
		return FALSE;

	Attach(imgLoaded);
	return TRUE;
	}


_Image*
_Image::LoadImage(const std::string sImageFile, int nWidth /*= -1*/, int nHeight /*= -1*/, bool bConvert32bpp /*= true*/){
	std::string sImageFileFull	= sImageFile;
	_Image*		pImage			= NULL;

#ifdef LOAD_FILES_FROM_DLL
	CString sTemp = sImageFile;
	sTemp.Replace('\\', '@');
	sTemp.MakeUpper();

	CString sResName;
	sResName.Format(_T("~%s~"), sTemp);
	sResName.Replace('~', '\x22');
	HMODULE hResModule = GetModuleHandle(RESOURCE_MODULE);
	pImage = LoadPngFromResource(hResModule, sResName.GetBuffer());
#endif

	if( !pImage ){
		pImage = new _Image();
		Path::BuildFullPath(sImageFileFull);

		LPCSTR	lpszImageFile	= sImageFileFull.c_str();
		int		nLen			= sImageFileFull.length();
		LPCTSTR lpszExt			= &lpszImageFile[nLen - 4];

		if( !pImage->LoadImage32bpp(sImageFileFull.c_str()) ){
			delete pImage;
			return NULL;
			}
		}

	if( nWidth > 0 && nHeight > 0 ){
		_Image bmStretch;
		if( bmStretch.CreateDIBBitmap(pImage->GetBPP(), (COLORREF)0L, nWidth, nHeight, NULL) ){
			HDC memDC = CreateCompatibleDC(NULL);
			::SelectObject(memDC, bmStretch);

			ImageDef stretchImageDef = bmStretch;
			if( pImage->ScaleBitmap_Bilinier(nWidth, nHeight, _Rect(0, 0, 0, 0), stretchImageDef, (pImage->GetBPP() == 32), 255, false) ){
				pImage->Destroy();
				pImage->Attach((HBITMAP)bmStretch.Detach());
				}
			else
				bmStretch.Destroy();
			::DeleteDC(memDC);
			}
		}

	// Convert into 32bpp. ################
	if( pImage->GetBPP() != 32 && bConvert32bpp ){
		pImage->ConvertToBitmap32bpp();

		// Make image opaque
		BITMAP bmImage;
		::GetObject(*pImage, sizeof(BITMAP), &bmImage);
		RGBQUAD* p = (RGBQUAD*)bmImage.bmBits;

		if( p ){
			int nLoop = 0, nCt = bmImage.bmWidth*bmImage.bmHeight;
			while( nLoop < nCt ){
				p->rgbReserved = 255;
				p		++;
				nLoop	++;
				}
			}
		}
	// #######################################
	return pImage;
	}

BOOL
_Image::Save(const std::string sImageFile){
	return FALSE;
	}

BOOL
_Image::ConvertToBitmap32bpp(){
	if( IsNull() ){
		ASSERT(FALSE);
		return FALSE;
		}

	BOOL	bRet		= FALSE;
	HBITMAP hBmImage	= (HBITMAP)Detach();
	HDC		memDC, srcDC;

	memDC = ::CreateCompatibleDC(NULL);
	srcDC = ::CreateCompatibleDC(memDC);
	SelectObject(srcDC, hBmImage);
	
	BITMAP bmp;
	::GetObject (hBmImage, sizeof(BITMAP), &bmp);
	if( bmp.bmBitsPixel == 32 && bmp.bmBits ){
		DeleteDC(memDC);
		DeleteDC(srcDC);
		return TRUE;
		}

	BITMAPINFOHEADER bmpInfo32;
	memset(&bmpInfo32,0,sizeof(BITMAPINFOHEADER));
	bmpInfo32.biBitCount    = 32;
	bmpInfo32.biCompression = BI_RGB;
	bmpInfo32.biPlanes      = 1;
	bmpInfo32.biHeight      = bmp.bmHeight;
	bmpInfo32.biSize        = sizeof(BITMAPINFOHEADER);
	bmpInfo32.biWidth       = bmp.bmWidth;
	
	void*   lpMap = NULL;
	HBITMAP hDib  = CreateDIBSection(memDC, (BITMAPINFO*)&bmpInfo32, DIB_RGB_COLORS, &lpMap, NULL, 0);
	if( !hDib ){
		DeleteDC(memDC);
		DeleteDC(srcDC);
		Attach(hBmImage);
		return FALSE;
		}

	SelectObject(memDC, hDib);
	BitBlt		(memDC, 0, 0, bmp.bmWidth, bmp.bmHeight, srcDC, 0, 0, SRCCOPY);

	::DeleteObject	(hBmImage);
	::DeleteDC		(memDC);
	::DeleteDC		(srcDC);

	Attach(hDib);
	return 1;
	}

BOOL
_Image::ConvertToBitmap24bpp(_Image* pCopyInto /*= NULL*/){
	if( IsNull() ){
		ASSERT(FALSE);
		return FALSE;
		}

	BITMAP bmp;
	::GetObject(image_, sizeof(BITMAP), &bmp);
	if( bmp.bmBitsPixel == 24 && bmp.bmBits )
		return FALSE;

	ImageDef	hBmImage	= (HBITMAP)Detach();
	HDC			memDC		= ::CreateCompatibleDC(NULL);

	BITMAPINFOHEADER bmpInfo32;
	memset(&bmpInfo32,0,sizeof(BITMAPINFOHEADER));
	bmpInfo32.biBitCount    = 24;
	bmpInfo32.biCompression = BI_RGB;
	bmpInfo32.biPlanes      = 1;
	bmpInfo32.biHeight      = bmp.bmHeight;
	bmpInfo32.biSize        = sizeof(BITMAPINFOHEADER);
	bmpInfo32.biWidth       = bmp.bmWidth;
	
	void*   lpMap = NULL;
	HBITMAP hDib  = ::CreateDIBSection(memDC, (BITMAPINFO*)&bmpInfo32, DIB_RGB_COLORS, &lpMap, NULL, 0);
	if( !hDib ){
		DeleteDC(memDC);
		Attach(hBmImage);
		return FALSE;
		}

	BITMAP bmDest;
	GetObject(hDib, sizeof(BITMAP), &bmDest);

	BYTE* pDest = (BYTE*)lpMap;
	BYTE* pSrc = (BYTE*)bmp.bmBits;
	for(int y=0; y<bmp.bmHeight; y++){
		pSrc	= &((BYTE*)bmp.bmBits)[y*bmp.bmWidthBytes];
		pDest	= &((BYTE*)lpMap)[y*bmDest.bmWidthBytes];

		for(int x=0; x<bmp.bmWidth; x++){
			pDest[0] = pSrc[0]; // Blue
			pDest[1] = pSrc[1]; // Green
			pDest[2] = pSrc[2]; // Blue
			pSrc	+= 4;
			pDest	+= 3;
			}
		}

	if( !pCopyInto ){
		::DeleteObject	(hBmImage);
		Attach			(hDib);
		}
	else{
		Attach(hBmImage);
		if( !pCopyInto->IsNull() )
			pCopyInto->Destroy();
		pCopyInto->Attach(hDib);
		}
	::DeleteDC(memDC);
	return TRUE;
	}

BOOL
_Image::CreateDIBBitmap(int nBPP, UINT* pColorTable, UINT nWidth, UINT nHeight, BITMAP* pBmp /*= NULL*/){
	if( !IsNull() ){
		ASSERT(FALSE);
		return FALSE;
		}

	BITMAPINFOHEADER	bmpInfo32;
	memset(&bmpInfo32, 0, sizeof(BITMAPINFOHEADER));
	bmpInfo32.biBitCount		= nBPP;
	bmpInfo32.biCompression		= BI_RGB;
	bmpInfo32.biPlanes			= 1;
	bmpInfo32.biHeight			= nHeight;
	bmpInfo32.biSize			= sizeof(BITMAPINFOHEADER);
	bmpInfo32.biWidth			= nWidth;

	HDC					memDC	= ::CreateCompatibleDC(NULL);
	UINT*				lpMap;
	HBITMAP				hDib	= CreateDIBSection(memDC, (BITMAPINFO*)&bmpInfo32, DIB_RGB_COLORS, 
													(void**)&lpMap, NULL, 0);
	ASSERT( lpMap );

	UINT index = 0;
	while( index < nWidth*nHeight ){
		lpMap[index] = pColorTable[index];
		index ++;
		}

	if( pBmp )
		::GetObject(hDib, sizeof(BITMAP), pBmp);

	::DeleteDC(memDC);
	Attach(hDib);
	return TRUE;
	}

BOOL
_Image::CreateDIBBitmap(int nBPP, COLORREF crFill, UINT nWidth, UINT nHeight, BITMAP* pBmp /*= NULL*/, _DC* pDCCompatible /*= NULL*/){
	if( !IsNull() ){
		ASSERT(FALSE);
		return FALSE;
		}

	BITMAPINFOHEADER	bmpInfo32;
	memset(&bmpInfo32, 0, sizeof(BITMAPINFOHEADER));
	bmpInfo32.biBitCount		= nBPP;
	bmpInfo32.biCompression		= BI_RGB;
	bmpInfo32.biPlanes			= 1;
	bmpInfo32.biHeight			= nHeight;
	bmpInfo32.biSize			= sizeof(BITMAPINFOHEADER);
	bmpInfo32.biWidth			= nWidth;

	UINT*	lpMap	= NULL;
	HBITMAP	hDib	= NULL;
	
	if( !pDCCompatible ){
		HDC	memDC	= ::CreateCompatibleDC(NULL);
		hDib		= ::CreateDIBSection(memDC, (BITMAPINFO*)&bmpInfo32, DIB_RGB_COLORS, (void**)&lpMap, NULL, 0);
		ASSERT( lpMap );
		::DeleteDC(memDC);
		}
	else{
		hDib	= ::CreateDIBSection(*pDCCompatible, (BITMAPINFO*)&bmpInfo32, DIB_RGB_COLORS, (void**)&lpMap, NULL, 0);
		ASSERT( lpMap );
		}

	if( crFill != 0 ){
		int			nLoop	= 0, nIndex = 0, nCount		= nWidth*nHeight;
		nBPP				/= 8;

		BYTE btRed			= crFill&0xFF;
		BYTE btGreen		= (crFill&0xFF00)>>8;
		BYTE btBlue			= (crFill&0xFF0000)>>16;

		BITMAP bmImage;
		::GetObject(hDib, sizeof(BITMAP), &bmImage);

		for(int y=0; y<nHeight; y++){
			BYTE* lpBits = &((BYTE*)lpMap)[nIndex];
			for(int x=0; x<nWidth; x++){
				lpBits[0]	= btBlue; // Blue
				lpBits[1]	= btGreen; // Green
				lpBits[2]	= btRed; // Red
				lpBits		= &lpBits[nBPP];
				}
			nIndex += bmImage.bmWidthBytes;
			}
		}

	if( pBmp )
		::GetObject(hDib, sizeof(BITMAP), pBmp);

	Attach(hDib);
	return 1;
	}

BOOL
_Image::GetImageFromImage(_Rect rcDest, _Image& image){
	if( IsNull() || rcDest.IsRectEmpty() )
		return FALSE;
	
	BITMAP bmImage;
	::GetObject(image_, sizeof(BITMAP), &bmImage);

	_Image bmpImage;
	if( !bmpImage.CreateDIBBitmap((int)bmImage.bmBitsPixel, RGB(0, 0, 0), rcDest.Width(), rcDest.Height()) )
		return FALSE;

	BITMAP bmImageDest;
	::GetObject(bmpImage, sizeof(BITMAP), &bmImageDest);
	bmpImage.BitmapOnBitmap(0, 0, image_, rcDest.left, rcDest.top, bmImage.bmWidth, bmImage.bmHeight, rcDest.Width(), rcDest.Height()); 
	image.Attach((HBITMAP)bmpImage.Detach());
	return TRUE;
	}

BOOL
_Image::BitmapOnBitmap(int xDest, int yDest, ImageDef hBmpSrc, int xSrc, int ySrc, int nWidthSrc, int nHeightSrc, 
						int nDrawWidth, int nDrawHeight, BOOL bAlpha /*= FALSE*/, BYTE btAlpha /*= 255*/, BOOL bIgnoreFullAlphaPixel /*= FALSE*/, bool bDontAffectAlphaChannel /*= false*/)
	{
	if( IsNull() ){
		return FALSE;
		}

	ImageDef hBmpDest = image_;
	// check parameters validation. ======================================
	BITMAP bmpSrc, bmpDest;
	::GetObject(hBmpSrc, sizeof(BITMAP), &bmpSrc);
	::GetObject(hBmpDest, sizeof(BITMAP), &bmpDest);
	if( bmpSrc.bmBits == NULL || bmpDest.bmBits == NULL )
		return FALSE;
	// ===================================================================
	/*
	HDC srcDC = ::CreateCompatibleDC(NULL);
	HDC destDC = ::CreateCompatibleDC(NULL);

	BLENDFUNCTION bf;
	bf.SourceConstantAlpha = 0;
	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.AlphaFormat = AC_SRC_ALPHA;

	BOOL bRet = FALSE;//GdiAlphaBlend(

	DeleteDC(srcDC);
	DeleteDC(destDC);
	return bRet;
	*/

	BITMAP* pbmpDest = &bmpDest;
	BITMAP* pbmpSrc = &bmpSrc;
	ASSERT( xDest < pbmpDest->bmWidth  && nDrawWidth  <= pbmpDest->bmWidth);
	ASSERT( yDest < pbmpDest->bmHeight && nDrawHeight <= pbmpDest->bmHeight);
	ASSERT( xSrc  < pbmpSrc->bmWidth   && nWidthSrc   <= pbmpSrc->bmWidth );
	ASSERT( ySrc  < pbmpSrc->bmHeight  && nHeightSrc  <= pbmpSrc->bmHeight);

	if( !nWidthSrc ) nWidthSrc  = pbmpSrc->bmWidth;
	if( !nHeightSrc ) nHeightSrc = pbmpSrc->bmHeight;

	if( pbmpSrc->bmBitsPixel < 24 || pbmpDest->bmBitsPixel < 24 )
		return FALSE;

	int   nAlpha	= (int)btAlpha;
	_Rect rcDest	(xDest, yDest, xDest + nDrawWidth, yDest + nDrawHeight);
	_Rect rcSrc		(xSrc, ySrc, min(xSrc+nWidthSrc, pbmpSrc->bmWidth), min(ySrc+nHeightSrc, pbmpSrc->bmHeight));

	if( rcSrc.Width() > rcDest.Width() )
		rcSrc.right = rcSrc.left + rcDest.Width();
	if( rcSrc.Height() > rcDest.Height() )
		rcSrc.bottom = rcSrc.top + rcDest.Height();

	_Rect rcDrawDest(rcDest.left, rcDest.top, rcDest.left + rcSrc.Width(), rcDest.top + rcSrc.Height());
	BYTE* pbtColorByteByAlphaByte = GetColorByteByAlphaByteArray();

	if( pbmpSrc->bmBitsPixel != 0x20 )
		bAlpha = FALSE;

	int nBPPSrc	= pbmpSrc->bmBitsPixel / 8, nBPPDest = pbmpDest->bmBitsPixel / 8;
	if( bAlpha ){
		if( btAlpha != 255 ){ // Use constant alpha.
			while( rcDrawDest.Width() > 0 ){
				while( rcDrawDest.Height() > 0 ){
					// Perform draw operation here. ##########
					for( UINT yLoop = 0; yLoop < rcDrawDest.Height(); yLoop ++ ){
						BYTE*	pPixelSrc	= (BYTE*)(((UINT)pbmpSrc->bmBits) + pbmpSrc->bmWidthBytes*(pbmpSrc->bmHeight - yLoop - rcSrc.top - 1) + nBPPSrc*rcSrc.left);
						BYTE*	pPixelDest	= (BYTE*)(((UINT)pbmpDest->bmBits) + pbmpDest->bmWidthBytes*(pbmpDest->bmHeight - yLoop - rcDrawDest.top - 1) + nBPPDest*rcDrawDest.left);

						if( nBPPSrc == 4 ){
							for( UINT xLoop = 0; xLoop < rcDrawDest.Width(); xLoop ++ ){
								if( pPixelSrc[3] < 10 ){/*
									// Blue color byte.
									pPixelDest[0] = pbtColorByteByAlphaByte[pPixelSrc[0]*256 + nAlpha] + pbtColorByteByAlphaByte[pPixelDest[0]*256 + (255 - nAlpha)];
									// Green color byte.
									pPixelDest[1] = pbtColorByteByAlphaByte[pPixelSrc[1]*256 + nAlpha] + pbtColorByteByAlphaByte[pPixelDest[1]*256 + (255 - nAlpha)];
									// Red color byte.
									pPixelDest[2] = pbtColorByteByAlphaByte[pPixelSrc[2]*256 + nAlpha] + pbtColorByteByAlphaByte[pPixelDest[2]*256 + (255 - nAlpha)];*/
									}
								else{
									// Blue color byte.
									pPixelDest[0] = pbtColorByteByAlphaByte[pPixelSrc[0]*256 + nAlpha] + pbtColorByteByAlphaByte[pPixelDest[0]*256 + (255 - nAlpha)];
									// Green color byte.
									pPixelDest[1] = pbtColorByteByAlphaByte[pPixelSrc[1]*256 + nAlpha] + pbtColorByteByAlphaByte[pPixelDest[1]*256 + (255 - nAlpha)];
									// Red color byte.
									pPixelDest[2] = pbtColorByteByAlphaByte[pPixelSrc[2]*256 + nAlpha] + pbtColorByteByAlphaByte[pPixelDest[2]*256 + (255 - nAlpha)];
									}

								pPixelDest	= &pPixelDest[nBPPDest];
								pPixelSrc	= &pPixelSrc[nBPPSrc];
								}
							}
						else{
							for( UINT xLoop = 0; xLoop < rcDrawDest.Width(); xLoop ++ ){
								// Blue color byte.
								pPixelDest[0] = pbtColorByteByAlphaByte[pPixelSrc[0]*256 + nAlpha] + pbtColorByteByAlphaByte[pPixelDest[0]*256 + (255 - nAlpha)];
								// Green color byte.
								pPixelDest[1] = pbtColorByteByAlphaByte[pPixelSrc[1]*256 + nAlpha] + pbtColorByteByAlphaByte[pPixelDest[1]*256 + (255 - nAlpha)];
								// Red color byte.
								pPixelDest[2] = pbtColorByteByAlphaByte[pPixelSrc[2]*256 + nAlpha] + pbtColorByteByAlphaByte[pPixelDest[2]*256 + (255 - nAlpha)];

								pPixelDest	= &pPixelDest[nBPPDest];
								pPixelSrc	= &pPixelSrc[nBPPSrc];
								}
							}
						}
					// #######################################
					int nIncreaseWidth	 =  min(rcSrc.Width(), (rcDest.right - rcDrawDest.right));
					if( nIncreaseWidth <= 0 ) break;
					rcDrawDest.left		 = rcDrawDest.right;
					rcDrawDest.right	+= nIncreaseWidth;
					}

				int nIncreaseHeight	 =  min(rcSrc.Height(), (rcDest.bottom - rcDrawDest.bottom));
				if( nIncreaseHeight <= 0 ) break;
				rcDrawDest.top		 = rcDrawDest.bottom;
				rcDrawDest.bottom	+= nIncreaseHeight;
				rcDrawDest.left		 = rcDest.left;
				rcDrawDest.right	 = rcDest.left + rcSrc.Width();	
				}
			}
		else{
			bool bUseDestAlpha = (nBPPDest == 4);
			if( bDontAffectAlphaChannel )
				bUseDestAlpha = false;
			while( rcDrawDest.Width() > 0 ){
				while( rcDrawDest.Height() > 0 ){
					// Perform draw operation here. ##########
					for( UINT yLoop = 0; yLoop < rcDrawDest.Height(); yLoop ++ ){
						BYTE*	pPixelSrc	= (BYTE*)(((UINT)pbmpSrc->bmBits) + pbmpSrc->bmWidthBytes*(pbmpSrc->bmHeight - yLoop - rcSrc.top - 1) + nBPPSrc*rcSrc.left);
						BYTE*	pPixelDest	= (BYTE*)(((UINT)pbmpDest->bmBits) + pbmpDest->bmWidthBytes*(pbmpDest->bmHeight - yLoop - rcDrawDest.top - 1) + nBPPDest*rcDrawDest.left);

						for( UINT xLoop = 0; xLoop < rcDrawDest.Width(); xLoop ++ ){
							nAlpha		= pPixelSrc[3];
							if( bIgnoreFullAlphaPixel && !nAlpha ){
								pPixelDest	= &pPixelDest[nBPPDest];
								pPixelSrc	= &pPixelSrc[nBPPSrc];
								continue;
								}

							if( nAlpha < 255 ){
								// Blue color byte.
								pPixelDest[0] = pbtColorByteByAlphaByte[pPixelSrc[0]*256 + nAlpha] + pbtColorByteByAlphaByte[pPixelDest[0]*256 + (255 - nAlpha)];
								// Green color byte.
								pPixelDest[1] = pbtColorByteByAlphaByte[pPixelSrc[1]*256 + nAlpha] + pbtColorByteByAlphaByte[pPixelDest[1]*256 + (255 - nAlpha)];
								// Red color byte.
								pPixelDest[2] = pbtColorByteByAlphaByte[pPixelSrc[2]*256 + nAlpha] + pbtColorByteByAlphaByte[pPixelDest[2]*256 + (255 - nAlpha)];
								}
							else{
								pPixelDest[0] = pPixelSrc[0]; // rgbBlue
								pPixelDest[1] = pPixelSrc[1]; // rgbGreen
								pPixelDest[2] = pPixelSrc[2]; // rgbRed
								}

							if( bUseDestAlpha )
								pPixelDest[3] = nAlpha;
							pPixelDest	= &pPixelDest[nBPPDest];
							pPixelSrc	= &pPixelSrc[nBPPSrc];
							}
						}
					// #######################################
					int nIncreaseWidth	 =  min(rcSrc.Width(), (rcDest.right - rcDrawDest.right));
					if( nIncreaseWidth <= 0 ) break;
					rcDrawDest.left		 = rcDrawDest.right;
					rcDrawDest.right	+= nIncreaseWidth;
					}

				int nIncreaseHeight	 =  min(rcSrc.Height(), (rcDest.bottom - rcDrawDest.bottom));
				if( nIncreaseHeight <= 0 ) break;
				rcDrawDest.top		 = rcDrawDest.bottom;
				rcDrawDest.bottom	+= nIncreaseHeight;
				rcDrawDest.left		 = rcDest.left;
				rcDrawDest.right	 = rcDest.left + rcSrc.Width();	
				}
			}
		}
	else{
		int nCopyBytesCt =  (bDontAffectAlphaChannel ? min(3, min(nBPPDest, nBPPSrc)) : min(nBPPDest, nBPPSrc));
		while( rcDrawDest.Width() > 0 ){
			while( rcDrawDest.Height() > 0 ){
				// Perform draw operation here. ##########
				for( UINT yLoop = 0; yLoop < rcDrawDest.Height(); yLoop ++ ){
					BYTE*	pPixelSrc	= (BYTE*)(((UINT)pbmpSrc->bmBits) + pbmpSrc->bmWidthBytes*(pbmpSrc->bmHeight - yLoop - rcSrc.top - 1) + nBPPSrc*rcSrc.left);
					BYTE*	pPixelDest	= (BYTE*)(((UINT)pbmpDest->bmBits) + pbmpDest->bmWidthBytes*(pbmpDest->bmHeight - yLoop - rcDrawDest.top - 1) + nBPPDest*rcDrawDest.left);

					for( UINT xLoop = 0; xLoop < rcDrawDest.Width(); xLoop ++ ){
						memcpy(pPixelDest, pPixelSrc, nCopyBytesCt);
						/*
						pPixelDest[0] = pPixelSrc[0]; // rgbBlue
						pPixelDest[1] = pPixelSrc[1]; // rgbGreen
						pPixelDest[2] = pPixelSrc[2]; // rgbRed
//						pPixelDest[3] = pPixelSrc[3]; // rgbReserved*/
						pPixelDest		= &pPixelDest[nBPPDest];
						pPixelSrc		= &pPixelSrc[nBPPSrc];
						}
					}
				// #######################################
				
				int nIncreaseWidth	 =  min(rcSrc.Width(), (rcDest.right - rcDrawDest.right));
				if( nIncreaseWidth <= 0 ) break;
				rcDrawDest.left		 = rcDrawDest.right;
				rcDrawDest.right	+= nIncreaseWidth;
				}

			int nIncreaseHeight	 =  min(rcSrc.Height(), (rcDest.bottom - rcDrawDest.bottom));
			if( nIncreaseHeight <= 0 ) break;
			rcDrawDest.top		 = rcDrawDest.bottom;
			rcDrawDest.bottom	+= nIncreaseHeight;
			rcDrawDest.left		 = rcDest.left;
			rcDrawDest.right	 = rcDest.left + rcSrc.Width();	
			}
		}
	return TRUE;
	}

BOOL
_Image::AlphaColorOnBitmap(int xDest, int yDest, int nDrawWidth, int nDrawHeight, COLORREF crForeground, BYTE btAlpha){
	if( IsNull() )
		return FALSE;

	if( xDest < 0 || yDest < 0 ) 
		return FALSE;

	RGBQUAD	 rgbQuadSrc = {
	((BYTE)(crForeground >> 16)),	// Blue
	((BYTE)(crForeground >> 8)),	// Green
	((BYTE)(crForeground)),			// Red
	btAlpha							// Alpha
	};

	BITMAP bmpDest;
	GetObject(image_, sizeof(BITMAP), &bmpDest);
	if( bmpDest.bmBits == NULL || xDest >= bmpDest.bmWidth || yDest >= bmpDest.bmHeight )
		return FALSE;

	if( xDest + nDrawWidth > bmpDest.bmWidth )
		nDrawWidth = bmpDest.bmWidth - xDest;
	if( yDest + nDrawHeight > bmpDest.bmHeight )
		nDrawHeight = bmpDest.bmHeight - yDest;

	int nIndexDest, nIndexSrc = 0;
	int nGotoStartDest;
	int nWidth = nDrawWidth, nHeight = nDrawHeight;
	int xDrawed = 0, yDrawed = 0;
	int xSrc = 0, ySrc = 0;

	BYTE*	pPixelSrc	= (BYTE*)&rgbQuadSrc;
	BYTE*	pArrDest	= (BYTE*)bmpDest.bmBits;
	int		nAlpha		= ((int)btAlpha)&0xFF;
	int		nAlphaDest	=  255 - nAlpha;
	BYTE*	pbtColorByteByAlphaByte = GetColorByteByAlphaByteArray();
	int		nBPP		= bmpDest.bmBitsPixel / 8;

	while( yDrawed < nDrawHeight ){
		while( xDrawed < nDrawWidth ){
			// =================================================
			nIndexDest     = (xDrawed + xDest + (bmpDest.bmHeight - (1 + yDrawed + yDest))*bmpDest.bmWidthBytes);
			nGotoStartDest = bmpDest.bmWidthBytes;
			// =================================================

			// Basic operation on bitmaps. =====================
			for( int y = 0; y < nHeight; y++ ){
				BYTE* pPixelDest	= (BYTE*)&pArrDest[nIndexDest];
				for( int x = 0; x < nWidth; x++ ){
					// Set Alpha Color. ========================
					// Blue color byte.
					pPixelDest[0]		= pbtColorByteByAlphaByte[pPixelSrc[0]*256 + nAlpha] + pbtColorByteByAlphaByte[pPixelDest[0]*256 + nAlphaDest];
					// Green color byte.
					pPixelDest[1]		= pbtColorByteByAlphaByte[pPixelSrc[1]*256 + nAlpha] + pbtColorByteByAlphaByte[pPixelDest[1]*256 + nAlphaDest];
					// Red color byte.
					pPixelDest[2]		= pbtColorByteByAlphaByte[pPixelSrc[2]*256 + nAlpha] + pbtColorByteByAlphaByte[pPixelDest[2]*256 + nAlphaDest];
					// ==========================================
					pPixelDest			= &pPixelDest[nBPP];
					}
				nIndexDest -= nGotoStartDest;
				}
			// =================================================
			xDrawed += nWidth;
			}

		yDrawed += nHeight;
		xDrawed  = 0;
		}
	return TRUE;
	}

_Image*
_Image::CombineImages(_Image* pImageBg, _Image* pImageFg, bool bFgIsAlphaChannelImage, int nCombinedImageBPP /*= 24*/, _Rect rcForeground /*= Rect(0, 0, 0, 0)*/){
	if( !pImageBg || !pImageFg ) 
		return NULL;

	_Image bmpCombined;
	if( !bmpCombined.CreateDIBBitmap(bFgIsAlphaChannelImage ? 32 : nCombinedImageBPP, RGB(0, 0, 0), pImageBg->GetWidth(), pImageBg->GetHeight()) )
		return NULL;

	HDC memDC = CreateCompatibleDC(NULL);
	::SelectObject(memDC, bmpCombined);

	// Draw background image.
	_Rect rcDraw(0, 0, pImageBg->GetWidth(), pImageBg->GetHeight());
	HDC srcDC = ::CreateCompatibleDC(NULL);
	::SelectObject(srcDC, *pImageBg);
	BitBlt(memDC, 0, 0, rcDraw.Width(), rcDraw.Height(), srcDC, 0, 0, SRCCOPY);
	::DeleteDC(srcDC);

	if( rcForeground.IsRectEmpty() )
		rcForeground.SetRect(0, 0, pImageBg->GetWidth(), pImageBg->GetHeight());

	// Draw foreground image.
	if( bFgIsAlphaChannelImage ){
		_Size szBg(pImageBg->GetWidth(), pImageBg->GetHeight());
		_Size szFg(pImageFg->GetWidth(), pImageFg->GetHeight());

		// Copy alpha channel.
		if( szBg == _Size(rcForeground.Width(), rcForeground.Height()) ){
			BITMAP bmImageBg;
			GetObject(bmpCombined, sizeof(BITMAP), &bmImageBg);
			RGBQUAD* pColors = (RGBQUAD*)bmImageBg.bmBits;

			BITMAP bmImageFg;
			GetObject(*pImageFg, sizeof(BITMAP), &bmImageFg);
			RGBQUAD* pAlpha = (RGBQUAD*)bmImageFg.bmBits;

			if( bmImageFg.bmBitsPixel == 0x08 ){
				BYTE* pBtAlpha = 0;
				for(int y=0; y<szBg.cy; y++){
					pBtAlpha = (BYTE*)((LONG)bmImageFg.bmBits + bmImageFg.bmWidthBytes*(rcForeground.top + y) + rcForeground.left*sizeof(BYTE));
					for(int x=0; x<szBg.cx; x++){
						pColors[y*szBg.cx + x].rgbReserved = *pBtAlpha;
						pBtAlpha ++;
						}
					}
				}
			else{
				if( bmImageFg.bmBitsPixel == 32 ){
					for(int y=0; y<szBg.cy; y++){
						pAlpha = (RGBQUAD*)((LONG)bmImageFg.bmBits + bmImageFg.bmWidthBytes*(rcForeground.top + y) + rcForeground.left*sizeof(RGBQUAD));
						for(int x=0; x<szBg.cx; x++){
							pColors[y*szBg.cx + x].rgbReserved = pAlpha->rgbReserved;
							pAlpha ++;
							}
						}
					}
				else{
					int nBPPFg = bmImageFg.bmBitsPixel / 8;
					BYTE* pRGBFg = NULL;
					for(int y=0; y<szBg.cy; y++){
						pRGBFg = (BYTE*)((LONG)bmImageFg.bmBits + bmImageFg.bmWidthBytes*(rcForeground.top + y) + rcForeground.left*nBPPFg);
						for(int x=0; x<szBg.cx; x++){
							pColors[y*szBg.cx + x].rgbReserved = pRGBFg[0];
							pRGBFg = &pRGBFg[nBPPFg];
							}
						}	
					}
				}
			}
		}
	else{
		bmpCombined.BitmapOnBitmap(0, 0, *pImageFg, rcForeground.left, rcForeground.top, 
			rcForeground.Width(), rcForeground.Height(), rcDraw.Width(), rcDraw.Height(), (pImageFg->GetBPP() == 32), 255, 1);
		}

	// Clean up.
	::DeleteDC(memDC);

	_Image* pImageComb = new _Image();
	pImageComb->Attach((HBITMAP)bmpCombined.Detach());
	return pImageComb;
	}

BOOL
_Image::CopyAlphaChannel(ImageDef hDest, _Rect rcSrc /*= _Rect(0, 0, 0, 0)*/, bool bCheckDestPixelAlpha /*= false*/, BYTE btMaxDestPixelAlphaChange /*= 255*/, int nSrcAlphaPixel /*= -1*/){
	if( IsNull() ){
		ASSERT(FALSE);
		return FALSE;
		}

	BITMAP bmDest, bmSrc;
	GetObject(hDest, sizeof(BITMAP), &bmDest);
	GetObject(image_, sizeof(BITMAP), &bmSrc);
	if( !bmDest.bmBits || !bmSrc.bmBits )
		return false;

	if( rcSrc.IsRectEmpty() ){
		rcSrc.left		= 0;
		rcSrc.top		= 0;
		rcSrc.right		= min(bmDest.bmWidth, bmSrc.bmWidth);
		rcSrc.bottom	= min(bmDest.bmHeight, bmSrc.bmHeight);
		}
	else{
		_Rect rcDestImg	(0, 0, bmDest.bmWidth, bmDest.bmHeight);
		_Rect rcSrcImg	(0, 0, bmSrc.bmWidth, bmSrc.bmHeight);

		rcSrc &= rcSrcImg;
		if( rcSrc.Width() > bmDest.bmWidth ||
			rcSrc.Height() > bmDest.bmHeight )
			return false;
		}

	BYTE*	pBitsDest			= (BYTE*)bmDest.bmBits;
	BYTE*	pBitsSrc			= (BYTE*)bmSrc.bmBits;
	int		nIndexSrc			= 0, nIndexDest = 0;
	int		nBPPSrc				= bmSrc.bmBitsPixel/8, nBPPDest = bmDest.bmBitsPixel/8;
	int		nSrcAlphaCompIndex	= (nBPPSrc == 4 ? 3 : 0);
	int		nDestAlphaCompIndex	= (nBPPDest == 4 ? 3 : 0);
	int		nWidth				= rcSrc.Width();
	int		nHeight				= rcSrc.Height();

	nSrcAlphaCompIndex			= (nBPPSrc == 3 ? 1 : nSrcAlphaCompIndex);
	nDestAlphaCompIndex			= (nBPPDest == 3 ? 1 : nDestAlphaCompIndex);

	if( nSrcAlphaPixel > -1 && nSrcAlphaPixel < 4 )
		nSrcAlphaCompIndex		= nSrcAlphaPixel;

	if( !bCheckDestPixelAlpha ){
		for(int y=0; y<nHeight; y++){
			pBitsSrc	= (BYTE*)((LONG)bmSrc.bmBits + bmSrc.bmWidthBytes*(rcSrc.top + y) + rcSrc.left*nBPPSrc);
			pBitsDest	= (BYTE*)((LONG)bmDest.bmBits + bmDest.bmWidthBytes*y);
			for(int x=0; x<nWidth; x++){
				pBitsDest[nDestAlphaCompIndex]	= pBitsSrc[nSrcAlphaCompIndex];
				pBitsDest						+= nBPPDest;
				pBitsSrc						+= nBPPSrc;
				}
			}	
		}
	else{
		BYTE*	pbtColorByteByAlphaByte = GetColorByteByAlphaByteArray();
		BYTE	btAlpha					= 0;
		for(int y=0; y<nHeight; y++){
			pBitsSrc	= (BYTE*)((LONG)bmSrc.bmBits + bmSrc.bmWidthBytes*(rcSrc.top + y) + rcSrc.left*nBPPSrc);
			pBitsDest	= (BYTE*)((LONG)bmDest.bmBits + bmDest.bmWidthBytes*y);
			for(int x=0; x<nWidth; x++){
				BYTE btAlpha					= pBitsSrc[nSrcAlphaCompIndex];
				pBitsDest[nDestAlphaCompIndex]	= pbtColorByteByAlphaByte[btAlpha*256 + btAlpha] + pbtColorByteByAlphaByte[pBitsDest[nDestAlphaCompIndex]*256 + (255 - btAlpha)];
				pBitsDest	+= nBPPDest;
				pBitsSrc	+= nBPPSrc;
				}
			}

		// Old one.
			/*
			for(int y=0; y<nHeight; y++){
				pBitsSrc	= (BYTE*)((LONG)bmSrc.bmBits + bmSrc.bmWidthBytes*(rcSrc.top + y) + rcSrc.left*nBPPSrc);
				pBitsDest	= (BYTE*)((LONG)bmDest.bmBits + bmDest.bmWidthBytes*y);
				for(int x=0; x<nWidth; x++){				
					if( pBitsDest[nDestAlphaCompIndex] <= btMaxDestPixelAlphaChange )
						pBitsDest[nDestAlphaCompIndex]	= pBitsSrc[nSrcAlphaCompIndex];

					pBitsDest						+= nBPPDest;
					pBitsSrc						+= nBPPSrc;
					}
				}	
			}*/
		}

	return TRUE;
	}

BOOL
_Image::CopyImage(_Image* pCopyInto){
	if( !pCopyInto || IsNull() )
		return FALSE;
	pCopyInto->Destroy();
	return pCopyInto->CreateDIBBitmap(GetBPP(),  (UINT*)this->bmpInfo_.bmBits, GetWidth(), GetHeight());
	}

BOOL
_Image::EncodeImageData(LongBinary* pBinaryInto, ImageDataFormatType formatType){
	if( IsNull() || !pBinaryInto )
		return FALSE;

	HGLOBAL hImageData = ::GlobalAlloc(GMEM_MOVEABLE, 100);
	LPSTREAM lpStream = NULL;
	if( CreateStreamOnHGlobal(hImageData, FALSE, &lpStream) != S_OK ){
		::GlobalFree(hImageData);
		return FALSE;
		}

	GUID arrGUID[] = {
		{0xb96b3caf,0x0728,0x11d3,0x9d,0x7b,0x00,0x00,0xf8,0x1e,0xf3,0x2e}, // PNG
		{0xb96b3cae,0x0728,0x11d3,0x9d,0x7b,0x00,0x00,0xf8,0x1e,0xf3,0x2e}, // JPEG
		{0xb96b3cab,0x0728,0x11d3,0x9d,0x7b,0x00,0x00,0xf8,0x1e,0xf3,0x2e}, // BMP
	};

	int nIndex = 0;
	switch (formatType){
		case ImageAsPNG:
			nIndex = 0;
			break;
		case ImageAsJPG:
			nIndex = 1;
			break;
		case ImageAsBMP:
			nIndex = 2;
			break;
		default:
			nIndex = 0;
			break;
		}

	Gdiplus::Status status = Gdiplus::Status::Ok;
	try{
		if( GetBPP() == 32 ){
			Gdiplus::Bitmap bmSrc( GetWidth(), GetHeight(), 0, PixelFormat32bppARGB, (BYTE*)( GetBits() ) );
			status = bmSrc.Save(lpStream, &arrGUID[nIndex]);
			}
		else{
			Gdiplus::Bitmap bmSrc(image_, NULL);
			status = bmSrc.Save(lpStream, &arrGUID[nIndex]);
			}
		}
	catch(...){
		lpStream->Release();
		::GlobalFree(hImageData);
		return FALSE;
		}

	int nImageDataSize = 0;
	ULARGE_INTEGER uCurr = {0, 0};
	LARGE_INTEGER nMove = {0, 0};
	if( lpStream->Seek(nMove, STREAM_SEEK_CUR, &uCurr) == S_OK ){
		nImageDataSize = (int)uCurr.LowPart;
		}

	BOOL bRet = FALSE;
	if( pBinaryInto->SetBinarySize(nImageDataSize) ){
		void* lpDataSrc = ::GlobalLock(hImageData);
		memcpy(pBinaryInto->LockMemory(), lpDataSrc, nImageDataSize);
		pBinaryInto->UnlockMemory();
		::GlobalUnlock(hImageData);
		bRet = TRUE;
		}

	lpStream->Release();
	::GlobalFree(hImageData);
	return bRet;
	}

ImageDef
_Image::CombineTwo32bppImages(ImageDef hImage1, ImageDef hImage2){
	BITMAP bmImage1, bmImage2;
	if( GetObject(hImage1, sizeof(BITMAP), &bmImage1) != sizeof(BITMAP) )
		return NULL;
	if( GetObject(hImage2, sizeof(BITMAP), &bmImage2) != sizeof(BITMAP) )
		return NULL;

	if( !bmImage1.bmBits || !bmImage2.bmBits ||
		bmImage1.bmBitsPixel != 32 || bmImage2.bmBitsPixel != 32 ||
		bmImage1.bmWidth != bmImage2.bmWidth ||
		bmImage1.bmHeight != bmImage2.bmHeight ){
		ASSERT(FALSE);
		return 0L;
		}

	_Image imageCombined;
	if( !imageCombined.CreateDIBBitmap(32, RGB(0, 0, 0), bmImage1.bmWidth, bmImage1.bmHeight) )
		return NULL;

	BITMAP bmCombined;
	if( GetObject(imageCombined, sizeof(BITMAP), &bmCombined) != sizeof(BITMAP) ){
		imageCombined.Destroy();
		return NULL;
		}

	RGBQUAD*	pPixel1 = (RGBQUAD*)bmImage1.bmBits, *pPixel2 = (RGBQUAD*)bmImage2.bmBits, *pPixelDest = (RGBQUAD*)bmCombined.bmBits;
	int			nLoop = 0, nCt = bmImage1.bmHeight*bmImage1.bmWidth;
	float		fAlphaA = 0.0f, fAlphaB		= 0.0f, fAlphaDest = 0.0f;

	while(nLoop < nCt){
		fAlphaA		= (pPixel2->rgbReserved / 256.0f);
		fAlphaB		= (pPixel1->rgbReserved / 256.0f);
		fAlphaDest	= fAlphaA + fAlphaB*(1.0f - fAlphaA);
		
		// Second approach
		pPixelDest->rgbReserved = (BYTE)(256*fAlphaDest);
		pPixelDest->rgbBlue		= (BYTE)((pPixel2->rgbBlue*fAlphaA + (1.0f - fAlphaA)*pPixel1->rgbBlue*fAlphaB) / fAlphaDest);
		pPixelDest->rgbGreen	= (BYTE)((pPixel2->rgbGreen*fAlphaA + (1.0f - fAlphaA)*pPixel1->rgbGreen*fAlphaB) / fAlphaDest);
		pPixelDest->rgbRed		= (BYTE)((pPixel2->rgbRed*fAlphaA + (1.0f - fAlphaA)*pPixel1->rgbRed*fAlphaB) / fAlphaDest);

		pPixel1 ++;
		pPixel2 ++;
		pPixelDest ++;
		nLoop	++;
		}

	return (HBITMAP)imageCombined.Detach();
	}

BOOL
_Image::RenderImage_OmitBorder(_DC* pDC, const _Rect rcClipDestDC, const _Rect rcDestDC, _Rect rcOmitBorder, BOOL bAlpha /*= TRUE*/, BOOL bSkipMiddleCenterPart /*= FALSE*/){
	if( !pDC || IsNull() )
		return FALSE;

	int		nImageWidth		= GetWidth();
	int		nImageHeight	= GetHeight();

	int		nSrcImageX		= (rcClipDestDC.left - rcDestDC.left);
	int		nSrcImageY		= (rcClipDestDC.top - rcDestDC.top);
	int		nImageDrawCX	= rcClipDestDC.Width();
	int		nImageDrawCY	= rcClipDestDC.Height();

	return DrawImage32bpp_Omitborder(pDC, rcDestDC, rcClipDestDC, &rcOmitBorder, bAlpha, bSkipMiddleCenterPart); 
	}

BOOL
_Image::RenderImage(_DC* pDC, const _Rect rcClipDestDC, const _Rect rcDestDC, bool bScale /*= true*/, bool bUseAlphaChannel /*= true*/, 
				ScaleMode mode /*= ScaleMode::ScaleMode_NearestNeghbore*/, ImageDef* pImageRendered /*= NULL*/, int nAlphaConstant /*= 255*/, bool bDontAffectAlphaChannel /*= false*/)
	{
	if( !pDC || IsNull() )
		return FALSE;

	BOOL bRet = FALSE;
	// Scale image.
	if( bScale ){
		ImageDef	hDestBmp = 0;
		_Image		bmBkgnd;
		_Size		szClipDesDC(rcClipDestDC.Width(), rcClipDestDC.Height());
		if( bUseAlphaChannel ){
			if( pDC->GetDibImageFromDC(32, bmBkgnd, rcClipDestDC.left, rcClipDestDC.top, szClipDesDC.cx, szClipDesDC.cy) )
				hDestBmp = (HBITMAP)bmBkgnd.Detach();
			}

		_Rect rcDestInvalid;
		if( rcDestDC.left < 0 )
			rcDestInvalid.left = -1*rcDestDC.left;
		else
			rcDestInvalid.left = (rcClipDestDC.left - rcDestDC.left);

		if( rcDestDC.top < 0 )
			rcDestInvalid.top = -1*rcDestDC.top;
		else
			rcDestInvalid.top = (rcClipDestDC.top - rcDestDC.top);

		rcDestInvalid.right		= rcDestInvalid.left + szClipDesDC.cx;
		rcDestInvalid.bottom	= rcDestInvalid.top + szClipDesDC.cy;

		if( ScaleBitmap(mode, rcDestDC.Width(), rcDestDC.Height(), rcDestInvalid, hDestBmp, bUseAlphaChannel, nAlphaConstant, bDontAffectAlphaChannel) ){
			// Render on DC bitmap instead of DC. {{
			// Reason: We should keep alpha channel.
			bool		bRenderOnDC = true;
			_Image		bmDestBmp;
			bmDestBmp.Attach(hDestBmp);
			ImageDef	 pbmPrevSelected = pDC->SelectObject(bmDestBmp);

			if( pbmPrevSelected ){
				_Image imagePrevSelected;
				imagePrevSelected.Attach(pbmPrevSelected);
				pDC->SelectObject(pbmPrevSelected);

				bRet = imagePrevSelected.BitmapOnBitmap(rcClipDestDC.left, rcClipDestDC.top, hDestBmp, 0, 0, 
					szClipDesDC.cx, szClipDesDC.cy, szClipDesDC.cx, szClipDesDC.cy, 0, 255, 0, bDontAffectAlphaChannel);

				imagePrevSelected.Detach();
				bRenderOnDC = false;
				}
			else
				pDC->SelectObject(pbmPrevSelected);
			bmDestBmp.Detach();
			// }}

			if( bRenderOnDC ){
				HDC memDC = ::CreateCompatibleDC(*pDC);
				::SelectObject	(memDC, hDestBmp);
				::BitBlt		(*pDC, rcClipDestDC.left, rcClipDestDC.top, szClipDesDC.cx, szClipDesDC.cy, memDC, 0, 0, SRCCOPY);
				::DeleteDC		(memDC);
				bRet	= TRUE;
				}
			}

		if( bRet && pImageRendered )
			*pImageRendered = hDestBmp;
		else
		if( hDestBmp )
			::DeleteObject(hDestBmp);
		}
	else{ // Repeat image
		_Size szImage		(GetWidth(), GetHeight());
		_Rect rcOmitBorder	((szImage.cx-2)/2, (szImage.cy-2)/2, (szImage.cx-2)/2, (szImage.cy-2)/2);
		return RenderImage_OmitBorder(pDC, rcClipDestDC, rcDestDC, rcOmitBorder, (GetBPP() == 32));
		}
	return bRet;
	}

BOOL
_Image::ScaleBitmap(ScaleMode mode, UINT nDestCX, UINT nDestCY, _Rect rcInvalidDest, ImageDef& hBmpDest, bool bUseAlphaChannel, BYTE btConstantAlpha, bool bDontAffectAlphaChannel){
	if( IsNull() )
		return FALSE;

	BOOL bRet = 0;
	switch( mode ){
		case ScaleMode::ScaleMode_NearestNeghbore:{
			bRet = ScaleBitmap_NearestN(nDestCX, nDestCY, rcInvalidDest, hBmpDest, bUseAlphaChannel, btConstantAlpha, bDontAffectAlphaChannel);
			break;
			}
		case ScaleMode::ScaleMode_Bilinier:{
			bRet = ScaleBitmap_Bilinier(nDestCX, nDestCY, rcInvalidDest, hBmpDest, bUseAlphaChannel, btConstantAlpha, bDontAffectAlphaChannel);
			break;
			}
		}
	return bRet;
	}

BOOL
_Image::ScaleBitmap_NearestN(UINT nDestCX, UINT nDestCY, _Rect rcInvalidDest, ImageDef& hBmpDest, bool bUseAlphaChannel, BYTE btConstantAlpha, bool bDontAffectAlphaChannel){
	if( IsNull() )
		return FALSE;

	BITMAP bmSrc;
	bmSrc.bmBits = NULL;
	GetObject(image_, sizeof(BITMAP), &bmSrc);
	if( !bmSrc.bmBits ) 
		return FALSE;

	_Rect rcDest(0, 0, nDestCX, nDestCY);
	rcInvalidDest &= rcDest;
	if( rcInvalidDest.IsRectEmpty() )
		return NULL;

	_Image	bmDest;
	BITMAP	bmpDest;

	if( hBmpDest != NULL && GetObject(hBmpDest, sizeof(BITMAP), &bmpDest) == sizeof(BITMAP) && 
		bmpDest.bmWidth >= rcInvalidDest.Width() && bmpDest.bmHeight >= rcInvalidDest.Height() ){
		bmDest.Attach(hBmpDest);
		}
	else{
		if( !bmDest.CreateDIBBitmap(bmSrc.bmBitsPixel, (COLORREF)0, rcInvalidDest.Width(), rcInvalidDest.Height(), &bmpDest) )
			return FALSE;
		}

	// Calculate scaling params
	float dx					= 1.0f / (nDestCX / (float)bmSrc.bmWidth);
	float dy					= 1.0f / (nDestCY / (float)bmSrc.bmHeight);
	long _width					= bmpDest.bmWidth;
	long _height				= bmpDest.bmHeight;
	long _pitch					= bmpDest.bmWidthBytes;

	long m_iPitch				= bmSrc.bmWidthBytes;
	long m_iBpp					= (bmSrc.bmBitsPixel / 8);
	int biWidth					= bmSrc.bmWidth;
	int biHeight				= bmSrc.bmHeight;

	fixed f_dx					= ftofx(dx);
	fixed f_dy					= ftofx(dy);
	fixed f_1					= itofx(1);

	if( nDestCX == biWidth && nDestCY == biHeight ){
		if( bmDest.BitmapOnBitmap(0, 0, image_, rcInvalidDest.left, rcInvalidDest.top, biWidth, biHeight, rcInvalidDest.Width(), rcInvalidDest.Height(), bUseAlphaChannel, btConstantAlpha, 0, bDontAffectAlphaChannel) ){
			hBmpDest = (HBITMAP)bmDest.Detach();
			return 1;
			}
		return 0;
		}

	if( rcInvalidDest.Width() < nDestCX ||
		rcInvalidDest.Height() < nDestCY ){
		int nHeight				= rcInvalidDest.Height();
		rcInvalidDest.top		= (nDestCY - rcInvalidDest.top - nHeight);
		rcInvalidDest.bottom	= rcInvalidDest.top + nHeight;
		}

	// Create temporary bitmap
	DWORD	dwSize				= _pitch * _height;
	LPBYTE	lpData				= (LPBYTE)bmpDest.bmBits;

	// Scale bitmap
	DWORD	dwDstHorizontalOffset;
	DWORD	dwDstVerticalOffset = 0;
	DWORD	dwDstTotalOffset;
	LPDWORD lpSrcData			= (LPDWORD)bmSrc.bmBits;
	DWORD	dwSrcTotalOffset	= 0;
	LPDWORD lpDstData			= (LPDWORD)lpData;

	if( bmpDest.bmBitsPixel != 32 )
		bUseAlphaChannel = false;

	if( bUseAlphaChannel ){
		BYTE*	pbtColorByteByAlphaByte = GetColorByteByAlphaByteArray();
		if( btConstantAlpha == 255 ){
			for (long i=rcInvalidDest.bottom-1; i>=rcInvalidDest.top; i--){
				dwDstHorizontalOffset	= 0;
				dwDstVerticalOffset		= (i - rcInvalidDest.top)*_pitch;
				for (long j=rcInvalidDest.left; j<rcInvalidDest.right; j++){
					// Update destination total offset
					dwDstTotalOffset = dwDstVerticalOffset + dwDstHorizontalOffset;

					// Update bitmap
					fixed f_i						= itofx(i);
					fixed f_j						= itofx(j);
					fixed f_a						= Mulfx(f_i, f_dy);
					fixed f_b						= Mulfx(f_j, f_dx);
					long m							= fxtoi(f_a);
					long n							= fxtoi(f_b);
					dwSrcTotalOffset				= m*m_iPitch + n*m_iBpp;

					RGBQUAD* pPixelSrc				= (RGBQUAD*)&lpSrcData[dwSrcTotalOffset>>2];
					BYTE red						= pPixelSrc->rgbRed;
					BYTE green						= pPixelSrc->rgbGreen;
					BYTE blue						= pPixelSrc->rgbBlue;
					BYTE alpha						= pPixelSrc->rgbReserved;
			
					//lpDstData[dwDstTotalOffset>>2]	= lpSrcData[dwSrcTotalOffset>>2];

					// Set Alpha Color. ========================
					RGBQUAD* pPixelDest = (RGBQUAD*)&lpDstData[dwDstTotalOffset>>2]; 
					// Blue color byte.
					((BYTE*)pPixelDest)[0] = pbtColorByteByAlphaByte[blue*256 + alpha] + pbtColorByteByAlphaByte[pPixelDest->rgbBlue*256 + (255 - alpha)];
					// Green color byte.
					((BYTE*)pPixelDest)[1] = pbtColorByteByAlphaByte[green*256 + alpha] + pbtColorByteByAlphaByte[pPixelDest->rgbGreen*256 + (255 - alpha)];
					// Red color byte.
					((BYTE*)pPixelDest)[2] = pbtColorByteByAlphaByte[red*256 + alpha] + pbtColorByteByAlphaByte[pPixelDest->rgbRed*256 + (255 - alpha)];
					// Alpha
					((BYTE*)pPixelDest)[3] = alpha;
					// ==========================================

					// Update destination horizontal offset
					dwDstHorizontalOffset += m_iBpp;
					}
				}
			}
		else{
			for (long i=rcInvalidDest.bottom-1; i>=rcInvalidDest.top; i--){
				dwDstHorizontalOffset	= 0;
				dwDstVerticalOffset		= (i - rcInvalidDest.top)*_pitch;
				for (long j=rcInvalidDest.left; j<rcInvalidDest.right; j++){
					// Update destination total offset
					dwDstTotalOffset = dwDstVerticalOffset + dwDstHorizontalOffset;

					// Update bitmap
					fixed f_i						= itofx(i);
					fixed f_j						= itofx(j);
					fixed f_a						= Mulfx(f_i, f_dy);
					fixed f_b						= Mulfx(f_j, f_dx);
					long m							= fxtoi(f_a);
					long n							= fxtoi(f_b);
					dwSrcTotalOffset				= m*m_iPitch + n*m_iBpp;

					RGBQUAD* pPixelSrc				= (RGBQUAD*)&lpSrcData[dwSrcTotalOffset>>2];
					BYTE red						= pPixelSrc->rgbRed;
					BYTE green						= pPixelSrc->rgbGreen;
					BYTE blue						= pPixelSrc->rgbBlue;
					BYTE alpha						= pPixelSrc->rgbReserved;
			
					//lpDstData[dwDstTotalOffset>>2]	= lpSrcData[dwSrcTotalOffset>>2];

					// Set Alpha Color. ========================
					RGBQUAD* pPixelDest = (RGBQUAD*)&lpDstData[dwDstTotalOffset>>2]; 
					// Blue color byte.
					((BYTE*)pPixelDest)[0] = pbtColorByteByAlphaByte[blue*256 + alpha] + pbtColorByteByAlphaByte[pPixelDest->rgbBlue*256 + (255 - alpha)];
					// Green color byte.
					((BYTE*)pPixelDest)[1] = pbtColorByteByAlphaByte[green*256 + alpha] + pbtColorByteByAlphaByte[pPixelDest->rgbGreen*256 + (255 - alpha)];
					// Red color byte.
					((BYTE*)pPixelDest)[2] = pbtColorByteByAlphaByte[red*256 + alpha] + pbtColorByteByAlphaByte[pPixelDest->rgbRed*256 + (255 - alpha)];
					// Alpha
					if( alpha > 0 ){
						((BYTE*)pPixelDest)[3] = btConstantAlpha;
						}
					// ==========================================

					// Update destination horizontal offset
					dwDstHorizontalOffset += m_iBpp;
					}
				}
			}
		}
	else{
		for (long i=rcInvalidDest.bottom-1; i>=rcInvalidDest.top; i--){
			dwDstHorizontalOffset	= 0;
			dwDstVerticalOffset		= (i - rcInvalidDest.top)*_pitch;
			for (long j=rcInvalidDest.left; j<rcInvalidDest.right; j++){
				// Update destination total offset
				dwDstTotalOffset				= dwDstVerticalOffset + dwDstHorizontalOffset;

				// Update bitmap
				fixed f_i						= itofx(i);
				fixed f_j						= itofx(j);
				fixed f_a						= Mulfx(f_i, f_dy);
				fixed f_b						= Mulfx(f_j, f_dx);
				long m							= fxtoi(f_a);
				long n							= fxtoi(f_b);
				dwSrcTotalOffset				= m*m_iPitch + n*m_iBpp;

				//lpDstData[dwDstTotalOffset>>2]	= lpSrcData[dwSrcTotalOffset>>2];
				((BYTE*)lpDstData)[dwDstTotalOffset]	= ((BYTE*)lpSrcData)[dwSrcTotalOffset];		// Blue
				((BYTE*)lpDstData)[dwDstTotalOffset+1]	= ((BYTE*)lpSrcData)[dwSrcTotalOffset+1];	// Green
				((BYTE*)lpDstData)[dwDstTotalOffset+2]	= ((BYTE*)lpSrcData)[dwSrcTotalOffset+2];	// Red

				// Update destination horizontal offset
				dwDstHorizontalOffset += m_iBpp;
				}
			}
		}

	hBmpDest = (ImageDef)bmDest.Detach();
	return TRUE;
	}

BOOL
_Image::ScaleBitmap_Bilinier(UINT nDestCX, UINT nDestCY, _Rect rcInvalidDest, ImageDef& hBmpDest, bool bUseAlphaChannel, BYTE btConstantAlpha, bool bDontAffectAlphaChannel){
	if( IsNull() )
		return FALSE;

	BITMAP bmSrc;
	bmSrc.bmBits = NULL;
	GetObject(image_, sizeof(BITMAP), &bmSrc);
	if( !bmSrc.bmBits ) return 0;

	_Rect rcDest(0, 0, nDestCX, nDestCY);
	rcInvalidDest &= rcDest;
	if( rcInvalidDest.IsRectEmpty() )
		return FALSE;

	_Image	bmDest;
	BITMAP	bmpDest;
	memset(&bmpDest, 0, sizeof(BITMAP));

	if( hBmpDest != NULL && GetObject(hBmpDest, sizeof(BITMAP), &bmpDest) == sizeof(BITMAP) && 
		bmpDest.bmWidth >= rcInvalidDest.Width() && bmpDest.bmHeight >= rcInvalidDest.Height() ){
		bmDest.Attach(hBmpDest);
		}
	else{
		if( !bmDest.CreateDIBBitmap(bmSrc.bmBitsPixel, (COLORREF)0, rcInvalidDest.Width(), rcInvalidDest.Height(), &bmpDest) )
			return 0;
		}

	// Calculate scaling params
	float dx					= 1.0f / (nDestCX / (float)bmSrc.bmWidth);
	float dy					= 1.0f / (nDestCY / (float)bmSrc.bmHeight);
	long _width					= bmpDest.bmWidth;
	long _height				= bmpDest.bmHeight;
	long _pitch					= bmpDest.bmWidthBytes;

	long m_iPitch				= bmSrc.bmWidthBytes;
	long m_iBpp					= (bmSrc.bmBitsPixel / 8);
	int biWidth					= bmSrc.bmWidth;
	int biHeight				= bmSrc.bmHeight;

	fixed f_dx					= ftofx(dx);
	fixed f_dy					= ftofx(dy);
	fixed f_1					= itofx(1);
	
	if( nDestCX == biWidth && nDestCY == biHeight ){
		if( bmDest.BitmapOnBitmap(0, 0, image_, rcInvalidDest.left, rcInvalidDest.top, biWidth, biHeight, rcInvalidDest.Width(), rcInvalidDest.Height(), bUseAlphaChannel, btConstantAlpha, 0, bDontAffectAlphaChannel) ){
			hBmpDest = (HBITMAP)bmDest.Detach();
			return 1;
			}
		return 0;
		}

	if( rcInvalidDest.Width() < nDestCX ||
		rcInvalidDest.Height() < nDestCY ){
		int nHeight				= rcInvalidDest.Height();
		rcInvalidDest.top		= (nDestCY - rcInvalidDest.top - nHeight);
		rcInvalidDest.bottom	= rcInvalidDest.top + nHeight;
		}

	// Create temporary bitmap
	DWORD	dwSize				= _pitch * _height;
	LPBYTE	lpData				= (LPBYTE)bmpDest.bmBits;
	ASSERT(lpData);

	// Scale bitmap
	DWORD	dwDstHorizontalOffset;
	DWORD	dwDstVerticalOffset = 0;
	DWORD	dwDstTotalOffset;
	LPDWORD lpSrcData			= (LPDWORD)bmSrc.bmBits;
	DWORD	dwSrcTotalOffset	= 0;
	LPDWORD lpDstData			= (LPDWORD)lpData;

	if( /*bmpDest.bmBitsPixel != 32 &&*/ bmSrc.bmBitsPixel != 32 )
		bUseAlphaChannel = false;

	if( !bDontAffectAlphaChannel && bmpDest.bmBitsPixel != 32 )
		bDontAffectAlphaChannel = true;

	if( bUseAlphaChannel ){
		BYTE*	pbtColorByteByAlphaByte = GetColorByteByAlphaByteArray();
		if( btConstantAlpha == 255 ){
			for (long i=rcInvalidDest.bottom-1; i>=rcInvalidDest.top; i--){
				dwDstHorizontalOffset	= 0;
				dwDstVerticalOffset		= (i - rcInvalidDest.top)*_pitch;
				for (long j=rcInvalidDest.left; j<rcInvalidDest.right; j++){
					// Update destination total offset
					dwDstTotalOffset = dwDstVerticalOffset + dwDstHorizontalOffset;

					// Update bitmap
					fixed f_i = itofx(i);
					fixed f_j = itofx(j);
					fixed f_a = Mulfx(f_i, f_dy);
					fixed f_b = Mulfx(f_j, f_dx);
					long m = fxtoi(f_a);
					long n = fxtoi(f_b);
					fixed f_f = f_a - itofx(m);
					fixed f_g = f_b - itofx(n);
					dwSrcTotalOffset = m*m_iPitch + n*m_iBpp;
					DWORD dwSrcTopLeft = dwSrcTotalOffset;
					DWORD dwSrcTopRight = dwSrcTotalOffset + m_iBpp;
					if (n >= biWidth-1)
						dwSrcTopRight = dwSrcTotalOffset;
					DWORD dwSrcBottomLeft = dwSrcTotalOffset + m_iPitch;
					if (m >= biHeight-1 )
						dwSrcBottomLeft = dwSrcTotalOffset;
					DWORD dwSrcBottomRight = dwSrcTotalOffset + m_iPitch + m_iBpp;
					if ((n >= biWidth-1) || (m >= biHeight-1))
						dwSrcBottomRight = dwSrcTotalOffset;

					fixed f_w1		= Mulfx(f_1-f_f, f_1-f_g);
					fixed f_w2		= Mulfx(f_1-f_f, f_g);
					fixed f_w3		= Mulfx(f_f, f_1-f_g);
					fixed f_w4		= Mulfx(f_f, f_g);
					/*
					_PIXEL pixel1	= lpSrcData[dwSrcTopLeft>>2];
					_PIXEL pixel2	= lpSrcData[dwSrcTopRight>>2];
					_PIXEL pixel3	= lpSrcData[dwSrcBottomLeft>>2];
					_PIXEL pixel4	= lpSrcData[dwSrcBottomRight>>2];
					*/
					_PIXEL pixel1	= *((DWORD*)&((BYTE*)lpSrcData)[dwSrcTopLeft]);
					_PIXEL pixel2	= *((DWORD*)&((BYTE*)lpSrcData)[dwSrcTopRight]);
					_PIXEL pixel3	= *((DWORD*)&((BYTE*)lpSrcData)[dwSrcBottomLeft]);
					_PIXEL pixel4	= *((DWORD*)&((BYTE*)lpSrcData)[dwSrcBottomRight]);

					fixed f_r1		= itofx(_GetRValue(pixel1));
					fixed f_r2		= itofx(_GetRValue(pixel2));
					fixed f_r3		= itofx(_GetRValue(pixel3));
					fixed f_r4		= itofx(_GetRValue(pixel4));
					fixed f_g1		= itofx(_GetGValue(pixel1));
					fixed f_g2		= itofx(_GetGValue(pixel2));
					fixed f_g3		= itofx(_GetGValue(pixel3));
					fixed f_g4		= itofx(_GetGValue(pixel4));
					fixed f_b1		= itofx(_GetBValue(pixel1));
					fixed f_b2		= itofx(_GetBValue(pixel2));
					fixed f_b3		= itofx(_GetBValue(pixel3));
					fixed f_b4		= itofx(_GetBValue(pixel4));

					fixed f_a1		= itofx(_GetAValue(pixel1));
					fixed f_a2		= itofx(_GetAValue(pixel2));
					fixed f_a3		= itofx(_GetAValue(pixel3));
					fixed f_a4		= itofx(_GetAValue(pixel4));

					BYTE red		= (BYTE)fxtoi(Mulfx(f_w1, f_r1) + Mulfx(f_w2, f_r2) + Mulfx(f_w3, f_r3) + Mulfx(f_w4, f_r4));
					BYTE green		= (BYTE)fxtoi(Mulfx(f_w1, f_g1) + Mulfx(f_w2, f_g2) + Mulfx(f_w3, f_g3) + Mulfx(f_w4, f_g4));
					BYTE blue		= (BYTE)fxtoi(Mulfx(f_w1, f_b1) + Mulfx(f_w2, f_b2) + Mulfx(f_w3, f_b3) + Mulfx(f_w4, f_b4));
					BYTE alpha		= (BYTE)fxtoi(Mulfx(f_w1, f_a1) + Mulfx(f_w2, f_a2) + Mulfx(f_w3, f_a3) + Mulfx(f_w4, f_a4));
					/*
					// Set Alpha Color. ========================
					RGBQUAD* pPixelDest = (RGBQUAD*)&lpDstData[dwDstTotalOffset>>2]; 
					// Blue color byte.
					((BYTE*)pPixelDest)[0] = pbtColorByteByAlphaByte[blue*256 + alpha] + pbtColorByteByAlphaByte[pPixelDest->rgbBlue*256 + (255 - alpha)];
					// Green color byte.
					((BYTE*)pPixelDest)[1] = pbtColorByteByAlphaByte[green*256 + alpha] + pbtColorByteByAlphaByte[pPixelDest->rgbGreen*256 + (255 - alpha)];
					// Red color byte.
					((BYTE*)pPixelDest)[2] = pbtColorByteByAlphaByte[red*256 + alpha] + pbtColorByteByAlphaByte[pPixelDest->rgbRed*256 + (255 - alpha)];
					// Alpha
					if( !bDontAffectAlphaChannel )
						((BYTE*)pPixelDest)[3] = alpha;
					// ==========================================
					*/
					BYTE* pPixelDest = (BYTE*)&((BYTE*)lpDstData)[dwDstTotalOffset]; 
					((BYTE*)lpDstData)[dwDstTotalOffset]	= pbtColorByteByAlphaByte[blue*256 + alpha] + pbtColorByteByAlphaByte[pPixelDest[0]*256 + (255 - alpha)]; // Blue
					((BYTE*)lpDstData)[dwDstTotalOffset+1]	= pbtColorByteByAlphaByte[green*256 + alpha] + pbtColorByteByAlphaByte[pPixelDest[1]*256 + (255 - alpha)];// Green
					((BYTE*)lpDstData)[dwDstTotalOffset+2]	= pbtColorByteByAlphaByte[red*256 + alpha] + pbtColorByteByAlphaByte[pPixelDest[2]*256 + (255 - alpha)];// Red
					// Alpha
					if( !bDontAffectAlphaChannel )
						((BYTE*)pPixelDest)[3] = alpha;

					// Update destination horizontal offset
					dwDstHorizontalOffset += m_iBpp;
					}
				}
			}
		else{
			for (long i=rcInvalidDest.bottom-1; i>=rcInvalidDest.top; i--){
				dwDstHorizontalOffset	= 0;
				dwDstVerticalOffset		= (i - rcInvalidDest.top)*_pitch;
				for (long j=rcInvalidDest.left; j<rcInvalidDest.right; j++){
					// Update destination total offset
					dwDstTotalOffset = dwDstVerticalOffset + dwDstHorizontalOffset;

					// Update bitmap
					fixed f_i = itofx(i);
					fixed f_j = itofx(j);
					fixed f_a = Mulfx(f_i, f_dy);
					fixed f_b = Mulfx(f_j, f_dx);
					long m = fxtoi(f_a);
					long n = fxtoi(f_b);
					fixed f_f = f_a - itofx(m);
					fixed f_g = f_b - itofx(n);
					dwSrcTotalOffset = m*m_iPitch + n*m_iBpp;
					DWORD dwSrcTopLeft = dwSrcTotalOffset;
					DWORD dwSrcTopRight = dwSrcTotalOffset + m_iBpp;
					if (n >= biWidth-1)
						dwSrcTopRight = dwSrcTotalOffset;
					DWORD dwSrcBottomLeft = dwSrcTotalOffset + m_iPitch;
					if (m >= biHeight-1 )
						dwSrcBottomLeft = dwSrcTotalOffset;
					DWORD dwSrcBottomRight = dwSrcTotalOffset + m_iPitch + m_iBpp;
					if ((n >= biWidth-1) || (m >= biHeight-1))
						dwSrcBottomRight = dwSrcTotalOffset;

					fixed f_w1		= Mulfx(f_1-f_f, f_1-f_g);
					fixed f_w2		= Mulfx(f_1-f_f, f_g);
					fixed f_w3		= Mulfx(f_f, f_1-f_g);
					fixed f_w4		= Mulfx(f_f, f_g);
					/*
					_PIXEL pixel1	= lpSrcData[dwSrcTopLeft>>2];
					_PIXEL pixel2	= lpSrcData[dwSrcTopRight>>2];
					_PIXEL pixel3	= lpSrcData[dwSrcBottomLeft>>2];
					_PIXEL pixel4	= lpSrcData[dwSrcBottomRight>>2];
*/
					_PIXEL pixel1	= *((DWORD*)&((BYTE*)lpSrcData)[dwSrcTopLeft]);
					_PIXEL pixel2	= *((DWORD*)&((BYTE*)lpSrcData)[dwSrcTopRight]);
					_PIXEL pixel3	= *((DWORD*)&((BYTE*)lpSrcData)[dwSrcBottomLeft]);
					_PIXEL pixel4	= *((DWORD*)&((BYTE*)lpSrcData)[dwSrcBottomRight]);

					fixed f_r1		= itofx(_GetRValue(pixel1));
					fixed f_r2		= itofx(_GetRValue(pixel2));
					fixed f_r3		= itofx(_GetRValue(pixel3));
					fixed f_r4		= itofx(_GetRValue(pixel4));
					fixed f_g1		= itofx(_GetGValue(pixel1));
					fixed f_g2		= itofx(_GetGValue(pixel2));
					fixed f_g3		= itofx(_GetGValue(pixel3));
					fixed f_g4		= itofx(_GetGValue(pixel4));
					fixed f_b1		= itofx(_GetBValue(pixel1));
					fixed f_b2		= itofx(_GetBValue(pixel2));
					fixed f_b3		= itofx(_GetBValue(pixel3));
					fixed f_b4		= itofx(_GetBValue(pixel4));

					fixed f_a1		= itofx(_GetAValue(pixel1));
					fixed f_a2		= itofx(_GetAValue(pixel2));
					fixed f_a3		= itofx(_GetAValue(pixel3));
					fixed f_a4		= itofx(_GetAValue(pixel4));

					BYTE red		= (BYTE)fxtoi(Mulfx(f_w1, f_r1) + Mulfx(f_w2, f_r2) + Mulfx(f_w3, f_r3) + Mulfx(f_w4, f_r4));
					BYTE green		= (BYTE)fxtoi(Mulfx(f_w1, f_g1) + Mulfx(f_w2, f_g2) + Mulfx(f_w3, f_g3) + Mulfx(f_w4, f_g4));
					BYTE blue		= (BYTE)fxtoi(Mulfx(f_w1, f_b1) + Mulfx(f_w2, f_b2) + Mulfx(f_w3, f_b3) + Mulfx(f_w4, f_b4));
					BYTE alpha		= (BYTE)fxtoi(Mulfx(f_w1, f_a1) + Mulfx(f_w2, f_a2) + Mulfx(f_w3, f_a3) + Mulfx(f_w4, f_a4));
					/*
					// Set Alpha Color. ========================
					RGBQUAD* pPixelDest = (RGBQUAD*)&lpDstData[dwDstTotalOffset>>2]; 
					// Blue color byte.
					((BYTE*)pPixelDest)[0] = pbtColorByteByAlphaByte[blue*256 + alpha] + pbtColorByteByAlphaByte[pPixelDest->rgbBlue*256 + (255 - alpha)];
					// Green color byte.
					((BYTE*)pPixelDest)[1] = pbtColorByteByAlphaByte[green*256 + alpha] + pbtColorByteByAlphaByte[pPixelDest->rgbGreen*256 + (255 - alpha)];
					// Red color byte.
					((BYTE*)pPixelDest)[2] = pbtColorByteByAlphaByte[red*256 + alpha] + pbtColorByteByAlphaByte[pPixelDest->rgbRed*256 + (255 - alpha)];
					*/

					BYTE* pPixelDest = (BYTE*)&((BYTE*)lpDstData)[dwDstTotalOffset]; 
					((BYTE*)lpDstData)[dwDstTotalOffset]	= pbtColorByteByAlphaByte[blue*256 + alpha] + pbtColorByteByAlphaByte[pPixelDest[0]*256 + (255 - alpha)]; // Blue
					((BYTE*)lpDstData)[dwDstTotalOffset+1]	= pbtColorByteByAlphaByte[green*256 + alpha] + pbtColorByteByAlphaByte[pPixelDest[1]*256 + (255 - alpha)];// Green
					((BYTE*)lpDstData)[dwDstTotalOffset+2]	= pbtColorByteByAlphaByte[red*256 + alpha] + pbtColorByteByAlphaByte[pPixelDest[2]*256 + (255 - alpha)];	// Red

					// Alpha
				//	if( alpha > 0 ){
				//		((BYTE*)pPixelDest)[3] = btConstantAlpha;
				//		}
					// ==========================================

					// Update destination horizontal offset
					dwDstHorizontalOffset += m_iBpp;
					}
				}
			}
		}
	else{
		for (long i=rcInvalidDest.bottom-1; i>=rcInvalidDest.top; i--){
			dwDstHorizontalOffset	= 0;
			dwDstVerticalOffset		= (i - rcInvalidDest.top)*_pitch;
			for (long j=rcInvalidDest.left; j<rcInvalidDest.right; j++){
				// Update destination total offset
				dwDstTotalOffset = dwDstVerticalOffset + dwDstHorizontalOffset;

				// Update bitmap
				fixed f_i = itofx(i);
				fixed f_j = itofx(j);
				fixed f_a = Mulfx(f_i, f_dy);
				fixed f_b = Mulfx(f_j, f_dx);
				long m = fxtoi(f_a);
				long n = fxtoi(f_b);
				fixed f_f = f_a - itofx(m);
				fixed f_g = f_b - itofx(n);
				dwSrcTotalOffset = m*m_iPitch + n*m_iBpp;
				DWORD dwSrcTopLeft = dwSrcTotalOffset;
				DWORD dwSrcTopRight = dwSrcTotalOffset + m_iBpp;
				if (n >= biWidth-1)
					dwSrcTopRight = dwSrcTotalOffset;
				DWORD dwSrcBottomLeft = dwSrcTotalOffset + m_iPitch;
				if (m >= biHeight-1 )
					dwSrcBottomLeft = dwSrcTotalOffset;
				DWORD dwSrcBottomRight = dwSrcTotalOffset + m_iPitch + m_iBpp;
				if ((n >= biWidth-1) || (m >= biHeight-1))
					dwSrcBottomRight = dwSrcTotalOffset;

				fixed f_w1		= Mulfx(f_1-f_f, f_1-f_g);
				fixed f_w2		= Mulfx(f_1-f_f, f_g);
				fixed f_w3		= Mulfx(f_f, f_1-f_g);
				fixed f_w4		= Mulfx(f_f, f_g);

				_PIXEL pixel1	= *((DWORD*)&((BYTE*)lpSrcData)[dwSrcTopLeft]);
				_PIXEL pixel2	= *((DWORD*)&((BYTE*)lpSrcData)[dwSrcTopRight]);
				_PIXEL pixel3	= *((DWORD*)&((BYTE*)lpSrcData)[dwSrcBottomLeft]);
				_PIXEL pixel4	= *((DWORD*)&((BYTE*)lpSrcData)[dwSrcBottomRight]);
	
				/*
				_PIXEL pixel1	= lpSrcData[dwSrcTopLeft>>2];
				_PIXEL pixel2	= lpSrcData[dwSrcTopRight>>2];
				_PIXEL pixel3	= lpSrcData[dwSrcBottomLeft>>2];
				_PIXEL pixel4	= lpSrcData[dwSrcBottomRight>>2];*/

				fixed f_r1		= itofx(_GetRValue(pixel1));
				fixed f_r2		= itofx(_GetRValue(pixel2));
				fixed f_r3		= itofx(_GetRValue(pixel3));
				fixed f_r4		= itofx(_GetRValue(pixel4));
				fixed f_g1		= itofx(_GetGValue(pixel1));
				fixed f_g2		= itofx(_GetGValue(pixel2));
				fixed f_g3		= itofx(_GetGValue(pixel3));
				fixed f_g4		= itofx(_GetGValue(pixel4));
				fixed f_b1		= itofx(_GetBValue(pixel1));
				fixed f_b2		= itofx(_GetBValue(pixel2));
				fixed f_b3		= itofx(_GetBValue(pixel3));
				fixed f_b4		= itofx(_GetBValue(pixel4));

				BYTE red		= (BYTE)fxtoi(Mulfx(f_w1, f_r1) + Mulfx(f_w2, f_r2) + Mulfx(f_w3, f_r3) + Mulfx(f_w4, f_r4));
				BYTE green		= (BYTE)fxtoi(Mulfx(f_w1, f_g1) + Mulfx(f_w2, f_g2) + Mulfx(f_w3, f_g3) + Mulfx(f_w4, f_g4));
				BYTE blue		= (BYTE)fxtoi(Mulfx(f_w1, f_b1) + Mulfx(f_w2, f_b2) + Mulfx(f_w3, f_b3) + Mulfx(f_w4, f_b4));

				((BYTE*)lpDstData)[dwDstTotalOffset]	= blue; // Blue
				((BYTE*)lpDstData)[dwDstTotalOffset+1]	= green;// Green
				((BYTE*)lpDstData)[dwDstTotalOffset+2]	= red;	// Red
				//lpDstData[dwDstTotalOffset>>2] = _RGB(red, green, blue);

				// Update destination horizontal offset
				dwDstHorizontalOffset += m_iBpp;
				}
			}
		}

	hBmpDest = (HBITMAP)bmDest.Detach();
	return TRUE;
	}

BOOL
_Image::MultiplyBlendImageBpp32(COLORREF crBlend){
	if( IsNull() )
		return FALSE;

	BITMAP bmImage;
	if( GetObject(image_, sizeof(BITMAP), &bmImage) != sizeof(BITMAP) || !bmImage.bmBits )
		return FALSE;

	RGBQUAD	 rgbQuadBlend = 
		{
		((BYTE)(crBlend >> 16)),	// Blue
		((BYTE)(crBlend >> 8)),		// Green
		((BYTE)(crBlend)),			// Red
		0							// Alpha
		};

	RGBQUAD* pixelSrc	= (RGBQUAD*)bmImage.bmBits;
	for( int y=0; y<bmImage.bmHeight; y++ ){
		pixelSrc = (RGBQUAD*)(((long)bmImage.bmBits) + y*bmImage.bmWidthBytes);
		for( int x=0; x<bmImage.bmWidth; x++ ){
			if( pixelSrc->rgbReserved != 0 ){
				pixelSrc->rgbBlue	= pixelSrc->rgbBlue*rgbQuadBlend.rgbBlue / 255;
				pixelSrc->rgbGreen	= pixelSrc->rgbGreen*rgbQuadBlend.rgbGreen / 255;
				pixelSrc->rgbRed	= pixelSrc->rgbRed*rgbQuadBlend.rgbRed / 255;
				
				/*
				pixelSrc->rgbBlue = pixelSrc->rgbBlue + rgbQuadBlend.rgbBlue -  pixelSrc->rgbBlue*rgbQuadBlend.rgbBlue / 255;
				pixelSrc->rgbGreen = pixelSrc->rgbGreen + rgbQuadBlend.rgbGreen - pixelSrc->rgbGreen*rgbQuadBlend.rgbGreen / 255;
				pixelSrc->rgbRed = pixelSrc->rgbRed + rgbQuadBlend.rgbRed - pixelSrc->rgbRed*rgbQuadBlend.rgbRed / 255;
*/
				}

			pixelSrc ++;
			}
		}
	return TRUE;
	}

BOOL
_Image::Rotate90Bitmap32bpp(){
	if( IsNull() )
		return FALSE;
	_Image rotate;
	rotate.Attach(Detach());
	if( rotate.Rotate90Bitmap32bpp(*this) )
		return TRUE;
	else
		Attach(rotate.Detach());
	return FALSE;
	}

BOOL
_Image::Rotate90Bitmap32bpp(_Image& imageRotated){
	if( IsNull() )
		return FALSE;

	BITMAP bmp;
	GetObject(image_, sizeof(BITMAP), &bmp);
	if( bmp.bmBits == NULL || bmp.bmBitsPixel != 32 )
		return FALSE;

	_Image resultImage;
	BITMAP bmpRotated;
	if( !resultImage.CreateDIBBitmap(bmp.bmBitsPixel, RGB(0, 0, 0), bmp.bmHeight, bmp.bmWidth, &bmpRotated) )
		return FALSE;

	RGBQUAD* pixelRotated	= (RGBQUAD*)bmpRotated.bmBits;
	RGBQUAD* pixelSrc		= (RGBQUAD*)bmp.bmBits;

	for( int y=0; y<bmpRotated.bmHeight; y++ ){
		for( int x=0; x<bmpRotated.bmWidth; x++ ){
			pixelSrc = (RGBQUAD*)(((long)bmp.bmBits) + (x)*bmp.bmWidthBytes + y*sizeof(RGBQUAD));

			pixelRotated->rgbBlue		= pixelSrc->rgbBlue;
			pixelRotated->rgbGreen		= pixelSrc->rgbGreen;
			pixelRotated->rgbRed		= pixelSrc->rgbRed;
			pixelRotated->rgbReserved	= pixelSrc->rgbReserved;

			pixelRotated ++;
			}
		}
	imageRotated.Attach((ImageDef)resultImage.Detach());
	return TRUE;
	}

BOOL
_Image::DrawImage32bpp_Omitborder(_DC* pDC, _Rect rcDestDC, _Rect rcClipDC, _Rect rcOmitBorder, BOOL bAlpha, BOOL bSkipMiddleCenterPart){
	// Parameters validation. ===
	if( !pDC || IsNull() )
		return FALSE;

	if( rcClipDC.Width() <= 0 ||
		rcClipDC.Height() <= 0 )
		return FALSE;

	if( rcDestDC.Width() <= 0 ||
		rcDestDC.Height() <= 0 )
		return FALSE;
	
	rcClipDC &= rcDestDC;
	if( rcClipDC.IsRectEmpty() )
		return FALSE;
	// ==========================

	int		nSrcCX		= GetWidth(), nSrcCY = GetHeight();
	bool	bBkImage	= false;

	// Retrieve destination bitmap image. (To draw image on)
	_Image	bmBkgnd;
	HBITMAP hDestDCImage = NULL;

	if( !pDC->GetSelectedDibImageFromDC(bmBkgnd) )
		return FALSE;
	// ####################################################

	_Rect rcTopLeftPart			(rcDestDC.left, rcDestDC.top, rcDestDC.left + rcOmitBorder.left, rcDestDC.top + rcOmitBorder.top);
	_Rect rcTopCenterPart		(rcTopLeftPart.right, rcDestDC.top, rcDestDC.right - rcOmitBorder.right, rcDestDC.top + rcOmitBorder.top);
	_Rect rcTopRightPart		(rcTopCenterPart.right, rcDestDC.top, rcDestDC.right, rcDestDC.top + rcOmitBorder.top);

	_Rect rcBottomLeftPart		(rcDestDC.left, rcDestDC.bottom - rcOmitBorder.bottom, rcDestDC.left + rcOmitBorder.left, rcDestDC.bottom);
	_Rect rcBottomCenterPart	(rcTopLeftPart.right, rcDestDC.bottom - rcOmitBorder.bottom, rcDestDC.right - rcOmitBorder.right, rcDestDC.bottom);
	_Rect rcBottomRightPart		(rcTopCenterPart.right, rcDestDC.bottom - rcOmitBorder.bottom, rcDestDC.right, rcDestDC.bottom);

	_Rect rcMidLeftPart			(rcDestDC.left, rcTopLeftPart.bottom, rcDestDC.left + rcOmitBorder.left, rcBottomLeftPart.top);
	_Rect rcMidCenterPart		(rcTopLeftPart.right, rcTopLeftPart.bottom, rcDestDC.right - rcOmitBorder.right, rcBottomLeftPart.top);
	_Rect rcMidRightPart		(rcTopCenterPart.right, rcTopLeftPart.bottom, rcDestDC.right, rcBottomLeftPart.top);

	_Rect rcTopLeftPartImg		(0, 0, rcOmitBorder.left, rcOmitBorder.top);
	_Rect rcTopCenterPartImg	(rcTopLeftPartImg.right, 0, nSrcCX - rcOmitBorder.right, rcOmitBorder.top);
	_Rect rcTopRightPartImg		(rcTopCenterPartImg.right, 0, nSrcCX, rcOmitBorder.top);

	_Rect rcBottomLeftPartImg	(0, nSrcCY - rcOmitBorder.bottom, rcOmitBorder.left, nSrcCY);
	_Rect rcBottomCenterPartImg	(rcBottomLeftPartImg.right, nSrcCY - rcOmitBorder.bottom, nSrcCX - rcOmitBorder.right, nSrcCY);
	_Rect rcBottomRightPartImg	(rcBottomCenterPartImg.right, nSrcCY - rcOmitBorder.bottom, nSrcCX, nSrcCY);

	_Rect rcMidLeftPartImg		(0, rcTopLeftPartImg.bottom, rcTopLeftPartImg.right, rcBottomLeftPartImg.top);
	_Rect rcMidCenterPartImg		(rcBottomCenterPartImg.left, rcTopLeftPartImg.bottom, rcBottomCenterPartImg.right, rcBottomLeftPartImg.top);
	_Rect rcMidRightPartImg		(rcBottomRightPartImg.left, rcTopLeftPartImg.bottom, rcBottomRightPartImg.right, rcBottomLeftPartImg.top);

	_Rect* pArrRects[]		= {	&rcTopLeftPart, &rcTopCenterPart, &rcTopRightPart, 
								&rcBottomLeftPart, &rcBottomCenterPart, &rcBottomRightPart,
								&rcMidLeftPart, &rcMidCenterPart, &rcMidRightPart};

	_Rect* pArrRectImgs[]	= {	&rcTopLeftPartImg, &rcTopCenterPartImg, &rcTopRightPartImg, 
								&rcBottomLeftPartImg, &rcBottomCenterPartImg, &rcBottomRightPartImg,
								&rcMidLeftPartImg, &rcMidCenterPartImg, &rcMidRightPartImg};

	int nLoop	= 0;
	int nCt		= sizeof(pArrRects) / sizeof(_Rect*);

	if( bSkipMiddleCenterPart ){
		pArrRects[7]	= pArrRects[8];
		pArrRectImgs[7] = pArrRectImgs[8];
		nCt --;
		}

	while( nLoop < nCt ){
		_Rect* pRect	= pArrRects[nLoop];
		_Rect* pRectImg	= pArrRectImgs[nLoop];
		if( !pRect->IsRectEmpty() && !pRectImg->IsRectEmpty() ){
			_Rect rcInvalid = *pRect&rcClipDC;
			if( !rcInvalid.IsRectEmpty() ){
				int xSrc		= pRectImg->left + (rcInvalid.left - pRect->left)%pRectImg->Width();
				int ySrc		= pRectImg->top + (rcInvalid.top - pRect->top)%pRectImg->Height();
				int	xDest		= max(rcInvalid.left, 0);
				int	yDest		= max(rcInvalid.top, 0);

				while( yDest < rcInvalid.bottom ){
					while( xDest < rcInvalid.right ){

						bmBkgnd.BitmapOnBitmap(xDest, yDest, image_, xSrc, ySrc, 
									min(pRectImg->Width(), rcInvalid.Width()), 
									min(pRectImg->Height(), rcInvalid.Height()), 
									min((rcInvalid.right - xDest), (pRectImg->right - xSrc)), 
									min((rcInvalid.bottom - yDest), (pRectImg->bottom - ySrc)), bAlpha);

						xDest	+= min((rcInvalid.right - xDest), (pRectImg->right - xSrc));
						xSrc	= pRectImg->left;
						}
					xDest	= max(rcInvalid.left, 0);
					yDest	+= min((rcInvalid.bottom - yDest), (pRectImg->bottom - ySrc));
					xSrc	= pRectImg->left + (rcInvalid.left - pRect->left)%pRectImg->Width();
					ySrc	= pRectImg->top;
					}
				}
			}

		nLoop ++;
		}

	bmBkgnd.Detach();
	return TRUE;
	}

RGNDef
_Image::GetBitmap32bppRgnByAlphaChannel(BYTE btMinAlpha){
	if( IsNull() )
		return NULL;

	BITMAP bmp;
	if( GetObject(image_, sizeof(BITMAP), &bmp) != sizeof(BITMAP) || !bmp.bmBits )
		return NULL;

	HRGN		hRgnBmp = NULL; //Handle To The Region
	HRGN		hOr     = NULL; 
	
#define MAX_RECT 200			
	HGLOBAL hGlobal		= GlobalAlloc(GMEM_MOVEABLE,sizeof(RGNDATA)+MAX_RECT*sizeof(RECT));
	RGNDATA * pData		= (RGNDATA*)GlobalLock(hGlobal);
	pData->rdh.iType	= RDH_RECTANGLES;
	pData->rdh.dwSize	= sizeof(RGNDATAHEADER);
	SetRect(&pData->rdh.rcBound,MAXLONG,MAXLONG,0,0);
	pData->rdh.nRgnSize = MAX_RECT*sizeof(RECT);
	pData->rdh.nCount	= 0;

	BYTE * p = (BYTE*)bmp.bmBits + (bmp.bmHeight-1)*bmp.bmWidthBytes;
	for(int y = 0; y < bmp.bmHeight; y ++){
		for(int x = 0; x < bmp.bmWidth; x ++){
			RGBQUAD *	pPixel	= &((RGBQUAD*)(p))[x];
			int			x1		= x;
			while( x < bmp.bmWidth ){
				if( pPixel->rgbReserved >= btMinAlpha )
					break;
				x	++;
				pPixel ++;
				}
			if( x > x1 ){
				int nRcCount = pData->rdh.nRgnSize / sizeof(RECT);
				int nCount   = pData->rdh.nCount;
				if( nCount >= nRcCount ){
					pData->rdh.nRgnSize += MAX_RECT*sizeof(RECT);
					GlobalUnlock(hGlobal);
					hGlobal				= GlobalReAlloc(hGlobal,sizeof(RGNDATA)+pData->rdh.nRgnSize,GMEM_MOVEABLE);
					pData				= (RGNDATA*)GlobalLock(hGlobal);
					}

				RECT * pRect = (RECT*)&pData->Buffer;
				SetRect(&pRect[pData->rdh.nCount],x1,y,x,y+1);
				pData->rdh.nCount ++;
						
				if( x1 < pData->rdh.rcBound.left )
					pData->rdh.rcBound.left = x1;
				if( y < pData->rdh.rcBound.top )
					pData->rdh.rcBound.top = y;
				if( x > pData->rdh.rcBound.right )
					pData->rdh.rcBound.right = x;
				if( y+1 > pData->rdh.rcBound.bottom )
					pData->rdh.rcBound.bottom = y+1;
				}
			}
		p -= bmp.bmWidthBytes;
		}

	hRgnBmp = ExtCreateRegion(NULL, sizeof(RGNDATA)+pData->rdh.nRgnSize, pData);
	GlobalFree(hGlobal);

	// Invert region. {{
	HRGN hRgnDest	= CreateRectRgn(0, 0, bmp.bmWidth, bmp.bmHeight);
	if( CombineRgn(hRgnDest, hRgnDest, hRgnBmp, RGN_DIFF) == ERROR ){
		DeleteObject(hRgnDest);
		hRgnDest = 0;
		}
	DeleteObject(hRgnBmp);
	// }}
	return hRgnDest;
	}

BOOL
_Image::SetSoftenFilter(){
	if( IsNull() )
		return FALSE;

	BITMAP bmSoften;
	if( ::GetObject(image_, sizeof(BITMAP), &bmSoften) != sizeof(BITMAP) || !bmSoften.bmBits )
		return FALSE;

	_Image bmpTest;
	if( !bmpTest.CreateDIBBitmap(bmSoften.bmBitsPixel, (UINT*)bmSoften.bmBits, bmSoften.bmWidth, bmSoften.bmHeight) )
		return FALSE;

	BITMAP bmTemp;
	if( ::GetObject((HBITMAP)bmpTest, sizeof(BITMAP), &bmTemp) != sizeof(BITMAP) || !bmTemp.bmBits )
		return FALSE;

	// Basic parameters. {{
	long Ksize		= 3;
	long Kfactor	= 16;
	long Koffset	= 0;
	long kernel[]	={1,1,1,1,8,1,1,1,1};
	// }}

	long k2 = Ksize/2;
	long kmax= Ksize-k2;
	long r,g,b,i;
	long ksumcur,ksumtot;
	RGBQUAD c = {0, 0, 0, 0};

	long xmin,xmax,ymin,ymax;
	xmin = ymin = 0;
	xmax = bmSoften.bmWidth; ymax=bmSoften.bmHeight;

	ksumtot = 0;
	for(long j=-k2;j<kmax;j++){
		for(long k=-k2;k<kmax;k++){
			ksumtot += kernel[(j+k2)+Ksize*(k+k2)];
			}
		}

	int nWidth = bmSoften.bmWidth;
	int nHeight = bmSoften.bmHeight;

	for(long y=ymin; y<ymax; y++){
		for(long x=xmin; x<xmax; x++){
			r=b=g=ksumcur=0;
			for(long j=-k2;j<kmax;j++){
				for(long k=-k2;k<kmax;k++){
					// Is not inside.
					if ( !(0<=(y+k) && (y+k)<nHeight && 0<=(x+k) && (x+k)<nWidth) ) continue;
					if( GetPixel(bmSoften, x+j,y+k, c) ){
						i = kernel[(j+k2)+Ksize*(k+k2)];
						r += c.rgbRed * i;
						g += c.rgbGreen * i;
						b += c.rgbBlue * i;
						ksumcur += i;
						}
					}
				}

			if (Kfactor==0 || ksumcur==0){
				c.rgbRed   = (BYTE)min(255, max(0,(int)(r + Koffset)));
				c.rgbGreen = (BYTE)min(255, max(0,(int)(g + Koffset)));
				c.rgbBlue  = (BYTE)min(255, max(0,(int)(b + Koffset)));
			} else if (ksumtot == ksumcur) {
				c.rgbRed   = (BYTE)min(255, max(0,(int)(r/Kfactor + Koffset)));
				c.rgbGreen = (BYTE)min(255, max(0,(int)(g/Kfactor + Koffset)));
				c.rgbBlue  = (BYTE)min(255, max(0,(int)(b/Kfactor + Koffset)));
			} else {
				c.rgbRed   = (BYTE)min(255, max(0,(int)((r*ksumtot)/(ksumcur*Kfactor) + Koffset)));
				c.rgbGreen = (BYTE)min(255, max(0,(int)((g*ksumtot)/(ksumcur*Kfactor) + Koffset)));
				c.rgbBlue  = (BYTE)min(255, max(0,(int)((b*ksumtot)/(ksumcur*Kfactor) + Koffset)));
				}
			SetPixel(bmTemp, x, y, c);
			}
		}

	int nPixelDataSize = bmTemp.bmWidthBytes*bmTemp.bmHeight;
	memcpy(bmSoften.bmBits, bmTemp.bmBits, nPixelDataSize);
	bmpTest.Destroy();
	return true;
	}

/*
////////////////////////////////////////////////////////////////////////////////
/** 
 * generates a 1-D convolution matrix to be used for each pass of 
 * a two-pass gaussian blur.  Returns the length of the matrix.
 * \author [nipper]
 */
int 
_Image::gen_convolve_matrix (float radius, float **cmatrix_p){
	int matrix_length;
	int matrix_midpoint;
	float* cmatrix;
	int i,j;
	float std_dev;
	float sum;
	
	/* we want to generate a matrix that goes out a certain radius
	* from the center, so we have to go out ceil(rad-0.5) pixels,
	* inlcuding the center pixel.  Of course, that's only in one direction,
	* so we have to go the same amount in the other direction, but not count
	* the center pixel again.  So we double the previous result and subtract
	* one.
	* The radius parameter that is passed to this function is used as
	* the standard deviation, and the radius of effect is the
	* standard deviation * 2.  It's a little confusing.
	* <DP> modified scaling, so that matrix_lenght = 1+2*radius parameter
	*/
	radius = (float)fabs(0.5*radius) + 0.25f;
	
	std_dev = radius;
	radius = std_dev * 2;
	
	/* go out 'radius' in each direction */
	matrix_length = int (2 * ceil(radius-0.5) + 1);
	if (matrix_length <= 0) matrix_length = 1;
	matrix_midpoint = matrix_length/2 + 1;
	*cmatrix_p = new float[matrix_length];
	cmatrix = *cmatrix_p;
	
	/*  Now we fill the matrix by doing a numeric integration approximation
	* from -2*std_dev to 2*std_dev, sampling 50 points per pixel.
	* We do the bottom half, mirror it to the top half, then compute the
	* center point.  Otherwise asymmetric quantization errors will occur.
	*  The formula to integrate is e^-(x^2/2s^2).
	*/
	
	/* first we do the top (right) half of matrix */
	for (i = matrix_length/2 + 1; i < matrix_length; i++){
		float base_x = i - (float)floor((float)(matrix_length/2)) - 0.5f;
		sum = 0;
		for (j = 1; j <= 50; j++){
			if ( base_x+0.02*j <= radius ) 
				sum += (float)exp (-(base_x+0.02*j)*(base_x+0.02*j) / 
				(2*std_dev*std_dev));
		}
		cmatrix[i] = sum/50;
    }
	
	/* mirror the thing to the bottom half */
	for (i=0; i<=matrix_length/2; i++) {
		cmatrix[i] = cmatrix[matrix_length-1-i];
	}
	
	/* find center val -- calculate an odd number of quanta to make it symmetric,
	* even if the center point is weighted slightly higher than others. */
	sum = 0;
	for (j=0; j<=50; j++){
		sum += (float)exp (-(0.5+0.02*j)*(0.5+0.02*j) /
			(2*std_dev*std_dev));
    }
	cmatrix[matrix_length/2] = sum/51;
	
	/* normalize the distribution by scaling the total sum to one */
	sum=0;
	for (i=0; i<matrix_length; i++) sum += cmatrix[i];
	for (i=0; i<matrix_length; i++) cmatrix[i] = cmatrix[i] / sum;
	return matrix_length;
	}
////////////////////////////////////////////////////////////////////////////////
/**
 * generates a lookup table for every possible product of 0-255 and
 * each value in the convolution matrix.  The returned array is
 * indexed first by matrix position, then by input multiplicand (?)
 * value.
 * \author [nipper]
 */
float*
_Image::gen_lookup_table (float *cmatrix, int cmatrix_length){
	float* lookup_table = new float[cmatrix_length * 256];
	float* lookup_table_p = lookup_table;
	float* cmatrix_p      = cmatrix;
	
	for (int i=0; i<cmatrix_length; i++){
		for (int j=0; j<256; j++){
			*(lookup_table_p++) = *cmatrix_p * (float)j;
		}
		cmatrix_p++;
    }
	return lookup_table;
	}
////////////////////////////////////////////////////////////////////////////////
/**
 * this function is written as if it is blurring a column at a time,
 * even though it can operate on rows, too.  There is no difference
 * in the processing of the lines, at least to the blur_line function.
 * \author [nipper]
 */
void 
_Image::blur_line (float *ctable, float *cmatrix, int cmatrix_length, BYTE* cur_col, BYTE* dest_col, int y, long bytes){
	float scale;
	float sum;
	int i=0, j=0;
	int row;
	int cmatrix_middle = cmatrix_length/2;
	
	float *cmatrix_p;
	BYTE  *cur_col_p;
	BYTE  *cur_col_p1;
	BYTE  *dest_col_p;
	float *ctable_p;
	
	/* this first block is the same as the non-optimized version --
	* it is only used for very small pictures, so speed isn't a
	* big concern.
	*/
	if (cmatrix_length > y){
		for (row = 0; row < y ; row++){
			scale=0;
			/* find the scale factor */
			for (j = 0; j < y ; j++){
				/* if the index is in bounds, add it to the scale counter */
				if ((j + cmatrix_middle - row >= 0) &&
					(j + cmatrix_middle - row < cmatrix_length))
					scale += cmatrix[j + cmatrix_middle - row];
				}
			for (i = 0; i<bytes; i++){
				sum = 0;
				for (j = 0; j < y; j++){
					if ((j >= row - cmatrix_middle) &&
						(j <= row + cmatrix_middle))
						sum += cur_col[j*bytes + i] * cmatrix[j];
					}
				dest_col[row*bytes + i] = (BYTE)(0.5f + sum / scale);
				}
			}
		}
	else{
		/* for the edge condition, we only use available info and scale to one */
		for (row = 0; row < cmatrix_middle; row++){
			/* find scale factor */
			scale=0;
			for (j = cmatrix_middle - row; j<cmatrix_length; j++)
				scale += cmatrix[j];
			for (i = 0; i<bytes; i++){
				sum = 0;
				for (j = cmatrix_middle - row; j<cmatrix_length; j++){
					sum += cur_col[(row + j-cmatrix_middle)*bytes + i] * cmatrix[j];
					}
				dest_col[row*bytes + i] = (BYTE)(0.5f + sum / scale);
				}
		}
		/* go through each pixel in each col */
		dest_col_p = dest_col + row*bytes;
		for (; row < y-cmatrix_middle; row++){
			cur_col_p = (row - cmatrix_middle) * bytes + cur_col;
			for (i = 0; i<bytes; i++){
				sum = 0;
				cmatrix_p = cmatrix;
				cur_col_p1 = cur_col_p;
				ctable_p = ctable;
				for (j = cmatrix_length; j>0; j--){
					sum += *(ctable_p + *cur_col_p1);
					cur_col_p1 += bytes;
					ctable_p += 256;
					}
				cur_col_p++;
				*(dest_col_p++) = (BYTE)(0.5f + sum);
				}
			}
		
		/* for the edge condition , we only use available info, and scale to one */
		for (; row < y; row++){
			/* find scale factor */
			scale=0;
			for (j = 0; j< y-row + cmatrix_middle; j++)
				scale += cmatrix[j];
			for (i = 0; i<bytes; i++){
				sum = 0;
				for (j = 0; j<y-row + cmatrix_middle; j++){
					sum += cur_col[(row + j-cmatrix_middle)*bytes + i] * cmatrix[j];
					}
				dest_col[row*bytes + i] = (BYTE) (0.5f + sum / scale);
				}
			}
		}
	}

bool
_Image::Image_SetCol(BITMAP bmDest, int x, BYTE* src_col){
	if( bmDest.bmWidth <= x || x < 0 || !bmDest.bmBits )
		return false;
	if( bmDest.bmBitsPixel != 32 ) 
		return false;

	BYTE*		lpPixels	= (BYTE*)bmDest.bmBits;
	RGBQUAD*	pPixelDest	= NULL;
	int			nIndexDest	= 0;
	int			nLoop		= 0;
	int			bypp		= (bmDest.bmBitsPixel >> 3);
	for(int y=0; y<bmDest.bmHeight; y++){
		nIndexDest = x + ((bmDest.bmHeight - y - 1)*bmDest.bmWidth);
		pPixelDest = &((RGBQUAD*)lpPixels)[nIndexDest];

		pPixelDest->rgbBlue		= src_col[nLoop];
		pPixelDest->rgbGreen	= src_col[++nLoop];
		pPixelDest->rgbRed		= src_col[++nLoop];
		pPixelDest->rgbReserved = src_col[++nLoop];
		}

	return true;
	}

bool
_Image::Image_GetCol(BITMAP bmDest, int x, BYTE* src_col){
	if( bmDest.bmWidth <= x || x < 0 || !bmDest.bmBits )
		return false;
	if( bmDest.bmBitsPixel != 32 ) 
		return false;

	BYTE*		lpPixels	= (BYTE*)bmDest.bmBits;
	RGBQUAD*	pPixelDest	= NULL;
	int			nIndexDest	= 0;
	int			nLoop		= 0;
	int			bypp		= (bmDest.bmBitsPixel >> 3);
	for(int y=0; y<bmDest.bmHeight; y++){
		nIndexDest = x + ((bmDest.bmHeight - y - 1)*bmDest.bmWidth);
		pPixelDest = (RGBQUAD*)&lpPixels[nIndexDest];

		src_col[nLoop] = pPixelDest->rgbBlue;
		src_col[++nLoop] = pPixelDest->rgbGreen;
		src_col[++nLoop] = pPixelDest->rgbRed;
		src_col[++nLoop] = pPixelDest->rgbReserved;
		}
	return true;
	}

bool
_Image::Image_SetRow(BITMAP bmDest, int y, BYTE* src_row){
	if( bmDest.bmHeight <= y || y < 0 || !bmDest.bmBits )
		return false;
	if( bmDest.bmBitsPixel != 32 ) 
		return false;

	BYTE*		lpPixels	= (BYTE*)bmDest.bmBits;
	RGBQUAD*	pPixelDest	= NULL;
	int			nIndexDest	= 0;
	int			nLoop		= 0;
	int			bypp		= (bmDest.bmBitsPixel >> 3);
	for(int x=0; x<bmDest.bmWidth; x++){
		nIndexDest = x + ((bmDest.bmHeight - y - 1)*bmDest.bmWidth);
		pPixelDest = &((RGBQUAD*)lpPixels)[nIndexDest];

		pPixelDest->rgbBlue		= src_row[nLoop];
		pPixelDest->rgbGreen	= src_row[++nLoop];
		pPixelDest->rgbRed		= src_row[++nLoop];
		pPixelDest->rgbReserved = src_row[++nLoop];
		}
	return true;
	}

bool
_Image::Image_GetRow(BITMAP bmDest, int y, BYTE* src_row){
	if( bmDest.bmHeight <= y || y < 0 || !bmDest.bmBits )
		return false;
	if( bmDest.bmBitsPixel != 32 ) 
		return false;

	BYTE*		lpPixels	= (BYTE*)bmDest.bmBits;
	RGBQUAD*	pPixelDest	= NULL;
	int			nIndexDest	= 0;
	int			nLoop		= 0;
	int			bypp		= (bmDest.bmBitsPixel >> 3);
	for(int x=0; x<bmDest.bmWidth; x++){
		nIndexDest = x + ((bmDest.bmHeight - y - 1)*bmDest.bmWidth);
		pPixelDest = &((RGBQUAD*)lpPixels)[nIndexDest];

		src_row[nLoop]		= pPixelDest->rgbBlue;
		src_row[++nLoop]	= pPixelDest->rgbGreen;
		src_row[++nLoop]	= pPixelDest->rgbRed;
		src_row[++nLoop]	= pPixelDest->rgbReserved;
		}
	return true;
	}

bool
_Image::GetPixel(BITMAP bmImage, int x, int y, RGBQUAD& c){
	if( !bmImage.bmBits || x >= bmImage.bmWidth || y >= bmImage.bmHeight || x < 0 || y < 0)
		return false;
	int			nIndexDest = x + ((bmImage.bmHeight - y - 1)*bmImage.bmWidth);
	RGBQUAD*	pPixelDest = &((RGBQUAD*)bmImage.bmBits)[nIndexDest];
	/*
	c.rgbBlue	= pPixelDest->rgbBlue;
	c.rgbGreen	= pPixelDest->rgbGreen;
	c.rgbRed	= pPixelDest->rgbRed;
	c.rgbReserved = pPixelDest->rgbReserved;*/
	memcpy(&c, pPixelDest, sizeof(RGBQUAD));
	return true;
	}

bool
_Image::SetPixel(BITMAP bmImage, int x, int y, RGBQUAD& c){
	if( !bmImage.bmBits || x >= bmImage.bmWidth || y >= bmImage.bmHeight || x < 0 || y < 0)
		return false;
	int			nIndexDest = x + ((bmImage.bmHeight - y - 1)*bmImage.bmWidth);
	RGBQUAD*	pPixelDest = &((RGBQUAD*)bmImage.bmBits)[nIndexDest];
	memcpy(pPixelDest, &c, sizeof(RGBQUAD));
	return true;
	}