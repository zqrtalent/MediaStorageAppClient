#include "stdafx.h"
#include "ImageResourceHacker.h"

// disable typecast warnings.
#pragma warning(disable:4311 4312)

ImageResourceHacker::ImageResourceHacker()
	{
	m_enumProc	= NULL;
	// Startup initialization. ===========
	FreeAllResources();
	// ===================================
	}

ImageResourceHacker::~ImageResourceHacker()
	{
	FreeAllResources();
	}

BOOL
ImageResourceHacker::LoadImageFile(const CString& strImageFile)
	{
	// Open image file. #########################################
	HANDLE	hImage = CreateFile(strImageFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0L, NULL);
	if( hImage == INVALID_HANDLE_VALUE )
		{
		m_strLastError = _T("Couldn't open image file !!!");
		return FALSE;
		}
	// ##########################################################

	UINT				nReadCt = 0L;
	IMAGE_DOS_HEADER	dos_hdr;
	IMAGE_NT_HEADERS	image_hdr;

	// Read image's dos header structure. #######################
	if( !ReadFile(hImage, (void*)&dos_hdr, sizeof(IMAGE_DOS_HEADER), (LPDWORD)&nReadCt, NULL) || 
		nReadCt != sizeof(IMAGE_DOS_HEADER) )
		{
		CloseHandle(hImage);
		m_strLastError = _T("Couldn't read IMAGE_DOS_HEADER structure !!!");
		return FALSE;
		}
	// ##########################################################

	// Check dos header validation. #############################
	if( dos_hdr.e_magic != 0x5A4D /*MZ - Mark Zbikowsky*/ || 
		dos_hdr.e_lfanew <= 0 )
		{
		CloseHandle(hImage);
		m_strLastError = _T("Invalid Portable Executable file !!!");
		return FALSE;
		}
	// ##########################################################
	
	//  Set file pointer to the beginning of IMAGE_NT_HEADERS structure.
	if( SetFilePointer(hImage, dos_hdr.e_lfanew, NULL, FILE_BEGIN) != dos_hdr.e_lfanew )
		{
		CloseHandle(hImage);
		m_strLastError = _T("SetFilePointer error !!!");
		return FALSE;
		}
	// ##########################################################

	// Read IMAGE_NT_HEADERS structure. #########################
	if( !ReadFile(hImage, (void*)&image_hdr, sizeof(IMAGE_NT_HEADERS), (LPDWORD)&nReadCt, NULL) ||
		nReadCt != sizeof(IMAGE_NT_HEADERS) )
		{
		CloseHandle(hImage);
		m_strLastError = _T("Couldn't read IMAGE_NT_HEADERS structure !!!");
		return FALSE;
		}
	// ##########################################################

	// Check signature validation. ##############################
	if( image_hdr.Signature != 0x00004550 /*PE\0\0*/)
		{
		CloseHandle(hImage);
		m_strLastError = _T("Invalid Portable Executable file !!!");
		return FALSE;
		}
	// ##########################################################

	// Read section header structures. ##########################
	UINT	nSecHdrsSize = sizeof(IMAGE_SECTION_HEADER)*image_hdr.FileHeader.NumberOfSections;
	BYTE*	pSectionHdrs = new BYTE[nSecHdrsSize];

	if( !ReadFile(hImage, (void*)pSectionHdrs, nSecHdrsSize, (LPDWORD)&nReadCt, NULL) ||
		nReadCt != nSecHdrsSize )
		{
		CloseHandle(hImage);
		m_strLastError = _T("Couldn't read section headers from image file !!!");
		delete pSectionHdrs;
		return FALSE;
		}
	// ##########################################################

	// Search resource section. #################################
	int						iLoop		= 0;
	PIMAGE_SECTION_HEADER	pRsrcHdr	= NULL;
	UINT nSize = 0;
	while( iLoop < image_hdr.FileHeader.NumberOfSections )
		{
		PIMAGE_SECTION_HEADER	pSecHdr		= ((PIMAGE_SECTION_HEADER)&pSectionHdrs[iLoop*sizeof(IMAGE_SECTION_HEADER)]);
		CString					strSection	= (LPTSTR)&pSecHdr->Name;
		if( !strSection.CompareNoCase(_T(".rsrc")) )
			{
			pRsrcHdr = pSecHdr;
		//	break;
			}
		nSize += pSecHdr->SizeOfRawData;
		iLoop ++;
		}

	if( pRsrcHdr == NULL )
		{
		CloseHandle(hImage);
		m_strLastError = _T("Resource section not exists !!!");
		delete pSectionHdrs;
		return FALSE;
		}
	// ##########################################################

	//  Set file pointer to the beginning of resource raw data. #
	BYTE*	pResRawData	= new BYTE[pRsrcHdr->SizeOfRawData];

	if( SetFilePointer(hImage, pRsrcHdr->PointerToRawData, NULL, FILE_BEGIN) != pRsrcHdr->PointerToRawData )
		{
		CloseHandle(hImage);
		m_strLastError = _T("SetFilePointer Error !!!");
		delete pSectionHdrs;
		delete pResRawData;
		return FALSE;
		}
	// ##########################################################

	// Read resource raw data. ##################################
	UINT nReadSize = pRsrcHdr->SizeOfRawData;
	if( !ReadFile(hImage, (void*)pResRawData, nReadSize, (LPDWORD)&nReadCt, NULL) || 
		nReadCt != nReadSize )
		{
		CloseHandle(hImage);
		m_strLastError = _T("Couldn't read resources at all from image file !!!");
		delete pSectionHdrs;
		delete pResRawData;
		return FALSE;
		}
	// ##########################################################

	// Free all founded resources. ##############################
	m_arrResDataEntry.DeleteLast(m_arrResDataEntry.GetCount());
	// ##########################################################
	
	// Clean up before. #########################################
	FreeAllResources();
	// ##########################################################

	// Scan resource tree. ######################################
	PResourceItem pItemRoot		= new ResourceItem;
	pItemRoot->bFolder			= TRUE;
	pItemRoot->nItemId			= 0L;
	pItemRoot->pChildItem		= NULL;
	pItemRoot->pItemDataEntry	= NULL;
	pItemRoot->pNextItem		= NULL;
	WalkThroughResourceDirectory((PIMG_RES_DIR)pResRawData, pItemRoot);
	// ##########################################################

	// Load resources. ##########################################
	if( !LoadResourcesFromResRawData(pResRawData, pRsrcHdr) )
		{
		CloseHandle(hImage);
		m_strLastError = _T("Couldn't load resources !!!");
		delete pSectionHdrs;
		delete pResRawData;
		return FALSE;
		}
	// ##########################################################

	// Copy loaded resource data. ###############################
	memmove(&m_dos_hdr,			&dos_hdr,	sizeof(dos_hdr));
	memmove(&m_image_nt_hdrs,	&image_hdr, sizeof(image_hdr));
	
	m_strImageFile	= strImageFile;
	m_pRootRes		= pItemRoot;
	m_pSectionHdrs	= pSectionHdrs;
	m_pResRawData	= pResRawData;
	m_pRsrcSecHdr	= pRsrcHdr;
	// ##########################################################
	
	CloseHandle(hImage);
	return TRUE;
	}

BOOL
ImageResourceHacker::ReplaceResource(const CString& strResType, const CString& strResName, 
									 const CString& strReplaceFile)
	{
	if( !m_pRootRes ) return FALSE;
	// Find resource. =====================================================
	PResourceItem pResult = NULL;
	if( FindResource(strResType, strResName, m_pRootRes, pResult) == FALSE )
		{
		m_strLastError.Format(_T("Couldnt found [Res Type = %s] [Res Name = %s]"), strResType, strResName);
		return FALSE;
		}
	// ====================================================================

	// Check for replace file validation. =================================
	HANDLE hReplaceFile = ::CreateFile(strReplaceFile, GENERIC_READ, FILE_SHARE_READ, 
										NULL, OPEN_EXISTING, 0L, NULL);
	if( hReplaceFile == INVALID_HANDLE_VALUE || GetFileSize(hReplaceFile, NULL) == 0 )
		{
		m_strLastError.Format(_T("Invalid replace file [%s]"), strReplaceFile);
		return FALSE;
		}
	CloseHandle(hReplaceFile);
	// ====================================================================

	// Check for Resource data entry existance. ===========================
	int nIndex = m_arrResDataEntry.FindValue((void*)pResult->pItemDataEntry);
	if( nIndex == -1 )
		{
		m_strLastError.Format(_T("Couldn't find resource data entry [Res Type = %s] [Res Name = %s]"), strResType, strResName);
		return FALSE;
		}
	// ====================================================================

	// Add founded resource into sorted array. ============================
	RepResTo* pRepResTo			= new RepResTo;
	pRepResTo->strReplaceFile	= strReplaceFile;
	pRepResTo->nBuffSize		= 0L;
	pRepResTo->pBuffer			= NULL;
	m_arrReplaceRes.Add((void*)nIndex, (void*)pRepResTo);
	// ====================================================================
	return TRUE;
	}

BOOL
ImageResourceHacker::ReplaceResource(const CString& strResType, const CString& strResName, 
									BYTE* lpBuffer, UINT nBuffSize)
	{
	if( !m_pRootRes ) 
		return FALSE;

	return TRUE;
	}

BOOL
ImageResourceHacker::SaveResource(const CString& strResType, const CString& strResName,
								  const CString& strFileSave)
	{
	ASSERT( m_pRootRes );
	PResourceItem itemSave;
	if( FindResource(strResType, strResName, m_pRootRes, itemSave) )
		{
		HANDLE hImageFile = CreateFile(m_strImageFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0L, NULL);
		if( hImageFile != INVALID_HANDLE_VALUE )
			{
			UINT nResOffset = ((long)itemSave->pItemDataEntry->OffsetToData) - (long)((long)m_pRsrcSecHdr->VirtualAddress - (long)m_pRsrcSecHdr->PointerToRawData);
			UINT nResSize	= itemSave->pItemDataEntry->Size;
			UINT nCopied	= 0L;

			if( SetFilePointer(hImageFile, nResOffset, NULL, FILE_BEGIN) == nResOffset )
				{
				// Create file save to. #########################################
				CFile fileSave(strFileSave, CFile::modeWrite|CFile::modeCreate);
				// ##############################################################
				
				// If bitmap resource. ##########################################
				if( !strResType.CompareNoCase(_T("bitmap")) )
					{
					BITMAPFILEHEADER bmFHdr;
					bmFHdr.bfType		= (((WORD)'M') << 8) | ((WORD)'B'); // BM keyword.
					bmFHdr.bfSize		= nResSize + sizeof(BITMAPFILEHEADER);
					bmFHdr.bfReserved1	= 0;
					bmFHdr.bfReserved2	= 0;
					bmFHdr.bfOffBits	= sizeof(BITMAPFILEHEADER) + sizeof(BITMAPCOREHEADER);
					fileSave.Write((void*)&bmFHdr, sizeof(BITMAPFILEHEADER));
					}
				// ##############################################################
				
				// Allocate buffer and copy resource data into file. ############
				BYTE* lpBuffer = new BYTE[512*1024];
				CopyData(lpBuffer, 512*1024, hImageFile, fileSave.m_hFile, nResSize, nCopied);
				delete lpBuffer;
				fileSave.Close();
				// ##############################################################
				}
			CloseHandle(hImageFile);
			if( nResSize && nResSize != nCopied )
				return FALSE;
			return TRUE;
			}
		}

	return FALSE;
	}

// Georgian symbol codes alphabetically.
const BYTE btGeoAlphabetical[] =
	{
	0xf4, 0xe8, 0xef, 0xe2, 0xf3, 0xec, 0xff, 
	0xc5, 0xf8, 0xeb, 0xe4, 0xfc, 0xf2, 0xf9, 
	0xe7, 0xce, 0xea, 0xfb, 0xe5, 0xe3, 0xe0, 
	0xe9, 0xca, 0xed, 0xdb, 0xd1, 0xf1, 0xdf, 
	0xf6, 0xd6, 0xfa, 0xee, 0xf0, 0x00
	};

int 
ImageResourceHacker::CharToUnicode(const CString strChar, wchar_t* &wUnicode)
	{
	static AutoSortedArray m_arrAlphabetical;
	if( m_arrAlphabetical.GetCount() == 0 )
		{
		int nLoop = 0;
		while( btGeoAlphabetical[nLoop] != 0x00 )
			{
			m_arrAlphabetical.AddAsLong(((long)btGeoAlphabetical[nLoop])&0xFFL, nLoop);
			nLoop ++;
			}
		}

	wUnicode	= new wchar_t[strChar.GetLength() + 1];
	wUnicode[strChar.GetLength()] = L'\x00';

	int nLoop = 0;
	while( nLoop < strChar.GetLength() )
		{
		int nFind = m_arrAlphabetical.FindValue((void*)( ((long)strChar[nLoop]) & 0xFF));
		if( nFind != -1 )
			wUnicode[nLoop] = (wchar_t)((int)m_arrAlphabetical.GetData(nFind) + 0x10d0);
		else
			wUnicode[nLoop]	= (wchar_t)(strChar[nLoop]);
		nLoop ++;
		}

	return strChar.GetLength();
	}

BOOL
ImageResourceHacker::FlushImage(const CString& strImageFileOut, BOOL bOverwrite /*= FALSE*/)
	{
	if( !m_pRootRes || m_arrReplaceRes.GetCount() == 0 || strImageFileOut.IsEmpty() ) 
		return FALSE;

	wchar_t* wFileOut;
	CharToUnicode(strImageFileOut, wFileOut);

	// Check for file existance. ##############################
	HANDLE hFileOut = CreateFileW(wFileOut, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0L, 0L);
	if( hFileOut != INVALID_HANDLE_VALUE )
		{
		CloseHandle(hFileOut);
		if( bOverwrite )
			{
			if( DeleteFileW(wFileOut) == FALSE )
				{
				m_strLastError.Format(_T("Couldn't delete file %s !!!"), strImageFileOut);
				delete wFileOut;
				return FALSE;
				}
			}
		else
			{
			m_strLastError.Format(_T("%s file exists !!!"), strImageFileOut);
			delete wFileOut;
			return FALSE;
			}
		}
	// #########################################################

	// Create image file. ######################################
	hFileOut = CreateFileW(wFileOut, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, 
							NULL, CREATE_NEW, 0L, 0L);
	if( hFileOut == INVALID_HANDLE_VALUE )
		{
		m_strLastError.Format(_T("Couldn't create image file %s !!!"), strImageFileOut);
		delete wFileOut;
		return FALSE;
		}
	// #########################################################

	// Open source image file. #################################
	HANDLE hFileImageSrc = CreateFile(m_strImageFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0L, 0L);
	if( hFileImageSrc == INVALID_HANDLE_VALUE )
		{
		CloseHandle(hFileOut);
		m_strLastError.Format(_T("Couldn't open image file %s !!!"), m_strImageFile);
		delete wFileOut;
		return FALSE;
		}
	// #########################################################

	delete wFileOut;

	PIMG_RES_DATA_ENTRY pFirstResDataEntry	= (PIMG_RES_DATA_ENTRY)m_arrResDataEntry.Get(0);
	ASSERT( pFirstResDataEntry );
	PIMG_RES_DATA_ENTRY pLastResDataEntry	= (PIMG_RES_DATA_ENTRY)m_arrResDataEntry.Get(m_arrResDataEntry.GetCount() - 1);
	ASSERT( pLastResDataEntry );

	int					nDataEntryCount		= ((long)pLastResDataEntry - (long)pFirstResDataEntry) / sizeof(IMAGE_RESOURCE_DATA_ENTRY) + 1;
	int					iLoop				= 0;
	UINT				nFirstResDataOffset = (UINT)-1;
	UINT				nDataEntrySize		= nDataEntryCount*sizeof(IMAGE_RESOURCE_DATA_ENTRY);

	//  retrieve first resource data offset. ###################
	while( iLoop < nDataEntryCount )
		{
		nFirstResDataOffset = min(nFirstResDataOffset, pFirstResDataEntry[iLoop].OffsetToData);
		iLoop ++;
		}
	// #########################################################

	// Convert from virtual address to real file offset. #######
	nFirstResDataOffset = nFirstResDataOffset - (long)((long)m_pRsrcSecHdr->VirtualAddress - (long)m_pRsrcSecHdr->PointerToRawData);
	// #########################################################

	// Allocate memory for destination image's data directory + data entries. #
	ASSERT( nFirstResDataOffset > m_pRsrcSecHdr->PointerToRawData );
	UINT nDataDirectorySize = nFirstResDataOffset - m_pRsrcSecHdr->PointerToRawData;
	BYTE*	lpDataDirDest	= new BYTE[nDataDirectorySize];
	// #########################################################
	
	// Copy data directory + data entries to destination images buffer. #######
	memcpy(lpDataDirDest, m_pResRawData, nDataDirectorySize);

	PIMG_RES_DATA_ENTRY pFirstResDataEntryDest	= (PIMG_RES_DATA_ENTRY)((long)pFirstResDataEntry - (long)m_pResRawData + (long)lpDataDirDest);
	ASSERT( pFirstResDataEntry );
	PIMG_RES_DATA_ENTRY pLastResDataEntryDest	= (PIMG_RES_DATA_ENTRY)((long)pLastResDataEntry - (long)m_pResRawData + (long)lpDataDirDest);
	ASSERT( pLastResDataEntry );
	// #########################################################

	UINT	nCopied			= 0L;
	UINT	nBuffSize		= 1024*1024;
	BYTE*	lpBuffer		= new BYTE[nBuffSize];
	UINT	nWritten		= 0L;
	UINT	nCopySize		= 0L;

	// Copy image data till resource datas. #####################
	nCopySize = nFirstResDataOffset;
	if( !CopyData(lpBuffer, nBuffSize, hFileImageSrc, hFileOut, nCopySize, nCopied) )
		{
		delete lpDataDirDest;
		delete lpBuffer;
		CloseHandle(hFileImageSrc);
		CloseHandle(hFileOut);
		m_strLastError = _T("Couldn't copy image file data !!!");
		return FALSE;
		}
	// ##########################################################

	iLoop						= 0;
	UINT	nOffset				= pFirstResDataEntry->OffsetToData;
	long	nAllResSizeOld		= m_image_nt_hdrs.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].Size;
	long	nAllResSizeNew		= nAllResSizeOld;

	while( iLoop < nDataEntryCount )
		{
		int		nIndex		= m_arrResDataEntry.FindValue(&pFirstResDataEntry[iLoop]);
		ASSERT( nIndex != -1 );
		BYTE*	lpResData	= (BYTE*)m_arrResDataEntry.GetData(nIndex);
		ASSERT( lpResData );

		nIndex = m_arrReplaceRes.FindValue((void*)nIndex);
		if( nIndex != -1 )
			{
			PRepResTo pRepRes = (PRepResTo)m_arrReplaceRes.GetData(nIndex);
			ASSERT( pRepRes );

			if( !pRepRes->strReplaceFile.IsEmpty() )
				{
				// Open replace file. ##########################################
				HANDLE hResFile = CreateFile(pRepRes->strReplaceFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0L, 0L);
				if( hResFile == INVALID_HANDLE_VALUE )
					{
					m_strLastError.Format(_T("Couldn't open file %s !!!"), pRepRes->strReplaceFile);
					delete lpDataDirDest;
					delete lpBuffer;
					CloseHandle(hFileImageSrc);
					CloseHandle(hFileOut);
					return FALSE;
					}
				// ############################################################

				UINT nFileSize = GetFileSize(hResFile, NULL);
				ASSERT( nFileSize );
				// Copy new resource data. ####################################
				if( !CopyData(lpBuffer, nBuffSize, hResFile, hFileOut, nFileSize, nCopied) )
					{
					delete lpDataDirDest;
					delete lpBuffer;
					CloseHandle(hResFile);
					CloseHandle(hFileImageSrc);
					CloseHandle(hFileOut);
					m_strLastError = _T("Couldn't copy image file data !!!");
					return FALSE;
					}
				CloseHandle(hResFile);
				// ########################################################

				// Round resource data by 4. ##############################
				nCopySize = ROUND(nFileSize, 4) - nFileSize;
				if( nCopySize > 0 )
					{
					int nZero = 0;
					if( !WriteFile(hFileOut, (void*)&nZero, nCopySize, (LPDWORD)&nWritten, NULL) || nWritten != nCopySize )
						{
						delete lpDataDirDest;
						delete lpBuffer;
						CloseHandle(hResFile);
						CloseHandle(hFileImageSrc);
						CloseHandle(hFileOut);
						m_strLastError = _T("WriteFile error !!!");
						return FALSE;
						}
					}
				// ########################################################

				long lAdd = nFileSize - pFirstResDataEntryDest[iLoop].Size;
				pFirstResDataEntryDest[iLoop].OffsetToData	= nOffset;
				pFirstResDataEntryDest[iLoop].Size			= nFileSize;
				nOffset										+= ROUND(nFileSize, 4);
				nAllResSizeNew								+= lAdd;
				}
			else
				{
				}
			}
		else
			{
			// Copy resource data. ########################################
			UINT nWrite = ROUND(pFirstResDataEntry[iLoop].Size, sizeof(DWORD));
			if( !WriteFile(hFileOut, lpResData, nWrite, (LPDWORD)&nWritten, NULL) ||
				nWritten != nWrite )
				{
				delete lpDataDirDest;
				delete lpBuffer;
				CloseHandle(hFileImageSrc);
				CloseHandle(hFileOut);
				m_strLastError = _T("Couldn't write resource data !!!");
				return FALSE;
				}
			// ############################################################
			pFirstResDataEntryDest[iLoop].OffsetToData	= nOffset;
			nOffset										+= nWrite;
			}
		iLoop ++;
		}
	// ######################################################################

	// Set file pointer to the end of resources. ##########################
	nOffset			= m_pRsrcSecHdr->PointerToRawData + m_image_nt_hdrs.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].Size;
	if( SetFilePointer(hFileImageSrc, nOffset, NULL, FILE_BEGIN) != nOffset )
		{
		delete lpBuffer;
		CloseHandle(hFileImageSrc);
		CloseHandle(hFileOut);
		m_strLastError = _T("SetFilePointer error !!!");
		return FALSE;
		}
	// ####################################################################

	// Copy all remaining image data. #####################################
	UINT nFileSize	= GetFileSize(hFileImageSrc, NULL);
	nCopySize		= nFileSize - nOffset;
	if( !CopyData(lpBuffer, nBuffSize, hFileImageSrc, hFileOut, nCopySize, nCopied) )
		{
		delete lpDataDirDest;
		delete lpBuffer;
		CloseHandle(hFileImageSrc);
		CloseHandle(hFileOut);
		m_strLastError = _T("Couldn't copy image file data !!!");
		return FALSE;
		}
	// ####################################################################

	// Set file pointer to the beginning of resource raw data. ############
	if( SetFilePointer(hFileOut, m_pRsrcSecHdr->PointerToRawData, NULL, FILE_BEGIN) != m_pRsrcSecHdr->PointerToRawData )
		{
		delete lpBuffer;
		CloseHandle(hFileImageSrc);
		CloseHandle(hFileOut);
		m_strLastError = _T("SetFilePointer error !!!");
		return FALSE;
		}
	// ###################################################################

	// Copy resource data directories, data entries and constant strings.#
	nCopySize = nFirstResDataOffset - m_pRsrcSecHdr->PointerToRawData;
	if( !WriteFile(hFileOut, (void*)lpDataDirDest, nCopySize, (LPDWORD)&nWritten, NULL) ||
		nWritten != nCopySize )
		{
		delete lpDataDirDest;
		delete lpBuffer;
		CloseHandle(hFileImageSrc);
		CloseHandle(hFileOut);
		m_strLastError = _T("Couldn't write resource directory entries !!!");
		return FALSE;
		}
	// ####################################################################

	// Save changable values. #############################################
	UINT nRsrcVirtualSizeOld		= m_pRsrcSecHdr->Misc.VirtualSize;
	UINT nRsrcSizeOfRawDataOld		= m_pRsrcSecHdr->SizeOfRawData;
	UINT nRsrcSizeOfRawDataSecOld	= m_image_nt_hdrs.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].Size;
	UINT nSizeOfImageOld			= m_image_nt_hdrs.OptionalHeader.SizeOfImage;
	// ####################################################################

	// Write IMAGE_NT_HEADERS and section headers structure. ##############
	m_pRsrcSecHdr->Misc.VirtualSize				= nAllResSizeNew;
	nAllResSizeNew								= ROUND(nAllResSizeNew, sizeof(DWORD));
	m_image_nt_hdrs.OptionalHeader.SizeOfImage -= m_pRsrcSecHdr->SizeOfRawData;
	m_image_nt_hdrs.OptionalHeader.SizeOfImage += ROUND(nAllResSizeNew, m_image_nt_hdrs.OptionalHeader.FileAlignment);
	m_pRsrcSecHdr->SizeOfRawData				= nAllResSizeNew;
	m_image_nt_hdrs.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].Size = nAllResSizeNew;
	// ####################################################################

	// Set file pointer to NT header start. ###############################
	if( SetFilePointer(hFileOut, m_dos_hdr.e_lfanew, NULL, FILE_BEGIN) != m_dos_hdr.e_lfanew )
		{
		delete lpDataDirDest;
		delete lpBuffer;
		CloseHandle(hFileImageSrc);
		CloseHandle(hFileOut);
		m_strLastError = _T("SetFilePointer error !!!");
		return FALSE;
		}
	// ######################################################################

	// Write IMAGE_NT_HEADERS structure and section header structures. #####
	UINT nSecHdrsSize = m_image_nt_hdrs.FileHeader.NumberOfSections*sizeof(IMAGE_SECTION_HEADER);
	if( !WriteFile(hFileOut, (void*)&m_image_nt_hdrs, sizeof(IMAGE_NT_HEADERS), (LPDWORD)&nWritten, NULL)	|| 
		nWritten != sizeof(IMAGE_NT_HEADERS)																|| 
		!WriteFile(hFileOut, (void*)m_pSectionHdrs,	nSecHdrsSize, (LPDWORD)&nWritten, NULL)					||
		nWritten != nSecHdrsSize																			)
		{
		delete lpDataDirDest;
		delete lpBuffer;
		CloseHandle(hFileImageSrc);
		CloseHandle(hFileOut);
		m_strLastError = _T("Couldn't write IMAGE_NT_HEADERS structure and section header structures !!!");
		return FALSE;
		}
	// ######################################################################

	// Restore changable values. ##########################################
	m_pRsrcSecHdr->Misc.VirtualSize				= nRsrcVirtualSizeOld;
	m_pRsrcSecHdr->SizeOfRawData				= nRsrcSizeOfRawDataOld;
	m_image_nt_hdrs.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].Size = nRsrcSizeOfRawDataSecOld;
	m_image_nt_hdrs.OptionalHeader.SizeOfImage	= nSizeOfImageOld;
	// ####################################################################

	delete lpDataDirDest;
	delete lpBuffer;
	CloseHandle(hFileImageSrc);
	CloseHandle(hFileOut);
	return TRUE;
	}

BOOL
ImageResourceHacker::EnumResources(PPROC_ENUM_RESOURCES enumProc)
	{
	ASSERT( !IsBadCodePtr((FARPROC)enumProc) );
	if( !m_pRootRes ) return FALSE;

	CString			sResNameOrId;
	CString			sResType;
	PResourceItem	pItemType		= m_pRootRes->pChildItem;
	int				nResIndex		= 0;
	int				nResTypeIndex	= 0;
	while( pItemType )													// resource types loop.
		{
		PResourceItem	pItemRes = pItemType->pChildItem;
		sResType = pItemType->sItemName;
		if( sResType.IsEmpty() )
			sResType.Format("%d", pItemType->nItemId);

		while( pItemRes )												// resource names loop.
			{
			if( pItemRes->bFolder && pItemRes->pChildItem )
				{
				ASSERT( pItemRes->pChildItem );
				sResNameOrId = pItemRes->sItemName;
				if( sResNameOrId.IsEmpty() )
					sResNameOrId.Format("%d", pItemRes->nItemId);
				}
			else
				{
				sResNameOrId = pItemRes->sItemName;
				if( sResNameOrId.IsEmpty() )
					sResNameOrId.Format("%d", pItemRes->nItemId);
				}
			
			(enumProc)(sResType, sResNameOrId, nResTypeIndex, nResIndex);
			pItemRes = pItemRes->pNextItem;
			nResIndex ++;
			}
		pItemType = pItemType->pNextItem; nResIndex = 0; nResTypeIndex ++;
		}

	return TRUE;
	}

// =============================================================
//
//	Protected Methods.
//
// =============================================================

const CString strResTypes[] =
				{_T("Cursor"),			_T("Bitmap"),			_T("Icon"),		_T("Menu"),			_T("Dialog"),
				 _T("String Table"),	_T("Fontdir"),			_T("Font"),		_T("Accelerator"),	_T("RcData"),
				 _T("MessageTable"),	_T("Cursor Group"),		_T("13"),		_T("Icon Group"),	_T("15"),
				 _T("Version"),			_T("Dialog Include"),	_T("Plug Play"),_T("18"),			_T("VXD"),
				 _T("Animated Cursor"), _T("Animated Icon"),	_T("HTML"),		_T("Manifest")};

void
ImageResourceHacker::WalkThroughResourceDirectory(PIMG_RES_DIR pResDir, PResourceItem pItem, 
												  UINT nResOffset /*= 0*/, BOOL bFirstItem /*= TRUE*/)
	{
	PIMG_RES_DIR_ENTRY	pFirstDirEntry	= (PIMG_RES_DIR_ENTRY)(((long)pResDir) + sizeof(IMAGE_RESOURCE_DIRECTORY));
	UINT				nIdEntry		= 0;
	UINT				nEntryCt		= pResDir->NumberOfIdEntries + pResDir->NumberOfNamedEntries;
	
	PResourceItem		pItemFirst		= NULL;
	PResourceItem		pItemPrev		= NULL;
	PResourceItem		pItemNew		= NULL;

	while( nIdEntry < nEntryCt )
		{
		// Allocate new ResourceItem structure. =========
		pItemNew					= new ResourceItem;
		pItemNew->nItemId			= 0;
		pItemNew->bFolder			= FALSE;
		pItemNew->pChildItem		= NULL;
		pItemNew->pItemDataEntry	= NULL;
		pItemNew->pNextItem			= NULL;
		// ==============================================
		
		// Add new pointer to pointers chain. ===========
		if( pItemFirst == NULL )
			pItemFirst	= pItemNew;
		else
			pItemPrev->pNextItem = pItemNew;
		pItemPrev	= pItemNew;
		// ==============================================

		PIMG_RES_DIR_ENTRY pDirEntry	= &pFirstDirEntry[nIdEntry];
		if( pDirEntry->NameIsString )
			{
			WORD	wLength				= *(WORD*)(((long)pResDir) - nResOffset + pDirEntry->NameOffset);
			LPWSTR	lpwszResName		= (LPWSTR)(((long)pResDir) - nResOffset + pDirEntry->NameOffset + sizeof(WORD));
			char	lpszResName[255];
			lpszResName[wLength]		= '\x00';
			// Convert UNICODE to ANSI. ===========================
			::WideCharToMultiByte(CP_ACP, 0L, lpwszResName, wLength, lpszResName, 255, NULL, NULL);
			// ====================================================
			pItemNew->sItemName			= lpszResName;
			}
		else
			{
			pItemNew->nItemId			= pDirEntry->Id;
			}

		if( pDirEntry->DataIsDirectory )
			{
			pItemNew->bFolder			= TRUE;
			if( bFirstItem )
				{
				// Determine resource type. ===========================
				if( pItemNew->nItemId >= 1 && pItemNew->nItemId <= 24 )
					pItemNew->sItemName = strResTypes[pItemNew->nItemId - 1];
				// =====================================================
				}
			WalkThroughResourceDirectory((PIMG_RES_DIR)(((long)pResDir) - nResOffset + pDirEntry->OffsetToDirectory), pItemNew, pDirEntry->OffsetToDirectory, FALSE);
			}
		else
			{
			PIMG_RES_DATA_ENTRY pResDataEntry	= (PIMG_RES_DATA_ENTRY)(((long)pResDir) - nResOffset + pDirEntry->OffsetToData);
			pItemNew->pItemDataEntry			= pResDataEntry;
			m_arrResDataEntry.Add(pResDataEntry);
			}
		nIdEntry	++;
		}

	pItem->pChildItem = pItemFirst;
	}

BOOL			
ImageResourceHacker::FindResource(const CString strResType, const CString strResName, 
								PResourceItem& pItemResult)
	{
	return FindResource(strResType, strResName, m_pRootRes, pItemResult);
	}

BOOL
ImageResourceHacker::FindResource(const CString strResType, const CString strResName, 
								  PResourceItem pRoot, PResourceItem& pItemResult)
	{
	BOOL			bRet		= FALSE;	
	PResourceItem	pItemType	= pRoot->pChildItem;
	UINT			nResId		= atoi(strResName);
	UINT			nResType	= atoi(strResType);

	while( pItemType )														// resource types loop.
		{
		if( strResType.CompareNoCase(pItemType->sItemName) == 0  ||			// resource type name found;
			(nResType && nResType == pItemType->nItemId) )			
			{
			PResourceItem	pItemRes = pItemType->pChildItem;
			while( pItemRes )												// resource names loop.
				{
				if( strResName.CompareNoCase(pItemRes->sItemName) == 0 ||
					(nResId && nResId == pItemRes->nItemId) )	// resource name found;
					{
					if( pItemRes->pItemDataEntry  == NULL )
						pItemRes = pItemRes->pChildItem;

					bRet		= (pItemRes->pItemDataEntry != NULL);
					pItemResult = pItemRes;
					break;
					}
				pItemRes = pItemRes->pNextItem;
				}
			}
		pItemType = pItemType->pNextItem;
		}
	return bRet;
	}

void			
ImageResourceHacker::DestroyResourceItemsTree(PResourceItem	pItem)
	{
	while( pItem )
		{
		if( pItem->pChildItem )
			DestroyResourceItemsTree(pItem->pChildItem);

		PResourceItem	pItemNext = pItem->pNextItem;
		delete pItem;
		pItem = pItemNext;
		}
	}

void
ImageResourceHacker::FreeAllResources()
	{
	// Destroy resource items.===========
	if( IsBadReadPtr(m_pRootRes, sizeof(ResourceItem)) == FALSE )
		{
		DestroyResourceItemsTree(m_pRootRes);
		m_pRootRes = NULL;
		}
	else
		m_pRootRes		= NULL;
	// ==================================

	// Free replace array. ==============
	int iLoop = 0;
	while( iLoop < m_arrReplaceRes.GetCount() )
		{
		PRepResTo pResTo = (PRepResTo)m_arrReplaceRes.GetData(iLoop);
		if( pResTo != NULL )
			delete pResTo;
		iLoop ++;
		}
	m_arrReplaceRes.DeleteLast(m_arrReplaceRes.GetCount());
	// ==================================

	m_strImageFile		= _T("");
	m_pRootRes			= NULL;
	m_pRsrcSecHdr		= NULL;

	ZeroMemory(&m_dos_hdr,		sizeof(m_dos_hdr)		);
	ZeroMemory(&m_image_nt_hdrs,sizeof(m_image_nt_hdrs)	);

	if( IsBadReadPtr(m_pSectionHdrs, sizeof(BYTE)) == FALSE )
		{
		delete m_pSectionHdrs;
		m_pSectionHdrs = NULL;
		}
	else
		m_pSectionHdrs = NULL;

	if( IsBadReadPtr(m_pResRawData, sizeof(BYTE)) == FALSE )
		{
		delete m_pResRawData;
		m_pResRawData = NULL;
		}
	else
		m_pResRawData = NULL;
	}

BOOL
ImageResourceHacker::CopyData(BYTE* lpBuffer, UINT nBufferSize, HANDLE hFileFrom, 
							  HANDLE hFileTo, UINT nCopySize, UINT& nCopied)
	{
	UINT nReadCt, nWritten;
	nCopied = 0;
	while( nCopySize > 0 )
		{
		int nReadWriteCt = min(nCopySize, nBufferSize);
		if( !ReadFile(hFileFrom, lpBuffer, nReadWriteCt, (LPDWORD)&nReadCt, NULL) || 
			nReadWriteCt != nReadCt )
			{
			return FALSE;
			}

		if( !WriteFile(hFileTo, lpBuffer, nReadWriteCt, (LPDWORD)&nWritten, NULL) ||
			nReadWriteCt != nWritten )
			{
			return FALSE;
			}
		nCopySize	-= nReadWriteCt;
		nCopied		+= nReadWriteCt;
		}
	return TRUE;
	}

BOOL
ImageResourceHacker::LoadResourcesFromResRawData(void* lpResRawData, PIMAGE_SECTION_HEADER pRsrcSecHdr)
	{
	long nCount					= m_arrResDataEntry.GetCount();
	long iLoop					= 0;
	
	while( iLoop < nCount )
		{
		PIMG_RES_DATA_ENTRY pResDataEntry = ((PIMG_RES_DATA_ENTRY)m_arrResDataEntry[iLoop]);
		if( pResDataEntry )
			{
			ASSERT( pResDataEntry->Size );
			long	lOffset			 = (long)pResDataEntry->OffsetToData - (long)pRsrcSecHdr->VirtualAddress;
			void*	lpResDataPointer = (void*)(lOffset + (long)lpResRawData);
			m_arrResDataEntry.Set(iLoop, pResDataEntry, lpResDataPointer);
			}
		iLoop ++;
		}

	return TRUE;
	}