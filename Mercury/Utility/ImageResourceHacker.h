#pragma once

#ifndef _WINNT_
	#include <winnt.h>
#endif

#include "Array\AutoSortedArray.h"

#define PIMG_RES_DATA_ENTRY PIMAGE_RESOURCE_DATA_ENTRY
#define PIMG_RES_DIR_ENTRY	PIMAGE_RESOURCE_DIRECTORY_ENTRY
#define PIMG_RES_DIR		PIMAGE_RESOURCE_DIRECTORY

__inline UINT ROUND(UINT value, UINT roundby)
	{
	if( value%roundby != 0 )
		value = (value/roundby)*roundby + roundby;
	return value;
	}

typedef struct ResourceItem_tag
	{
	ResourceItem_tag*	pChildItem;
	ResourceItem_tag*	pNextItem;
	BOOL				bFolder;
	CString				sItemName;
	int					nItemId;
	PIMG_RES_DATA_ENTRY pItemDataEntry;
	} ResourceItem, *PResourceItem;

typedef struct RepResTo_tag
	{
	CString			strReplaceFile;
	BYTE*			pBuffer;
	UINT			nBuffSize;
	} RepResTo, *PRepResTo;

typedef void (__cdecl *PPROC_ENUM_RESOURCES)(const CString sResType, const CString sResName, 
											 int nResTypeIndex, int nResCount);

class ImageResourceHacker
{
public:		// Constructor / Destructor
	ImageResourceHacker	();
	~ImageResourceHacker();

protected:	// Protected methods.
	void			WalkThroughResourceDirectory(PIMG_RES_DIR pResDir, PResourceItem pItem, 
												UINT nResOffset = 0, BOOL bFirstItem = TRUE);

	BOOL			FindResource				(const CString strResType, const CString strResName, PResourceItem pRoot, 
												PResourceItem& pItemResult);

	BOOL			CopyData					(BYTE* lpBuffer, UINT nBufferSize, 
												HANDLE hFileFrom, HANDLE hFileTo, UINT nCopySize, UINT& nCopied);
	/* Loads resource pointers from resource raw data and stores it in  m_arrResDataEntry as data value*/
	BOOL			LoadResourcesFromResRawData (void* lpResRawData, PIMAGE_SECTION_HEADER pRsrcSecHdr);
	/* Destroys resource item trees. */
	void			DestroyResourceItemsTree	(PResourceItem	pItem);
	void			FreeAllResources			();

	int CharToUnicode(const CString strChar, wchar_t* &wUnicode);

public:		// Public methods.
	BOOL			LoadImageFile				(const CString& strImageFile);
	BOOL			ReplaceResource				(const CString& strResType, const CString& strResName, 
												const CString& strReplaceFile);
	BOOL			FindResource				(const CString strResType, const CString strResName, 
												PResourceItem& pItemResult);
	BOOL			ReplaceResource				(const CString& strResType, const CString& strResName, 
												BYTE* lpBuffer, UINT nBuffSize);
	BOOL			FlushImage					(const CString& strImageFileOut, BOOL bOverwrite = FALSE);
	const CString&	GetLastErrorString			() { return m_strLastError; };
	BOOL			EnumResources				(PPROC_ENUM_RESOURCES enumProc);
	BOOL			SaveResource				(const CString& strResType, const CString& strResName,
												const CString& strFileSave);

	IMAGE_NT_HEADERS*		GetNTHeadersPointer	() { return &m_image_nt_hdrs;};
	IMAGE_SECTION_HEADER*	GetRsrcSecHeaderPtr	() { return  m_pRsrcSecHdr; };
public:		// Public attributes.

protected:	// Protected attributes.
	CString					m_strImageFile;			// Loaded image file path.
	PResourceItem			m_pRootRes;				// Pointer to the resource root item.
	CString					m_strLastError;			// Last error string.

	PPROC_ENUM_RESOURCES	m_enumProc;				// Resources enumerate procedure.

	IMAGE_DOS_HEADER		m_dos_hdr;				// Image's dos header structure.
	IMAGE_NT_HEADERS		m_image_nt_hdrs;		// Image's nt headers structure.
	PIMAGE_SECTION_HEADER	m_pRsrcSecHdr;			// Pointer to the image resource section header structure.
	BYTE*					m_pSectionHdrs;			// Image's section headers.
	BYTE*					m_pResRawData;			// Ponter to resource raw data.
	AutoSortedArray			m_arrResDataEntry;		// Sorted array of resource data entry pointers to current resource data.
	AutoSortedArray			m_arrReplaceRes;		// index of resource in m_arrResDataEntry to PRepResTo(as data).
};