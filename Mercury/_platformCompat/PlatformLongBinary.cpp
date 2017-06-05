#include "PlatformLongBinary.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
LongBinary::LongBinary(){
	m_hMemory		= NULL;
	m_dwSize		= 0L;
	}

LongBinary::~LongBinary(){
	SetBinarySize(0);
	}

void*
LongBinary::LockMemory(){
	if( m_dwSize == 0 || !m_hMemory )
		return NULL;
#ifdef __WINDOWS
	void* lpMem = NULL;
	lpMem = ::GlobalLock(m_hMemory);
	return lpMem;
#else
    return (void*)m_hMemory;
#endif
	}

void
LongBinary::UnlockMemory(){
#ifdef _WINDOWS
	if( m_dwSize == 0  || !m_hMemory )
		return;
	::GlobalUnlock(m_hMemory);
#endif
	}

BOOL
LongBinary::SetBinarySize(DWORD sizeNew, void* lpDataCopy){
#ifdef _WINDOWS
	DWORD sizeOld = m_hMemory != NULL ? (DWORD)::GlobalSize(m_hMemory) : 0;
	if( sizeOld == 0 ){
		m_hMemory		= ::GlobalAlloc(GMEM_MOVEABLE, sizeNew);
		m_dwSize		= sizeNew;
		}
	else{
		if( sizeNew > sizeOld ){
			m_hMemory	= ::GlobalReAlloc(m_hMemory, sizeNew, GMEM_MOVEABLE);
			m_dwSize	= sizeNew;
			}
		}
	
	if( lpDataCopy != NULL && m_hMemory ){
		void* lpDest = ::GlobalLock(m_hMemory);
		memcpy(lpDest, lpDataCopy, sizeNew);
		::GlobalUnlock(m_hMemory);
		}
#else
    DWORD sizeOld = m_dwSize;
    if( sizeNew == 0 ){
        if(m_hMemory){
            free(m_hMemory);
            m_hMemory = nullptr;
            }
        m_dwSize	= 0;
        }
    else{
        if( sizeNew > sizeOld ){
			m_hMemory	= realloc(m_hMemory, sizeNew);
			m_dwSize	= sizeNew;
            }
        }
    
    if( lpDataCopy != NULL && m_hMemory )
		memcpy(m_hMemory, lpDataCopy, sizeNew);
#endif
	return TRUE;
	}
