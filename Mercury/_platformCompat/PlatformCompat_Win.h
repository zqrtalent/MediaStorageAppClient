//
//  PlatformCompat.h
//  TestApp
//
//  Created by Zaqro Butskhrikidze on 7/30/14.
//  Copyright (c) 2014 Zaqro Butskhrikidze. All rights reserved.
//

#ifndef TestApp_PlatformCompat_h
#define TestApp_PlatformCompat_h
#include <WinSock2.h>
#include <windows.h>
#include <Mstcpip.h>
#include <MSWSock.h>
#include <WS2tcpip.h>

#ifdef ASSERT
#undef ASSERT
#endif
#include <assert.h>

#ifndef BOOL
	#define BOOL unsigned int
#endif

#ifndef HANDLE
	typedef void* HANDLE;
#endif

#ifndef UINT
	#define UINT unsigned int
#endif

#ifndef DWORD
	#define DWORD unsigned long
#endif

#ifndef ASSERT
	#define ASSERT(x) assert(x)
#endif

#ifndef TRUE
	#define TRUE 1
#endif

#ifndef FALSE
	#define FALSE 0
#endif

#ifndef BYTE
	#define BYTE unsigned char
#endif

#ifndef LPCTSTR
	#define LPCTSTR const TCHAR*
#endif

#define ASSERT(x) assert(x)

#if defined(__x86_64__) || defined(_M_X64)
/* x86 64-bit ----------------------------------------------- */
#define PTR_HOLDER_INTEGER int64_t

#elif defined(__i386) || defined(_M_IX86)
/* x86 32-bit ----------------------------------------------- */
#define PTR_HOLDER_INTEGER long
#endif

//#ifndef _T
//#define _T(x) x
//#endif

#ifndef TCHAR
#define TCHAR char
#endif

#define _tcscpy_ _tcscpy_s

#define SQLSMALLINT unsigned short

// Rect
typedef tagRECT		RECTDef;
#define LPRECTDef RECTDef*
#define LPCRECTDef const RECTDef*

// Point
typedef tagPOINT	POINTDef;
#define LPPOINTDef POINTDef*
#define LPCPOINTDef const POINTDef*

typedef struct tagPOINTF{
    FLOAT x;
    FLOAT y;
    } 	POINTF;

// Size
typedef tagSIZE		SIZEDef;
#define LPSIZEDef	SIZEDef*
#define LPCSIZEDef const SIZEDef*

// Region
#define RGNDef HRGN

// Device Context Handle
typedef HDC DCDef;
// Font Object Handle
typedef HFONT FONTDef;
// Image Object Handle
typedef HBITMAP ImageDef;

/*
typedef struct tagLOGFONTA
{
    LONG      lfHeight;
    LONG      lfWidth;
    LONG      lfEscapement;
    LONG      lfOrientation;
    LONG      lfWeight;
    BYTE      lfItalic;
    BYTE      lfUnderline;
    BYTE      lfStrikeOut;
    BYTE      lfCharSet;
    BYTE      lfOutPrecision;
    BYTE      lfClipPrecision;
    BYTE      lfQuality;
    BYTE      lfPitchAndFamily;
    CHAR      lfFaceName[LF_FACESIZE];
} LOGFONTA, *PLOGFONTA, NEAR *NPLOGFONTA, FAR *LPLOGFONTA;

typedef struct RECT_tag
{
    int left;
    int top;
    int right;
    int bottom;
}RECT;

typedef struct POINT_tag
{
    int x;
    int y;
}POINT;

typedef struct SIZE_tag
{
    int cx;
    int cy;
}SIZE;
*/
#include <tchar.h>
#include <string>
#include <memory>

typedef std::string _string;

class BaseClass {
public:
	BaseClass() {};
	virtual ~BaseClass() {};
};

class stringEx{
	public:
		static void			MakeUpper(std::string& str);
		static void			MakeLower(std::string& str);
		static const TCHAR* GetBuffer(std::string& str);
		static void			Remove(std::string& str, TCHAR cSymbol);
		static std::string	Right(std::string& sVal, int nCt);
		static std::string	TrimLeft(std::string& sVal);
		static std::string	TrimRight(std::string& sVal);
		static void			Format(std::string& resultString, const TCHAR* fmt_str, ...);
	};

#include <vector>
typedef std::vector<DWORD> CDWordArray;
typedef std::vector<std::string> CStringArray;
typedef std::vector<void*> CPtrArray;

/*
#include "PlatformSize_Win.h"
#include "PlatformRect_Win.h"
#include "PlatformPoint_Win.h"
#include "PlatformDateTime.h"
#include "PlatformLongBinary.h"
#include "PlatformFont_Win.h"

// Mutex/Synchronization
#include "PlatformCriticalSection_Win.h"
*/
#endif
