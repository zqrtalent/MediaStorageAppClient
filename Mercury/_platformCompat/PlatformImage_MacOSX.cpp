#include "PlatformImage_MacOSX.h"
#include "PlatformDeviceContext_MacOSX.h"
#include "PlatformLongBinary.h"
#include "../Path/Path.h"
#include "../Utility/StringHelper.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

_Image::_Image(){
    //image_ = NULL;
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
		return false;/*
    image_ = [[NSBitmapImageRep alloc] initWithCGImage:image];
    if( image_ != NULL ){
        bmpInfo_.bmBitsPixel = image_.bitsPerPixel;
        bmpInfo_.bmWidthBytes = (int)image_.bytesPerRow;
        bmpInfo_.bmWidth = (int)image_.size.width;
        bmpInfo_.bmHeight = (int)image_.size.height;
        bmpInfo_.bmBits = image_.bitmapData;
        return true;
    }*/
    return false;
	}

ImageDef
_Image::Detach(){
	//ASSERT( !IsNull() );
	if( IsNull() )
		return NULL;
    /*ImageDef img = [image_ CGImage];
    image_ = NULL;
    memset(&bmpInfo_, 0, sizeof(BITMAP));
     return img;*/ return nil;
	}

void
_Image::Destroy(){/*
	if( !IsNull() ){
		memset(&bmpInfo_, 0, sizeof(BITMAP));
        image_ = nil;
        }*/
	}

BOOL
_Image::LoadImage32bpp(const _string sImageFile){
	if( !IsNull() ){
		ASSERT(FALSE);
		return FALSE;
		}
    return FALSE;
    /*
	// Initialize gdi plus.
	InitGDIPlus(false);

	ImageDef imgLoaded = NULL;
	wchar_t wszFileName[128];
	StringHelper::UTF8ToUnicode(&sImageFile, wszFileName, 127);

	try{
		Gdiplus::Bitmap bmSrc(wszFileName);
		if( bmSrc.GetLastStatus() != Gdiplus::Ok )
			return FALSE;


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

		Image imgDib;
		if( !imgDib.CreateDIBBitmap(nBPP, RGB(0, 0, 0), bmSrc.GetWidth(), bmSrc.GetHeight(), NULL, NULL) )
			return FALSE;

		if( eDestPixelFormat == eSrcPixelFormat ){
			// The pixel formats are identical, so just memcpy the rows.
			Gdiplus::BitmapData data;
			Gdiplus::_Rect rect( 0, 0, imgDib.GetWidth(), imgDib.GetHeight() );
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
			}
		imgLoaded = imgDib.Detach();
		}
	catch(...){
		return FALSE;
		}

	if( !imgLoaded )
		return FALSE;

	Attach(imgLoaded);*/
	return TRUE;
	}

BOOL
_Image::LoadFromBinary(LongBinary* pBinary){
    if( !IsNull() || !pBinary || !pBinary->GetBinarySize() )
        return FALSE;
    /*
    NSData* pData = [[NSData alloc] initWithBytesNoCopy:pBinary->LockMemory() length:pBinary->GetBinarySize()];
    NSBitmapImageRep* image = [[NSBitmapImageRep alloc] initWithData:pData];
    
    if( image != NULL ){
        image_ = image;
        bmpInfo_.bmBitsPixel = image_.bitsPerPixel;
        bmpInfo_.bmWidthBytes = (int)image_.bytesPerRow;
        bmpInfo_.bmWidth = (int)image_.size.width;
        bmpInfo_.bmHeight = (int)image_.size.height;
        bmpInfo_.bmBits = image.bitmapData;
        return TRUE;
        }*/
    return FALSE;
	}


_Image*
_Image::LoadImage(const _string sImageFile, int nWidth /*= -1*/, int nHeight /*= -1*/, bool bConvert32bpp /*= true*/){
	_string sImageFileFull	= sImageFile;
	_Image*		pImage		= NULL;
/*
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
		Path::BuildFullPath(sImageFileFull);
        
        NSImage* nsImage = [[NSImage alloc] initWithContentsOfFile:[NSString stringWithUTF8String:sImageFileFull.c_str()]];
        if(nsImage == nil)
            return nullptr;
        
        NSRect nsRect = NSMakeRect(0.0, 0.0, [nsImage size].width, [nsImage size].height);
        CGImageRef imageRef = [nsImage CGImageForProposedRect:&nsRect context:nil hints:nil];
        if(imageRef == nil)
            return nullptr;
        
        pImage = new _Image();
        pImage->Attach(imageRef);
        }
                           
	return pImage;*/
    
    /*
     CGFloat width = CGImageGetWidth(imgRef);
     CGFloat height = CGImageGetHeight(imgRef);
     CGSize imageSize = CGSizeMake(width, height);
     CGRect bounds = CGRectMake(0, 0, width, height);
     CGAffineTransform transform = CGAffineTransformIdentity;
     
     
     CGFloat scaleRatio = bounds.size.width / width;
     
     transform = CGAffineTransformMakeScale(-1.0, 1.0);
     transform = CGAffineTransformRotate(transform, M_PI / 2.0);
     
     
     UIGraphicsBeginImageContextWithOptions(bounds.size, FALSE, 0);
     
     CGContextRef context = UIGraphicsGetCurrentContext();
     
     if (orient == UIImageOrientationRight || orient == UIImageOrientationLeft) {
     CGContextScaleCTM(context, -scaleRatio, scaleRatio);
     CGContextTranslateCTM(context, -height, 0);
     }
     else {
     CGContextScaleCTM(context, scaleRatio, -scaleRatio);
     CGContextTranslateCTM(context, 0, -height);
     }
     
     CGContextConcatCTM(context, transform);
     
     CGContextDrawImage(UIGraphicsGetCurrentContext(), CGRectMake(0, 0, width, height), imgRef);
     
     UIImage * scaledImage = UIGraphicsGetImageFromCurrentImageContext();
     UIGraphicsEndImageContext();
     return scaledImage;
     */
    
    return nil;
}

BOOL
_Image::Save(const _string sImageFile){
	return FALSE;
	}

BOOL
_Image::ConvertToBitmap32bpp(){
    /*
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

	Attach(hDib);*/
	return 1;
	}

BOOL
_Image::ConvertToBitmap24bpp(_Image* pCopyInto /*= NULL*/){
    /*
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
	::DeleteDC(memDC);*/
	return TRUE;
	}

BOOL
_Image::CreateDIBBitmap(int nBPP, UINT* pColorTable, UINT nWidth, UINT nHeight, BITMAP* pBmp /*= NULL*/){
    if( !IsNull() ){
        ASSERT(FALSE);
        return FALSE;
    }
//    
//    int bytesPerRow = (nBPP/8)*(nWidth);
//    // Align by 4 bytes
//    if( nBPP != 32 )
//        bytesPerRow += bytesPerRow%4;
//    
//    CGColorSpaceRef colorSpaceRef = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
//    CGDataProviderRef dataProviderRef = CGDataProviderCreateWithData(NULL, pColorTable, (bytesPerRow*nHeight), NULL);
//    CGBitmapInfo bitmapInfo = kCGImageAlphaLast;
//    
//    ImageDef image = CGImageCreate(nWidth, nHeight, 8, nBPP, bytesPerRow, colorSpaceRef, bitmapInfo, dataProviderRef,
//                                   NULL, true, kCGRenderingIntentDefault);
//    
//    
//    CGDataProviderRelease(dataProviderRef);
//    CGColorSpaceRelease(colorSpaceRef);
//    
//    if( Attach(image) ){
//        if( pBmp ){
//            memcpy(pBmp, &bmpInfo_, sizeof(BITMAP));
//        }
//        return TRUE;
//    }
	return FALSE;
	}

BOOL
_Image::CreateDIBBitmap(int nBPP, COLORREF crFill, UINT nWidth, UINT nHeight, BITMAP* pBmp /*= NULL*/, _DC* pDCCompatible /*= NULL*/){
    /*
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

	Attach(hDib);*/
	return 1;
	}

BOOL
_Image::GetImageFromImage(_Rect rcDest, _Image& image){/*
	if( IsNull() || rcDest.IsRectEmpty() )
		return FALSE;
	
	BITMAP bmImage;
	::GetObject(image_, sizeof(BITMAP), &bmImage);

	Image bmpImage;
	if( !bmpImage.CreateDIBBitmap((int)bmImage.bmBitsPixel, RGB(0, 0, 0), rcDest.Width(), rcDest.Height()) )
		return FALSE;

	BITMAP bmImageDest;
	::GetObject(bmpImage, sizeof(BITMAP), &bmImageDest);
	bmpImage.BitmapOnBitmap(0, 0, image_, rcDest.left, rcDest.top, bmImage.bmWidth, bmImage.bmHeight, rcDest.Width(), rcDest.Height()); 
	image.Attach((HBITMAP)bmpImage.Detach());*/
	return TRUE;
	}

BOOL
_Image::BitmapOnBitmap(BITMAP* pDestBmImage, int xDest, int yDest, BITMAP* pSrcBmImage, int xSrc, int ySrc, int nWidthSrc, int nHeightSrc,
                      int nDrawWidth, int nDrawHeight, BOOL bAlpha /*= FALSE*/, BYTE btAlpha /*= 255*/,
                      BOOL bIgnoreFullAlphaPixel /*= FALSE*/, bool bDontAffectAlphaChannel /*= false*/)
{
    if(pDestBmImage == nullptr || pDestBmImage->bmBits == nullptr ||
       pSrcBmImage == nullptr || pSrcBmImage->bmBits == nullptr)
        return FALSE;
    
    // check parameters validation. ======================================
    BITMAP bmpSrc, bmpDest;
    memcpy(&bmpDest, pDestBmImage, sizeof(BITMAP));
    
    //NSBitmapImageRep* srcBitmapRep = pSrcImage->image_;
    memcpy(&bmpSrc, pSrcBmImage, sizeof(BITMAP));
    if( bmpSrc.bmBits == NULL || bmpDest.bmBits == NULL )
        return FALSE;
    // ===================================================================
    
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
    _Rect rcSrc		(xSrc, ySrc, std::min(xSrc+nWidthSrc, pbmpSrc->bmWidth), std::min(ySrc+nHeightSrc, pbmpSrc->bmHeight));
    
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
                        BYTE*	pPixelSrc	= (BYTE*)(((UINT_PTR)pbmpSrc->bmBits) + pbmpSrc->bmWidthBytes*(pbmpSrc->bmHeight - yLoop - rcSrc.top - 1) + nBPPSrc*rcSrc.left);
                        //BYTE*	pPixelSrc	= (BYTE*)(((UINT_PTR)pbmpSrc->bmBits) + pbmpSrc->bmWidthBytes*(yLoop + rcSrc.top) + nBPPSrc*rcSrc.left);
                        BYTE*	pPixelDest	= (BYTE*)(((UINT_PTR)pbmpDest->bmBits) + pbmpDest->bmWidthBytes*(pbmpDest->bmHeight - yLoop - rcDrawDest.top - 1) + nBPPDest*rcDrawDest.left);
                        //BYTE*	pPixelDest	= (BYTE*)(((UINT_PTR)pbmpDest->bmBits) + pbmpDest->bmWidthBytes*(yLoop + rcDrawDest.top) + nBPPDest*rcDrawDest.left);
                        
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
                    int nIncreaseWidth	 =  std::min(rcSrc.Width(), (rcDest.right - rcDrawDest.right));
                    if( nIncreaseWidth <= 0 ) break;
                    rcDrawDest.left		 = rcDrawDest.right;
                    rcDrawDest.right	+= nIncreaseWidth;
                }
                
                int nIncreaseHeight	 =  std::min(rcSrc.Height(), (rcDest.bottom - rcDrawDest.bottom));
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
                        //BYTE*	pPixelSrc	= (BYTE*)(((UINT_PTR)pbmpSrc->bmBits) + pbmpSrc->bmWidthBytes*(pbmpSrc->bmHeight - yLoop - rcSrc.top - 1) + nBPPSrc*rcSrc.left);
                        BYTE*	pPixelDest	= (BYTE*)(((UINT_PTR)pbmpDest->bmBits) + pbmpDest->bmWidthBytes*(pbmpDest->bmHeight - yLoop - rcDrawDest.top - 1) + nBPPDest*rcDrawDest.left);
                        
                        BYTE*	pPixelSrc	= (BYTE*)(((UINT_PTR)pbmpSrc->bmBits) + pbmpSrc->bmWidthBytes*(yLoop + rcSrc.top) + nBPPSrc*rcSrc.left);
                        //BYTE*	pPixelDest	= (BYTE*)(((UINT_PTR)pbmpDest->bmBits) + pbmpDest->bmWidthBytes*(yLoop + rcDrawDest.top) + nBPPDest*rcDrawDest.left);
                        
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
                    int nIncreaseWidth	 =  std::min(rcSrc.Width(), (rcDest.right - rcDrawDest.right));
                    if( nIncreaseWidth <= 0 ) break;
                    rcDrawDest.left		 = rcDrawDest.right;
                    rcDrawDest.right	+= nIncreaseWidth;
                }
                
                int nIncreaseHeight	 =  std::min(rcSrc.Height(), (rcDest.bottom - rcDrawDest.bottom));
                if( nIncreaseHeight <= 0 ) break;
                rcDrawDest.top		 = rcDrawDest.bottom;
                rcDrawDest.bottom	+= nIncreaseHeight;
                rcDrawDest.left		 = rcDest.left;
                rcDrawDest.right	 = rcDest.left + rcSrc.Width();
            }
        }
    }
    else{
        int nCopyBytesCt =  (bDontAffectAlphaChannel ? std::min(3, std::min(nBPPDest, nBPPSrc)) : std::min(nBPPDest, nBPPSrc));
        while( rcDrawDest.Width() > 0 ){
            while( rcDrawDest.Height() > 0 ){
                // Perform draw operation here. ##########
                for( UINT yLoop = 0; yLoop < rcDrawDest.Height(); yLoop ++ ){
                    BYTE*	pPixelSrc	= (BYTE*)(((UINT_PTR)pbmpSrc->bmBits) + pbmpSrc->bmWidthBytes*(pbmpSrc->bmHeight - yLoop - rcSrc.top - 1) + nBPPSrc*rcSrc.left);
                    BYTE*	pPixelDest	= (BYTE*)(((UINT_PTR)pbmpDest->bmBits) + pbmpDest->bmWidthBytes*(pbmpDest->bmHeight - yLoop - rcDrawDest.top - 1) + nBPPDest*rcDrawDest.left);
                    
                    //BYTE*	pPixelSrc	= (BYTE*)(((UINT_PTR)pbmpSrc->bmBits) + pbmpSrc->bmWidthBytes*(yLoop + rcSrc.top) + nBPPSrc*rcSrc.left);
                    //BYTE*	pPixelDest	= (BYTE*)(((UINT_PTR)pbmpDest->bmBits) + pbmpDest->bmWidthBytes*(yLoop + rcDrawDest.top) + nBPPDest*rcDrawDest.left);
                    
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
                
                int nIncreaseWidth	 =  std::min(rcSrc.Width(), (rcDest.right - rcDrawDest.right));
                if( nIncreaseWidth <= 0 ) break;
                rcDrawDest.left		 = rcDrawDest.right;
                rcDrawDest.right	+= nIncreaseWidth;
            }
            
            int nIncreaseHeight	 =  std::min(rcSrc.Height(), (rcDest.bottom - rcDrawDest.bottom));
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
_Image::BitmapOnBitmap(int xDest, int yDest, _Image* pSrcImage, int xSrc, int ySrc, int nWidthSrc, int nHeightSrc,
						int nDrawWidth, int nDrawHeight, BOOL bAlpha /*= FALSE*/, BYTE btAlpha /*= 255*/,
                      BOOL bIgnoreFullAlphaPixel /*= FALSE*/, bool bDontAffectAlphaChannel /*= false*/)
	{
        if( IsNull() || pSrcImage == nil || bmpInfo_.bmBits == nullptr){
            return FALSE;
        }
        return BitmapOnBitmap(&bmpInfo_, xDest, yDest, &pSrcImage->bmpInfo_, xSrc, ySrc, nWidthSrc, nHeightSrc, nDrawWidth, nDrawHeight, bAlpha, btAlpha, bIgnoreFullAlphaPixel, bDontAffectAlphaChannel);
	}

BOOL
_Image::AlphaColorOnBitmap(int xDest, int yDest, int nDrawWidth, int nDrawHeight, COLORREF crForeground, BYTE btAlpha){
	return TRUE;
	}

_Image*
_Image::CombineImages(_Image* pImageBg, _Image* pImageFg, bool bFgIsAlphaChannelImage, int nCombinedImageBPP /*= 24*/, _Rect rcForeground /*= Rect(0, 0, 0, 0)*/){
    /*
	if( !pImageBg || !pImageFg )
		return NULL;

	Image bmpCombined;
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
		if( szBg == Size(rcForeground.Width(), rcForeground.Height()) ){
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

	Image* pImageComb = new Image();
	pImageComb->Attach((HBITMAP)bmpCombined.Detach());
	return pImageComb;*/
    return NULL;
	}

BOOL
_Image::CopyAlphaChannel(ImageDef hDest, _Rect rcSrc /*= Rect(0, 0, 0, 0)*/, bool bCheckDestPixelAlpha /*= false*/, BYTE btMaxDestPixelAlphaChange /*= 255*/, int nSrcAlphaPixel /*= -1*/){
    /*
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
		}
*/
	return TRUE;
	}

BOOL
_Image::CopyImage(_Image* pCopyInto){
    /*
	if( !pCopyInto || IsNull() )
		return FALSE;
	pCopyInto->Destroy();
    return pCopyInto->Attach([image_ CGImage]);*/
    return TRUE;
	//return pCopyInto->CreateDIBBitmap(GetBPP(),  (UINT*)this->bmpInfo_.bmBits, GetWidth(), GetHeight());
	}

BOOL
_Image::EncodeImageData(LongBinary* pBinaryInto, ImageDataFormatType formatType){
    /*
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
	return bRet;*/
    return false;
	}

ImageDef
_Image::CombineTwo32bppImages(ImageDef hImage1, ImageDef hImage2){
    /*
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

	Image imageCombined;
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

	return (HBITMAP)imageCombined.Detach();*/
    return 0;
	}

BOOL
_Image::RenderImage_OmitBorder(_DC* pDC, const _Rect rcClipDestDC, const _Rect rcDestDC, _Rect rcOmitBorder, BOOL bAlpha /*= TRUE*/, BOOL bSkipMiddleCenterPart /*= FALSE*/){
    
    if( !pDC || IsNull() )
		return FALSE;
/*
	int		nImageWidth		= GetWidth();
	int		nImageHeight	= GetHeight();

	int		nSrcImageX		= (rcClipDestDC.left - rcDestDC.left);
	int		nSrcImageY		= (rcClipDestDC.top - rcDestDC.top);
	int		nImageDrawCX	= rcClipDestDC.Width();
	int		nImageDrawCY	= rcClipDestDC.Height();
*/
	return DrawImage32bpp_Omitborder(pDC, rcDestDC, rcClipDestDC, &rcOmitBorder, bAlpha, bSkipMiddleCenterPart); 
	}

BOOL
_Image::RenderImage(_DC* pDC, const _Rect rcClipDestDC, const _Rect rcDestDC, bool bScale /*= true*/, bool bUseAlphaChannel /*= true*/,
				ScaleMode mode /*= ScaleMode::ScaleMode_NearestNeghbore*/, ImageDef* pImageRendered /*= NULL*/, int nAlphaConstant /*= 255*/, bool bDontAffectAlphaChannel /*= false*/)
	{
        if( IsNull() )
            return FALSE;
        
        CGRect rc = CGRectMake((float)rcClipDestDC.left, (float)rcClipDestDC.top,
                               (float)(rcClipDestDC.right-rcClipDestDC.left), (float)(rcClipDestDC.bottom-rcClipDestDC.top));
       /*
        DCDef dc = *pDC;
        CGContextSaveGState(dc);
        CGContextTranslateCTM(dc, 0, image_.size.height);
        CGContextScaleCTM(dc, 1.0, -1.0);
        CGContextDrawImage(dc, rc, [image_ CGImage]);
        CGContextRestoreGState(dc);
        //CGContextDrawTiledImage(*pDC, rc, [image_ CGImage]);*/
        return TRUE;
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
	return TRUE;
	}

BOOL
_Image::ScaleBitmap_Bilinier(UINT nDestCX, UINT nDestCY, _Rect rcInvalidDest, ImageDef& hBmpDest, bool bUseAlphaChannel, BYTE btConstantAlpha, bool bDontAffectAlphaChannel){
	return TRUE;
	}

BOOL
_Image::MultiplyBlendImageBpp32(COLORREF crBlend){
	if( IsNull() )
		return FALSE;

	BITMAP bmImage;
    memcpy(&bmImage, &bmpInfo_, sizeof(BITMAP));
	if(  bmImage.bmBits == nullptr)
		return FALSE;
    
    
    struct RGBQUAD
    {
        #pragma pack(1)
        BYTE rgbBlue;
        BYTE rgbGreen;
        BYTE rgbRed;
        BYTE rgbReserved;
    };

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
    /*
	if( IsNull() )
		return FALSE;

	BITMAP bmp;
	GetObject(image_, sizeof(BITMAP), &bmp);
	if( bmp.bmBits == NULL || bmp.bmBitsPixel != 32 )
		return FALSE;

	Image resultImage;
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
	imageRotated.Attach((ImageDef)resultImage.Detach());*/
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
    BITMAP bmInfo;
	pDC->GetDibImageInfo(bmInfo);
	if( bmInfo.bmBits == nullptr )
		return FALSE;
	// ####################################################

	_Rect rcTopLeftPart			(rcDestDC.left, rcDestDC.top, rcDestDC.left + rcOmitBorder.left, rcDestDC.top + rcOmitBorder.top);
	_Rect rcTopCenterPart		(rcTopLeftPart.right, rcDestDC.top, rcDestDC.right - rcOmitBorder.right, rcDestDC.top + rcOmitBorder.top);
	_Rect rcTopRightPart			(rcTopCenterPart.right, rcDestDC.top, rcDestDC.right, rcDestDC.top + rcOmitBorder.top);

	_Rect rcBottomLeftPart		(rcDestDC.left, rcDestDC.bottom - rcOmitBorder.bottom, rcDestDC.left + rcOmitBorder.left, rcDestDC.bottom);
	_Rect rcBottomCenterPart		(rcTopLeftPart.right, rcDestDC.bottom - rcOmitBorder.bottom, rcDestDC.right - rcOmitBorder.right, rcDestDC.bottom);
	_Rect rcBottomRightPart		(rcTopCenterPart.right, rcDestDC.bottom - rcOmitBorder.bottom, rcDestDC.right, rcDestDC.bottom);

	_Rect rcMidLeftPart			(rcDestDC.left, rcTopLeftPart.bottom, rcDestDC.left + rcOmitBorder.left, rcBottomLeftPart.top);
	_Rect rcMidCenterPart		(rcTopLeftPart.right, rcTopLeftPart.bottom, rcDestDC.right - rcOmitBorder.right, rcBottomLeftPart.top);
	_Rect rcMidRightPart			(rcTopCenterPart.right, rcTopLeftPart.bottom, rcDestDC.right, rcBottomLeftPart.top);

	_Rect rcTopLeftPartImg		(0, 0, rcOmitBorder.left, rcOmitBorder.top);
	_Rect rcTopCenterPartImg		(rcTopLeftPartImg.right, 0, nSrcCX - rcOmitBorder.right, rcOmitBorder.top);
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
	int nCt		= sizeof(pArrRects) / sizeof(Rect*);

	if( bSkipMiddleCenterPart ){
		pArrRects[7]	= pArrRects[8];
		pArrRectImgs[7] = pArrRectImgs[8];
		nCt --;
		}
    
	while( nLoop < nCt ){
		_Rect* pRect		= pArrRects[nLoop];
		_Rect* pRectImg	= pArrRectImgs[nLoop];
		if( !pRect->IsRectEmpty() && !pRectImg->IsRectEmpty() ){
			_Rect rcInvalid = *pRect&rcClipDC;
			if( !rcInvalid.IsRectEmpty() && pRectImg->Width() > 0 && pRectImg->Height() > 0 ){
				int xSrc		= pRectImg->left + (rcInvalid.left - pRect->left)%pRectImg->Width();
				int ySrc		= pRectImg->top + (rcInvalid.top - pRect->top)%pRectImg->Height();
                int	xDest		= std::max(rcInvalid.left, 0);
                int	yDest		= std::max(rcInvalid.top, 0);

				while( yDest < rcInvalid.bottom ){
					while( xDest < rcInvalid.right ){

						BitmapOnBitmap(&bmInfo, xDest, yDest, &bmpInfo_, xSrc, ySrc,
                                       std::min(pRectImg->Width(), rcInvalid.Width()),
                                               std::min(pRectImg->Height(), rcInvalid.Height()),
                                               std::min((rcInvalid.right - xDest), (pRectImg->right - xSrc)),
                                               std::min((rcInvalid.bottom - yDest), (pRectImg->bottom - ySrc)), bAlpha);

                        xDest	+= std::min((rcInvalid.right - xDest), (pRectImg->right - xSrc));
						xSrc	= pRectImg->left;
						}
                    xDest	= std::max(rcInvalid.left, 0);
                    yDest	+= std::min((rcInvalid.bottom - yDest), (pRectImg->bottom - ySrc));
					xSrc	= pRectImg->left + (rcInvalid.left - pRect->left)%pRectImg->Width();
					ySrc	= pRectImg->top;
					}
				}
			}

		nLoop ++;
		}
	return TRUE;
	}

RGNDef
_Image::GetBitmap32bppRgnByAlphaChannel(BYTE btMinAlpha){
    /*
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
	return hRgnDest;*/
    return 0;
	}

BOOL
_Image::SetSoftenFilter(){
    /*
	if( IsNull() )
		return FALSE;

	BITMAP bmSoften;
	if( ::GetObject(image_, sizeof(BITMAP), &bmSoften) != sizeof(BITMAP) || !bmSoften.bmBits )
		return FALSE;

	Image bmpTest;
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
	bmpTest.Destroy();*/
	return true;
	}

/*
 //
 //  UIImage+Aanbieding.m
 //  Aanbieding
 //
 //  Created by Daniel Salber on 6/11/13.
 //  Copyright (c) 2013 mackey.nl. All rights reserved.
 //
 
 #import "UIImage+Aanbieding.h"
 
 @implementation UIImage (Aanbieding)
 
 // compose two images
 - (UIImage *)overlayImage:(UIImage *)image atPoint:(CGPoint)point
 {
	UIImage * result = nil;
	
	// note: this uses the current image size and will crop image if necessary
 UIGraphicsBeginImageContextWithOptions(self.size, FALSE, 0);
	
	[self drawAtPoint:CGPointZero];
	[image drawAtPoint:point];
	
	result = UIGraphicsGetImageFromCurrentImageContext();
	
	UIGraphicsEndImageContext();
	
	return result;
 }
 
 // compose two images centered
 - (UIImage *)overlayCenteredImage:(UIImage *)image
 {
	CGPoint overlayOrigin = CGPointMake(fabs(self.size.width - image.size.width)/2, fabs(self.size.height - image.size.height)/2);
	return [self overlayImage:image atPoint:overlayOrigin];
 }
 
 // scale down receiver to maxResolution pixels on longest side
 - (UIImage *)scaleToMaxResolution:(NSUInteger)maxResolution
 {
	
 CGImageRef imgRef = self.CGImage;
	
 CGFloat width = CGImageGetWidth(imgRef);
 CGFloat height = CGImageGetHeight(imgRef);
 CGSize imageSize = CGSizeMake(width, height);
 CGRect bounds = CGRectMake(0, 0, width, height);
 CGAffineTransform transform = CGAffineTransformIdentity;
 
 if (width > maxResolution || height > maxResolution) {
 CGFloat ratio = width/height;
 if (ratio > 1) {
 bounds.size.width = maxResolution;
 bounds.size.height = bounds.size.width / ratio;
 }
 else {
 bounds.size.height = maxResolution;
 bounds.size.width = bounds.size.height * ratio;
 }
 }
	
 CGFloat scaleRatio = bounds.size.width / width;
 
 UIImageOrientation orient = self.imageOrientation;
 CGFloat boundHeight;
 switch(orient) {
 
 case UIImageOrientationUp: //EXIF = 1
 transform = CGAffineTransformIdentity;
 break;
 
 case UIImageOrientationUpMirrored: //EXIF = 2
 transform = CGAffineTransformMakeTranslation(imageSize.width, 0.0);
 transform = CGAffineTransformScale(transform, -1.0, 1.0);
 break;
 
 case UIImageOrientationDown: //EXIF = 3
 transform = CGAffineTransformMakeTranslation(imageSize.width, imageSize.height);
 transform = CGAffineTransformRotate(transform, M_PI);
 break;
 
 case UIImageOrientationDownMirrored: //EXIF = 4
 transform = CGAffineTransformMakeTranslation(0.0, imageSize.height);
 transform = CGAffineTransformScale(transform, 1.0, -1.0);
 break;
 
 case UIImageOrientationLeftMirrored: //EXIF = 5
 boundHeight = bounds.size.height;
 bounds.size.height = bounds.size.width;
 bounds.size.width = boundHeight;
 transform = CGAffineTransformMakeTranslation(imageSize.height, imageSize.width);
 transform = CGAffineTransformScale(transform, -1.0, 1.0);
 transform = CGAffineTransformRotate(transform, 3.0 * M_PI / 2.0);
 break;
 
 case UIImageOrientationLeft: //EXIF = 6
 boundHeight = bounds.size.height;
 bounds.size.height = bounds.size.width;
 bounds.size.width = boundHeight;
 transform = CGAffineTransformMakeTranslation(0.0, imageSize.width);
 transform = CGAffineTransformRotate(transform, 3.0 * M_PI / 2.0);
 break;
 
 case UIImageOrientationRightMirrored: //EXIF = 7
 boundHeight = bounds.size.height;
 bounds.size.height = bounds.size.width;
 bounds.size.width = boundHeight;
 transform = CGAffineTransformMakeScale(-1.0, 1.0);
 transform = CGAffineTransformRotate(transform, M_PI / 2.0);
 break;
 
 case UIImageOrientationRight: //EXIF = 8
 boundHeight = bounds.size.height;
 bounds.size.height = bounds.size.width;
 bounds.size.width = boundHeight;
 transform = CGAffineTransformMakeTranslation(imageSize.height, 0.0);
 transform = CGAffineTransformRotate(transform, M_PI / 2.0);
 break;
 
 default:
 //            [NSException raise:NSInternalInconsistencyException format:@"Invalid image orientation"];
 break;
 
 }
	
 UIGraphicsBeginImageContextWithOptions(bounds.size, FALSE, 0);
 
 CGContextRef context = UIGraphicsGetCurrentContext();
	
 if (orient == UIImageOrientationRight || orient == UIImageOrientationLeft) {
 CGContextScaleCTM(context, -scaleRatio, scaleRatio);
 CGContextTranslateCTM(context, -height, 0);
 }
 else {
 CGContextScaleCTM(context, scaleRatio, -scaleRatio);
 CGContextTranslateCTM(context, 0, -height);
 }
	
 CGContextConcatCTM(context, transform);
	
	CGContextDrawImage(UIGraphicsGetCurrentContext(), CGRectMake(0, 0, width, height), imgRef);
	
 UIImage * scaledImage = UIGraphicsGetImageFromCurrentImageContext();
	
 UIGraphicsEndImageContext();
	
 return scaledImage;
 }
 
 // scale down receiver to minResolution pixels on shortest side
 - (UIImage *)scaleToMinResolution:(NSUInteger)minResolution
 {
	
 CGImageRef imgRef = self.CGImage;
	
 CGFloat width = CGImageGetWidth(imgRef);
 CGFloat height = CGImageGetHeight(imgRef);
 CGSize imageSize = CGSizeMake(width, height);
 CGRect bounds = CGRectMake(0, 0, width, height);
 CGAffineTransform transform = CGAffineTransformIdentity;
 
 if (width > minResolution || height > minResolution) {
 CGFloat ratio = width/height;
 if (ratio < 1) {
 bounds.size.width = minResolution;
 bounds.size.height = bounds.size.width / ratio;
 }
 else {
 bounds.size.height = minResolution;
 bounds.size.width = bounds.size.height * ratio;
 }
 }
	
 CGFloat scaleRatio = bounds.size.width / width;
 
 UIImageOrientation orient = self.imageOrientation;
 CGFloat boundHeight;
 switch(orient) {
 
 case UIImageOrientationUp: //EXIF = 1
 transform = CGAffineTransformIdentity;
 break;
 
 case UIImageOrientationUpMirrored: //EXIF = 2
 transform = CGAffineTransformMakeTranslation(imageSize.width, 0.0);
 transform = CGAffineTransformScale(transform, -1.0, 1.0);
 break;
 
 case UIImageOrientationDown: //EXIF = 3
 transform = CGAffineTransformMakeTranslation(imageSize.width, imageSize.height);
 transform = CGAffineTransformRotate(transform, M_PI);
 break;
 
 case UIImageOrientationDownMirrored: //EXIF = 4
 transform = CGAffineTransformMakeTranslation(0.0, imageSize.height);
 transform = CGAffineTransformScale(transform, 1.0, -1.0);
 break;
 
 case UIImageOrientationLeftMirrored: //EXIF = 5
 boundHeight = bounds.size.height;
 bounds.size.height = bounds.size.width;
 bounds.size.width = boundHeight;
 transform = CGAffineTransformMakeTranslation(imageSize.height, imageSize.width);
 transform = CGAffineTransformScale(transform, -1.0, 1.0);
 transform = CGAffineTransformRotate(transform, 3.0 * M_PI / 2.0);
 break;
 
 case UIImageOrientationLeft: //EXIF = 6
 boundHeight = bounds.size.height;
 bounds.size.height = bounds.size.width;
 bounds.size.width = boundHeight;
 transform = CGAffineTransformMakeTranslation(0.0, imageSize.width);
 transform = CGAffineTransformRotate(transform, 3.0 * M_PI / 2.0);
 break;
 
 case UIImageOrientationRightMirrored: //EXIF = 7
 boundHeight = bounds.size.height;
 bounds.size.height = bounds.size.width;
 bounds.size.width = boundHeight;
 transform = CGAffineTransformMakeScale(-1.0, 1.0);
 transform = CGAffineTransformRotate(transform, M_PI / 2.0);
 break;
 
 case UIImageOrientationRight: //EXIF = 8
 boundHeight = bounds.size.height;
 bounds.size.height = bounds.size.width;
 bounds.size.width = boundHeight;
 transform = CGAffineTransformMakeTranslation(imageSize.height, 0.0);
 transform = CGAffineTransformRotate(transform, M_PI / 2.0);
 break;
 
 default:
 //            [NSException raise:NSInternalInconsistencyException format:@"Invalid image orientation"];
 break;
 
 }
	
 UIGraphicsBeginImageContextWithOptions(bounds.size, FALSE, 0);
 
 CGContextRef context = UIGraphicsGetCurrentContext();
	
 if (orient == UIImageOrientationRight || orient == UIImageOrientationLeft) {
 CGContextScaleCTM(context, -scaleRatio, scaleRatio);
 CGContextTranslateCTM(context, -height, 0);
 }
 else {
 CGContextScaleCTM(context, scaleRatio, -scaleRatio);
 CGContextTranslateCTM(context, 0, -height);
 }
	
 CGContextConcatCTM(context, transform);
	
	CGContextDrawImage(UIGraphicsGetCurrentContext(), CGRectMake(0, 0, width, height), imgRef);
	
 UIImage * scaledImage = UIGraphicsGetImageFromCurrentImageContext();
	
 UIGraphicsEndImageContext();
	
 return scaledImage;
 }
 
 
 @end
 */
