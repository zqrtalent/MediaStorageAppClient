//#include "stdafx.h"
#include "GrowableMemory.h"
#include "../Serialize/Serializable.h"
//#include <afxdb_.h>

GrowableMemory::GrowableMemory(UINT nInitialSize /*= 0x400*/, UINT nGrowSize /*= 0x400*/, bool bUseHeap /*= true*/){
	m_nInitialSize	= nInitialSize;
	m_nGrowSize		= nGrowSize;
	m_bUseHeap		= bUseHeap;
	m_bReadOnly		= false;
	m_hHeap			= NULL;
	m_nBufferSize	= 0;
	m_nUsedSize		= 0;
	m_nOffset		= 0;
	m_pBuffer		= NULL;

	if( nInitialSize > 0 ){
		SetBufferSize(nInitialSize);
		} 
	}

GrowableMemory::~GrowableMemory(){
	if( !m_bReadOnly )
		SetBufferSize(0);
	}

void
GrowableMemory::SetReadonlyBuffer(BYTE* lpData, int nSize){
	// Empty previously allocated memory.
	if( m_nBufferSize > 0 && !m_bReadOnly ){
		SetBufferSize(0);
		}
	// For read-only purposes only.
	m_bReadOnly		= true;
	m_nBufferSize	= nSize;
	m_nUsedSize		= nSize;
	m_nOffset		= 0;
	m_pBuffer		= lpData;
	}

bool	
GrowableMemory::AddBytes(BYTE* lpData, int nSize){
	ASSERT( !m_bReadOnly );
	if( lpData == NULL || nSize <= 0 )
		return false;

	if( m_nBufferSize < (m_nUsedSize + nSize) ){
		int nNewSize = ((int)((m_nUsedSize + nSize) / m_nGrowSize) + 1) * m_nGrowSize;
		SetBufferSize(nNewSize);
		}

	memcpy((void*)&m_pBuffer[m_nUsedSize], lpData, nSize);
	m_nUsedSize += nSize;
	return true;
	}

bool
GrowableMemory::AddString(_string& sValue){
	short	shLen		= sValue.length();
	BYTE*	pszBuffer	= (BYTE*)stringEx::GetBuffer(sValue);

	if( !AddBytes((BYTE*)&shLen, sizeof(short)) )
		return false;
	if( shLen > 0 ){
		if( !AddBytes(pszBuffer, shLen) )
			return false;
		}
	return true;
	}

int
GrowableMemory::SetBufferSize(UINT nSize){
	if( nSize <= 0 ){ // Empty buffer.
        if(!m_bReadOnly){
            if( !m_bUseHeap ){
                delete [] m_pBuffer;
                }
            else{
#ifdef _WINDOWS
                if( m_hHeap != NULL ){
                    HeapFree	(m_hHeap, 0L, m_pBuffer);
                    HeapDestroy	(m_hHeap);
                    }
#elif __APPLE__
                if( m_hHeap != NULL ){
                    free(m_pBuffer);
                    }
#endif
                }
            }
        
        m_bReadOnly     = false;
		m_hHeap			= NULL;
		m_nBufferSize	= 0;
		m_nUsedSize		= 0;
		m_nOffset		= 0;
		m_pBuffer		= NULL;
		}
	else{
		if( m_bUseHeap ){
#ifdef _WINDOWS
			if( !m_hHeap ){
				m_hHeap = HeapCreate(0L, nSize, 0L);
				ASSERT( m_hHeap );
				m_pBuffer = (BYTE*)HeapAlloc(m_hHeap, 0L, nSize);
				}
			else
				m_pBuffer = (BYTE*)HeapReAlloc(m_hHeap, 0L, m_pBuffer, nSize);
#elif __APPLE__
            if( !m_hHeap ){
				m_hHeap = malloc(nSize);
				ASSERT( m_hHeap );
                }
			else
				m_hHeap = (BYTE*)realloc(m_hHeap, nSize);
            m_pBuffer = (BYTE*)m_hHeap;
#endif
			}
		else{
			BYTE* lpTemp = m_pBuffer;
			m_pBuffer = new BYTE[nSize];
			if( lpTemp ){
				memcpy(m_pBuffer, lpTemp, m_nUsedSize);
				delete [] lpTemp;
				}
			}

		m_nBufferSize	= nSize;
		m_nUsedSize		= (nSize < m_nUsedSize) ? nSize : m_nUsedSize;
		m_nOffset		= (nSize <= m_nOffset) ? nSize - 1 : m_nOffset;
		}
	return m_nBufferSize;
	}

BYTE*
GrowableMemory::EmptyBufferGetPtr(){
	if( m_bUseHeap ){
		return NULL;
		}

	BYTE*	pBuff	= m_pBuffer;
	m_pBuffer		= NULL;
	m_nBufferSize	= 0;
	m_nUsedSize		= 0;
	m_nOffset		= 0;
    m_bReadOnly     = false;
	return pBuff;
	}

GrowableMemory*
GrowableMemory::CopyGrowableMemoryObject(BOOL bCopyData /*= true*/){
    GrowableMemory* pCopy = new GrowableMemory(0, m_nGrowSize, m_bUseHeap);
    if(bCopyData){
        if(m_bReadOnly){
            pCopy->SetReadonlyBuffer(m_pBuffer, m_nBufferSize);
        }
        else{
            pCopy->AddBytes(m_pBuffer, m_nBufferSize);
        }
    }
    else{
        if(m_bReadOnly){
            pCopy->SetReadonlyBuffer(m_pBuffer, m_nBufferSize);
        }
        else{
            pCopy->m_pBuffer = m_pBuffer;
            pCopy->m_nBufferSize = m_nBufferSize;
            pCopy->m_nUsedSize = m_nUsedSize;
            pCopy->m_pBuffer = m_pBuffer;
            
            m_pBuffer = NULL;
            m_nBufferSize = m_nUsedSize = m_nOffset = 0;
        }
    }
    return pCopy;
}

BYTE*
GrowableMemory::GetCurrentOffsetPtr(){
	if( !m_pBuffer ) return NULL;
	return &((BYTE*)m_pBuffer)[m_nOffset];
	}

DWORD
GrowableMemory::GetCurrentOffset(){
	if( !m_pBuffer ) return 0;
	return (DWORD)m_nOffset;
	}

bool
GrowableMemory::SetCurrentOffset(DWORD dwOffset){
	if( dwOffset >= m_nBufferSize ){
		m_nOffset = (DWORD)m_nBufferSize;
		return false;
		}
	m_nOffset = (int)dwOffset;
	return true;
	}

BYTE*
GrowableMemory::GetOffsetPtr(DWORD dwOffset){
	if( !m_pBuffer ) return NULL;
	if( dwOffset >= (DWORD)m_nBufferSize )
		return NULL;
	return (BYTE*)&m_pBuffer[dwOffset];
	}

BOOL
GrowableMemory::ReadSkipBytes(int nBytesSkip){
	if( !m_pBuffer ) return FALSE;
	if( (m_nOffset + nBytesSkip) >= (DWORD)m_nBufferSize )
		return FALSE;
	m_nOffset += nBytesSkip;
	return TRUE;
	}

bool
GrowableMemory::ReadInt8(char* cVal){
	ASSERT(cVal);
	if( (m_nOffset + sizeof(char)) > m_nUsedSize )
		return false;
	*cVal = *(char*)(&((BYTE*)m_pBuffer)[m_nOffset]);		
	m_nOffset += sizeof(char);
	return true;
	}

bool
GrowableMemory::ReadInt16(short* shVal){
	ASSERT(shVal);
	if( (m_nOffset + sizeof(short)) > m_nUsedSize )
		return false;
	*shVal = *(short*)(&((BYTE*)m_pBuffer)[m_nOffset]);		
	m_nOffset += sizeof(short);
	return true;
	}

bool
GrowableMemory::ReadInt32(int* nVal){
	ASSERT(nVal);
	if( (m_nOffset + sizeof(int)) > m_nUsedSize )
		return false;
	*nVal = *(int*)(&((BYTE*)m_pBuffer)[m_nOffset]);		
	m_nOffset += sizeof(int);
	return true;
	}

bool
GrowableMemory::ReadInt64(__int64* bigVal){
	ASSERT(bigVal);
	if( (m_nOffset + sizeof(__int64)) > m_nUsedSize )
		return false;
	*bigVal = *(__int64*)(&((BYTE*)m_pBuffer)[m_nOffset]);		
	m_nOffset += sizeof(__int64);
	return true;
	}

bool
GrowableMemory::ReadFloat(float* fVal){
	ASSERT(fVal);
	if( (m_nOffset + sizeof(float)) > m_nUsedSize )
		return false;
	*fVal = *(float*)(&((BYTE*)m_pBuffer)[m_nOffset]);		
	m_nOffset += sizeof(float);
	return true;
	}

bool
GrowableMemory::ReadDouble(double* dVal){
	ASSERT(dVal);
	if( (m_nOffset + sizeof(double)) > m_nUsedSize )
		return false;
	*dVal = *(double*)(&((BYTE*)m_pBuffer)[m_nOffset]);		
	m_nOffset += sizeof(double);
	return true;
	}

bool	
GrowableMemory::ReadString(_string* sVal){
	ASSERT(sVal);
	if( (m_nOffset + sizeof(short)) > m_nUsedSize )
		return false;

	short shStringLen = *(short*)(&((BYTE*)m_pBuffer)[m_nOffset]);
	if( shStringLen == 0 ){ // Empty string.
		m_nOffset += sizeof(short);
		return true;
		}

	if( (m_nOffset + sizeof(short) + shStringLen) > m_nUsedSize ) // Check string length for validation.
		return false;

	char* pszVal = (char*)(&((BYTE*)m_pBuffer)[m_nOffset + sizeof(short)]);
	char cTemp = pszVal[shStringLen];
	pszVal[shStringLen] = '\0';
    
#ifdef _WINDOWS
	*sVal = pszVal;
#elif __APPLE__
    *sVal = [[NSString stringWithUTF8String:pszVal] cStringUsingEncoding:NSUTF8StringEncoding];
#else
    // Complete
#endif
	pszVal[shStringLen] = cTemp;

	m_nOffset += (shStringLen + sizeof(short));
	return true;
	}

bool
GrowableMemory::ReadBinary(LongBinary* pBinary){
	if( (m_nOffset + sizeof(short)) > m_nUsedSize )
		return false;

	int nLen = *(int*)(&((BYTE*)m_pBuffer)[m_nOffset]);
	if( nLen <= 0 ){ // Empty binary.
		m_nOffset += sizeof(int);
		return true;
		}

	if( (m_nOffset + sizeof(int) + nLen) > m_nUsedSize ) // Check binary length for validation.
		return false;

	// Copy into longbinary. {{
	void*	lpBinVal	= (void*)(&((BYTE*)m_pBuffer)[m_nOffset + sizeof(int)]);
	bool	bRet		=  pBinary->SetBinarySize(nLen, lpBinVal);
	if( bRet ){
		m_nOffset += (nLen + sizeof(int));
		return true;
		}
	return false;
	}

bool
GrowableMemory::ReadtagRect(_Rect* pRect){
	ASSERT(pRect);
	if( (m_nOffset + sizeof(RECTDef)) > m_nUsedSize )
		return false;
	*pRect = *(RECTDef*)(&((BYTE*)m_pBuffer)[m_nOffset]);		
	m_nOffset += sizeof(RECTDef);
	return true;
	}

bool
GrowableMemory::ReadtagPoint(_Point* pPoint){
	ASSERT(pPoint);
	if( (m_nOffset + sizeof(POINTDef)) > m_nUsedSize )
		return false;
	*pPoint = *(POINTDef*)(&((BYTE*)m_pBuffer)[m_nOffset]);		
	m_nOffset += sizeof(POINTDef);
	return true;
	}

bool
GrowableMemory::ReadtagSize(_Size* pSize){
	ASSERT(pSize);
	if( (m_nOffset + sizeof(SIZEDef)) > m_nUsedSize )
		return false;
	*pSize = *(SIZEDef*)(&((BYTE*)m_pBuffer)[m_nOffset]);		
	m_nOffset += sizeof(SIZEDef);
	return true;
	}

bool
GrowableMemory::ReadLogFont(LOGFONT* pLogFont){
	ASSERT(pLogFont);
	if( (m_nOffset + sizeof(LOGFONT)) > m_nUsedSize )
		return false;
	memcpy(pLogFont, (LOGFONT*)(&((BYTE*)m_pBuffer)[m_nOffset]), sizeof(LOGFONT));	
	m_nOffset += sizeof(LOGFONT);
	return true;
	}
