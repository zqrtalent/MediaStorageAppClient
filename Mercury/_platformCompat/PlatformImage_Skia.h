#if !defined(PLATFORMIMAGE_H_INCLUDED)
#define      PLATFORMIMAGE_H_INCLUDED

#include "PlatformCompat.h"
#include "PlatformSize.h"
#include "PlatformRect.h"

// Mutex/Synchronization
#include "PlatformCriticalSection_Win.h"
#include "PlatformFont_Skia.h"
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

const double dAlpha[] =
{
	0.000000, 0.003922, 0.007843, 0.011765, 0.015686, 0.019608, 0.023529, 0.027451, 0.031373, 0.035294, 0.039216, 0.043137, 0.047059, 0.050980, 0.054902, 0.058824,
	0.062745, 0.066667, 0.070588, 0.074510, 0.078431, 0.082353, 0.086275, 0.090196, 0.094118, 0.098039, 0.101961, 0.105882, 0.109804, 0.113725, 0.117647, 0.121569,
	0.125490, 0.129412, 0.133333, 0.137255, 0.141176, 0.145098, 0.149020, 0.152941, 0.156863, 0.160784, 0.164706, 0.168627, 0.172549, 0.176471, 0.180392, 0.184314,
	0.188235, 0.192157, 0.196078, 0.200000, 0.203922, 0.207843, 0.211765, 0.215686, 0.219608, 0.223529, 0.227451, 0.231373, 0.235294, 0.239216, 0.243137, 0.247059,
	0.250980, 0.254902, 0.258824, 0.262745, 0.266667, 0.270588, 0.274510, 0.278431, 0.282353, 0.286275, 0.290196, 0.294118, 0.298039, 0.301961, 0.305882, 0.309804,
	0.313725, 0.317647, 0.321569, 0.325490, 0.329412, 0.333333, 0.337255, 0.341176, 0.345098, 0.349020, 0.352941, 0.356863, 0.360784, 0.364706, 0.368627, 0.372549,
	0.376471, 0.380392, 0.384314, 0.388235, 0.392157, 0.396078, 0.400000, 0.403922, 0.407843, 0.411765, 0.415686, 0.419608, 0.423529, 0.427451, 0.431373, 0.435294,
	0.439216, 0.443137, 0.447059, 0.450980, 0.454902, 0.458824, 0.462745, 0.466667, 0.470588, 0.474510, 0.478431, 0.482353, 0.486275, 0.490196, 0.494118, 0.498039,
	0.501961, 0.505882, 0.509804, 0.513725, 0.517647, 0.521569, 0.525490, 0.529412, 0.533333, 0.537255, 0.541176, 0.545098, 0.549020, 0.552941, 0.556863, 0.560784,
	0.564706, 0.568627, 0.572549, 0.576471, 0.580392, 0.584314, 0.588235, 0.592157, 0.596078, 0.600000, 0.603922, 0.607843, 0.611765, 0.615686, 0.619608, 0.623529,
	0.627451, 0.631373, 0.635294, 0.639216, 0.643137, 0.647059, 0.650980, 0.654902, 0.658824, 0.662745, 0.666667, 0.670588, 0.674510, 0.678431, 0.682353, 0.686275,
	0.690196, 0.694118, 0.698039, 0.701961, 0.705882, 0.709804, 0.713725, 0.717647, 0.721569, 0.725490, 0.729412, 0.733333, 0.737255, 0.741176, 0.745098, 0.749020,
	0.752941, 0.756863, 0.760784, 0.764706, 0.768627, 0.772549, 0.776471, 0.780392, 0.784314, 0.788235, 0.792157, 0.796078, 0.800000, 0.803922, 0.807843, 0.811765,
	0.815686, 0.819608, 0.823529, 0.827451, 0.831373, 0.835294, 0.839216, 0.843137, 0.847059, 0.850980, 0.854902, 0.858824, 0.862745, 0.866667, 0.870588, 0.874510,
	0.878431, 0.882353, 0.886275, 0.890196, 0.894118, 0.898039, 0.901961, 0.905882, 0.909804, 0.913725, 0.917647, 0.921569, 0.925490, 0.929412, 0.933333, 0.937255,
	0.941176, 0.945098, 0.949020, 0.952941, 0.956863, 0.960784, 0.964706, 0.968627, 0.972549, 0.976471, 0.980392, 0.984314, 0.988235, 0.992157, 0.996078, 1.000000
};

// Skia
#include "SkData.h"
#include "SkImage.h"
#include "SkStream.h"
#include "SkSurface.h"
#include "SkCanvas.h"

#ifndef PI
 #define PI 3.141592653589793f
#endif

#define _PI	3.1415926f											// Value of PI
#define _BITS_PER_PIXEL_32	32									// 32-bit color depth
#define _BITS_PER_PIXEL_24	24									// 24-bit color depth
#define _PIXEL	DWORD											// Pixel
#define _RGB(r,g,b)	(((r) << 16) | ((g) << 8) | (b))			// Convert to RGB
#define _GetRValue(c)	((BYTE)(((c) & 0x00FF0000) >> 16))		// Red color component
#define _GetGValue(c)	((BYTE)(((c) & 0x0000FF00) >> 8))		// Green color component
#define _GetBValue(c)	((BYTE)((c) & 0x000000FF))				// Blue color component
#define _GetAValue(c)	((BYTE)(((c) & 0xFF000000) >> 24))		// Alpha color component
#define _NOISE_WIDTH	192
#define _NOISE_HEIGHT	192
#define _NOISE_DEPTH	64

typedef long fixed;												// Our new fixed point type
#define itofx(x) ((x) << 8)										// Integer to fixed point
#define ftofx(x) (long)((x) * 256)								// Float to fixed point
#define dtofx(x) (long)((x) * 256)								// Double to fixed point
#define fxtoi(x) ((x) >> 8)										// Fixed point to integer
#define fxtof(x) ((float) (x) / 256)							// Fixed point to float
#define fxtod(x) ((double)(x) / 256)							// Fixed point to double
#define Mulfx(x,y) (((x) * (y)) >> 8)							// Multiply a fixed by a fixed
#define Divfx(x,y) (((x) << 8) / (y))							// Divide a fixed by a fixed

enum ScaleMode{
	ScaleMode_NearestNeghbore = 1,
	ScaleMode_Bilinier
	};

class _DC;
class LongBinary;
class _Image
{
public:
	_Image();
	_Image(ImageDef img);
	virtual ~_Image();

	enum ImageDataFormatType
	{
		ImageAsPNG  = 0,
		ImageAsJPG,
		ImageAsBMP,
	};

	operator ImageDef						(){ return image_; };
	bool			Attach					(ImageDef image);
	ImageDef		Detach					();
	void			Destroy					();

	BOOL			LoadImage32bpp			(const std::string sImageFile);
	BOOL			LoadFromBinary			(LongBinary* pBinary);
	BOOL			LoadImageFromStream		(LPSTREAM lpStream);
	static	_Image*	LoadImage				(const std::string sImageFile, int nWidth = -1, int nHeight = -1, bool bConvert32bpp = true);
	BOOL			Save					(const std::string sImageFile);

	BOOL			ConvertToBitmap32bpp	();
	BOOL			ConvertToBitmap24bpp	(_Image* pCopyInto = NULL);

	BOOL			CreateDIBBitmap			(int nBPP, UINT* pColorTable, UINT nWidth, UINT nHeight, BITMAP* pBmp = NULL);
	BOOL			CreateDIBBitmap			(int nBPP, COLORREF crFill, UINT nWidth, UINT nHeight, BITMAP* pBmp = NULL, _DC* pDCCompatible = NULL);
	BOOL			GetImageFromImage		(_Rect rcDest, _Image& image);

	BOOL			BitmapOnBitmap			(int xDest, int yDest, ImageDef hBmpSrc, int xSrc, int ySrc, int nWidthSrc, int nHeightSrc, 
											int nDrawWidth, int nDrawHeight, BOOL bAlpha = FALSE, BYTE btAlpha = 255, BOOL bIgnoreFullAlphaPixel = FALSE, bool bDontAffectAlphaChannel = false);

	BOOL			AlphaColorOnBitmap		(int xDest, int yDest, int nDrawWidth, int nDrawHeight, COLORREF crForeground, BYTE btAlpha);

	static _Image*	CombineImages			(_Image* pImageBg, _Image* pImageFg, bool bFgIsAlphaChannelImage, int nCombinedImageBPP = 24, _Rect rcForeground = _Rect(0, 0, 0, 0));
	BOOL			CopyAlphaChannel		(ImageDef hDest, _Rect rcSrc = _Rect(0, 0, 0, 0), bool bCheckDestPixelAlpha = false, BYTE btMaxDestPixelAlphaChange = 255, int nSrcAlphaPixel = -1);

	BOOL			CopyImage				(_Image* pCopyInto);
	BOOL			EncodeImageData			(LongBinary* pBinaryInto, ImageDataFormatType formatType);

	//Desc:  Renders one image on another and calculates alpha channel of combined image.
	static ImageDef CombineTwo32bppImages	(ImageDef hImage1, ImageDef hImage2);

	BOOL			RenderImage_OmitBorder	(_DC* pDC, const _Rect rcClipDestDC, const _Rect rcDestDC, _Rect rcOmitBorder, BOOL bAlpha = TRUE, BOOL bSkipMiddleCenterPart = FALSE);
	BOOL			RenderImage				(_DC* pDC, const _Rect rcClipDestDC, const _Rect rcDestDC, bool bScale = true, bool bUseAlphaChannel = true, 
											ScaleMode mode = ScaleMode::ScaleMode_NearestNeghbore, ImageDef* pImageRendered = NULL, int nAlphaConstant = 255, bool bDontAffectAlphaChannel = false);

	// Image scaling.
	BOOL			ScaleBitmap				(ScaleMode mode, UINT nDestCX, UINT nDestCY, _Rect rcInvalidDest, ImageDef& hBmpDest, bool bUseAlphaChannel, BYTE btConstantAlpha = 255, bool bDontAffectAlphaChannel = false);
	BOOL			ScaleBitmap_NearestN	(UINT nDestCX, UINT nDestCY, _Rect rcInvalidDest, ImageDef& hBmpDest, bool bUseAlphaChannel, BYTE btConstantAlpha = 255, bool bDontAffectAlphaChannel = false);
	BOOL			ScaleBitmap_Bilinier	(UINT nDestCX, UINT nDestCY, _Rect rcInvalidDest, ImageDef& hBmpDest, bool bUseAlphaChannel, BYTE btConstantAlpha = 255, bool bDontAffectAlphaChannel = false);

	BOOL			MultiplyBlendImageBpp32	(COLORREF crBlend);
	BOOL			Rotate90Bitmap32bpp		(_Image& imageRotated);
	BOOL			Rotate90Bitmap32bpp		();

	BOOL			DrawImage32bpp_Omitborder(_DC* pDC, _Rect rcDestDC, _Rect rcClipDC, _Rect rcOmitBorder, BOOL bAlpha = TRUE, BOOL bSkipMiddleCenterPart = FALSE);

	// Region from bitmap.
	RGNDef			GetBitmap32bppRgnByAlphaChannel(BYTE btMinAlpha = 255);

	// Soften filter.
	//BOOL			SetSoftenFilter			();

	// Attributes.
	bool			IsNull					(){return (image_ == NULL);};
	int				GetWidth				(){return bmpInfo_.bmWidth;};
	int				GetHeight				(){return bmpInfo_.bmHeight;};
	int				GetBPP					(){return (int)bmpInfo_.bmBitsPixel;};
	const void*		GetBits					(){return (const void*)bmpInfo_.bmBits;};
	const void*		GetBitsUpsidedown		(){return IsNull() ? NULL : (const void*)(&((BYTE*)bmpInfo_.bmBits)[(bmpInfo_.bmWidthBytes*(bmpInfo_.bmHeight - 1))]);};
	int				GetWidthBytes			(){return bmpInfo_.bmWidthBytes;};

protected:
	static bool InitGDIPlus(bool bShutDown = false) {
		static DWORD			dwToken;
		static CriticalSection	section;
		static bool				bInited;

		section.Lock(); // Lock
		bool bRet = true;
		if (!bShutDown) {
			Gdiplus::GdiplusStartupInput input;
			Gdiplus::GdiplusStartupOutput output;
			Gdiplus::Status status = Gdiplus::GdiplusStartup(&dwToken, &input, &output);
			if (status != Gdiplus::Ok)
				bRet = false;
			else
				bInited = true;
		}
		else {
			Gdiplus::GdiplusShutdown(dwToken);
			dwToken = 0L;
			bInited = false;
		}
		section.Unlock(); // Unlock
		return bRet;
	}

	static BYTE* GetColorByteByAlphaByteArray() {
		static BYTE g_btArray[256][256];
		static BOOL	g_bInited;
		if (!g_bInited) { // Initialize array.
			for (int c = 0; c <= 255; c++) {
				for (int a = 0; a <= 255; a++) {
					g_btArray[c][a] = (BYTE)(dAlpha[a] * c);
				}
			}
			g_bInited = TRUE;
		}
		return &g_btArray[0][0];
	}

protected:
	ImageDef	image_;
	BITMAP		bmpInfo_;
};

#endif // !defined(PLATFORMIMAGE_H_INCLUDED)

