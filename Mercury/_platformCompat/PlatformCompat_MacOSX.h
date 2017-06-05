//
//  PlatformCompat.h
//  TestApp
//
//  Created by Zaqro Butskhrikidze on 7/30/14.
//  Copyright (c) 2014 Zaqro Butskhrikidze. All rights reserved.
//

#ifndef TestApp_PlatformCompat_h
#define TestApp_PlatformCompat_h
#include <assert.h>

// Causes link error for iOS
//#ifndef BOOL
//typedef bool BOOL;
//#endif

#define HANDLE void*
#define HMODULE void*
#define HINSTANCE void*
#define WPARAM unsigned int
#define LPARAM unsigned int
#define LRESULT unsigned int
#define UINT unsigned int
#define LONG long
#define UINT_PTR unsigned long
#define DWORD_PTR unsigned long
#define __int64 int64_t
#define __uint64 uint64_t
#define DWORD unsigned int
#define ASSERT(x) assert(x)
#define TRUE 1
#define FALSE 0
#define BYTE unsigned char

#define TCHAR char
#define _T(x) x
/*
#ifdef _MBCS
    #define TCHAR char
    #define _T(x) x
#else
    #define TCHAR wchar_t
    #define _T(x) L ##x
#endif*/

#define LPBYTE BYTE*
#define LPCTSTR const TCHAR*
#define LPCSTR const TCHAR*
#define HGLOBAL void*
#define SQLSMALLINT short

#if defined(__x86_64__) || defined(_M_X64)
/* x86 64-bit ----------------------------------------------- */
#define PTR_HOLDER_INTEGER int64_t

#elif defined(__i386) || defined(_M_IX86)
/* x86 32-bit ----------------------------------------------- */
#define PTR_HOLDER_INTEGER long
#else
#define PTR_HOLDER_INTEGER int64_t
#endif

#define _cdecl
#define __stdcall

typedef struct tagRECT
{
    int left;
    int top;
    int right;
    int bottom;
}RECT;

typedef struct tagPOINT
{
    int x;
    int y;
}POINT;

typedef struct tagSIZE
{
    int cx;
    int cy;
}SIZE;

typedef struct tagLOGFONT {
    int  lfHeight;
    int  lfWidth;
    int  lfEscapement;
    int  lfOrientation;
    int  lfWeight;
    BYTE  lfItalic;
    BYTE  lfUnderline;
    BYTE  lfStrikeOut;
    BYTE  lfCharSet;
    BYTE  lfOutPrecision;
    BYTE  lfClipPrecision;
    BYTE  lfQuality;
    BYTE  lfPitchAndFamily;
    char lfFaceName[32]; // UTF8
} LOGFONT, *PLOGFONT;

typedef struct tagBITMAP {
    int   bmType;
    int   bmWidth;
    int   bmHeight;
    int   bmWidthBytes;
    short   bmPlanes;
    short   bmBitsPixel;
    void* bmBits;
} BITMAP, *PBITMAP;

#define RGB(r,g,b) (COLORREF)((r << 16) | (g << 8) | b)
#define ARGB(r,g,b,a) (COLORREF)((a << 24) | (r << 16) | (g << 8) | b)
#define R_FROM_COLORREF(argb) ((argb & 0xFF0000) >> 16)
#define G_FROM_COLORREF(argb) ((argb & 0xFF00) >> 8)
#define B_FROM_COLORREF(argb) (argb & 0xFF)
#define A_FROM_COLORREF(argb) ((argb & 0xFF000000) >> 24)

#define SRCCOPY 0

// Font
#define ANSI_CHARSET 1
#define PROOF_QUALITY 1
#define CLEARTYPE_QUALITY 2
#define ANTIALIASED_QUALITY 3
#define DEFAULT_PITCH 1
#define FW_NORMAL 300
#define FW_MEDIUM 400
#define FW_BOLD 500

#define TRANSPARENT 1 // Transparent backgrount mode
#define ETO_CLIPPED 1 // ExtTextOut mode
#define IDC_HAND 1001 

//Window style
#define SW_SHOW 1

//Scrollbar
#define SB_LINEDOWN 1
#define SB_LINEUP 2
#define SB_PAGEDOWN 3
#define SB_PAGEUP 4
#define SB_THUMBTRACK 5
#define SB_LINELEFT 6
#define SB_LINERIGHT 7
#define SB_PAGELEFT 8
#define SB_PAGERIGHT 9

//Button
#define ODS_DISABLED 1
#define ODS_DEFAULT 2
#define ODS_HOTLIGHT 4
#define ODS_SELECTED 8

//Mouse state
#define MK_LBUTTON 1

//Char codes
#define VK_ESCAPE 1
#define VK_RETURN 2
#define VK_DOWN 3
#define VK_UP 4
#define VK_LCONTROL 5
#define VK_RCONTROL 6
#define VK_LSHIFT 7
#define VK_RSHIFT 8

// Event synchronization
#define INFINITE -1
#define WAIT_OBJECT_0 0
#define WAIT_TIMEOUT 0x00000102L
#define Sleep(n) sleep(n)


// Rect
typedef tagRECT		RECTDef;
#define LPRECTDef RECTDef*
#define LPCRECTDef const RECTDef*

// Point
typedef tagPOINT	POINTDef;
#define LPPOINTDef POINTDef*
#define LPCPOINTDef const POINTDef*

typedef struct tagPOINTF{
    float x;
    float y;
} 	POINTF;

// Size
typedef tagSIZE		SIZEDef;
#define LPSIZEDef	SIZEDef*
#define LPCSIZEDef const SIZEDef*

// Region
#define RGNDef void*

#import <CoreGraphics/CoreGraphics.h>
#import <UIKit/UiKit.h>

// Device Context Handle
typedef CGContextRef DCDef;
// Font Object Handle
#ifdef TARGET_OS_IPHONE
    typedef UIFont* FONTDef;
    //typedef UIGraphicsContext NSGraphicsContext;
#else
    typedef NSFont* FONTDef;
#endif
// Image Object Handle
typedef CGImageRef ImageDef;

// COLOR reference ARGB - pixel per byte (4 bytes)
#define COLORREF int

#define HWND int

#ifndef _min
#define _min(a,b) (a>b)?b:a
#endif

#ifndef _max
#define _max(a,b) (a<b)?b:a
#endif

#include <string>
#include <memory>

//string methods
#define _tcschr strchr
#define _tcsnccmp strncmp
#define _tcsncicmp strncasecmp
#define _tcslen strlen
#define _tcsicmp strcasecmp
#define _tcscmp strcmp
#define _tcscpy_ strcpy
#define _ttoi atoi
#define _ttof atof

typedef std::string _string;
/*
#ifdef _MBCS
#define _tcschr strchr
#define _tcsnccmp strncmp
#define _tcsncicmp strncasecmp
#define _tcslen strlen
#define _tcsicmp strcasecmp
#define _tcscmp strcmp
#define _tcscpy strcpy

typedef std::string _string;
#else
#define _tcschr wcschr
#define _tcsnccmp wcsncmp
#define _tcsncicmp wcsncasecmp
#define _tcslen wcslen
#define _tcsicmp wcscasecmp
#define _tcscmp wcscmp
#define _tcscpy wcscpy
typedef std::wstring _string;
#endif
*/

class stringEx{
public:
    static void			MakeUpper(_string& str);
    static void			MakeLower(_string& str);
    static const TCHAR* GetBuffer(_string& str);
    static void			Remove(_string& str, TCHAR cSymbol);
    static _string      Right(_string& sVal, int nCt);
    static _string      TrimLeft(_string& sVal);
    static _string      TrimRight(_string& sVal);
    static void			Format(_string& resultString, const TCHAR* fmt_str, ...);
};

class BaseClass{
public:
    BaseClass(){};
    virtual ~BaseClass(){};
};

#include <vector>
typedef std::vector<DWORD> CDWordArray;
typedef std::vector<_string> CStringArray;
typedef std::vector<void*> CPtrArray;

#endif
