//////////////////////////////////////////////////////
//                                                  //
// started [22.09.2006]                             // 
//                                                  //
// Automatically sorted array...                    //
//                                                  //
// Author:  Zqr                                     //
//                                                  //
//////////////////////////////////////////////////////

#include "AutoSortedArray.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AutoSortedArray::AutoSortedArray(){
	m_pBuffer				= NULL;
	m_memAlloc				= 0L;
	m_memUsed				= 0L;
	m_memBlockCt			= 0L;
	m_bOptimizeLargeArray	= FALSE;
	m_nGrowBy				= 10;
	m_hHeap					= NULL;
    m_cmpFuncProc			= CompareDefault;
	m_lParam				= 0L;
	}

AutoSortedArray::~AutoSortedArray(){
	FreeMemory();
	}

void  
AutoSortedArray::Copy(AutoSortedArray* src){
	FreeMemory();
	int nCount       = src->GetCount();
	if( nCount ){
		AllocateMemory(nCount*ASARRAY_BLOCK_SIZE);
		m_memUsed    = nCount*ASARRAY_BLOCK_SIZE;
		m_memBlockCt = nCount;
        memcpy(m_pBuffer, src->GetBuffer(), m_memUsed);
		}
	}

// 2/7/2009 [ZqrTalent]
BOOL	
AutoSortedArray::GetData(void* lValue, void*& lData){
	int nFind	= FindValue(lValue);
	if( nFind == -1 )
		return FALSE;
	lData		= GetData(nFind);
	return TRUE;
	}

BOOL
AutoSortedArray::GetData(void* lValue, void*& lData, int& nIndex){
	int nFind	= FindValue(lValue);
	if( nFind == -1 )
		return FALSE;
	nIndex		= nFind;
	lData		= GetData(nFind);
	return 1;
	}

void 
AutoSortedArray::Add(void* lValue, void* lData){
	// allocate memory.////////////////////////////////////////
	if( m_memAlloc == m_memUsed )
		AllocateMemory(m_memAlloc+m_nGrowBy*ASARRAY_BLOCK_SIZE);
	//---------------------------------------------------------
	// find insert place(index) of value & insert it. /////////
	bool bExist;
	int nInsert  = FindValuePlace(lValue,bExist);
	void* dest   = (void*)((PTR_HOLDER_INTEGER)m_pBuffer + ASARRAY_BLOCK_SIZE*nInsert);
	long  size   = ASARRAY_BLOCK_SIZE*(m_memBlockCt-nInsert);
	if( size > 0 )
		memmove((void*)((PTR_HOLDER_INTEGER)dest+ASARRAY_BLOCK_SIZE),dest,size);
	//---------------------------------------------------------
	// add new values into memory./////////////////////////////
	void** pLong  = (void**)dest;
	*pLong		= lValue;
	pLong ++;
	*pLong		= lData;
	m_memUsed    += ASARRAY_BLOCK_SIZE; 
	m_memBlockCt ++;
	//---------------------------------------------------------
	//*******************************************************//
	}

void
AutoSortedArray::AddAsLong(long lValue, long lData){
	Add((void*)lValue, (void*)lData);
	}

void 
AutoSortedArray::Set(int nIndex, void* lValue, void* lData){
	ASSERT( nIndex > -1 && nIndex < m_memBlockCt );
	// find place of new value. //////////////////////////////////////
	bool bExist;
	int  nInsert   = FindValuePlace(lValue,bExist);
	//----------------------------------------------------------------
	// correct insert index of new value. ////////////////////////////
	void* lOldValue = Get(nIndex);
	if( (PTR_HOLDER_INTEGER)lValue > (PTR_HOLDER_INTEGER)lOldValue ) 
		nInsert --;
	//----------------------------------------------------------------
	// remove old block from array.///////////////////////////////////
	void*				dest     = (void*)((PTR_HOLDER_INTEGER)m_pBuffer + ASARRAY_BLOCK_SIZE*(nInsert+1));
	void*				src      = (void*)((PTR_HOLDER_INTEGER)m_pBuffer + ASARRAY_BLOCK_SIZE*(nIndex+1));
	PTR_HOLDER_INTEGER  size     = (PTR_HOLDER_INTEGER)dest-(PTR_HOLDER_INTEGER)src;
	if( size > 0 )
		memmove((void*)((PTR_HOLDER_INTEGER)src-ASARRAY_BLOCK_SIZE),src,size);
	if( size < 0 )
		memmove(dest,(void*)((PTR_HOLDER_INTEGER)dest-ASARRAY_BLOCK_SIZE),-size);
	//-----------------------------------------------------------------
	// set new element into array. ////////////////////////////////////
	void** pLong    = (void**)((PTR_HOLDER_INTEGER)dest-ASARRAY_BLOCK_SIZE);
	*pLong			= lValue;
//	pLong ++;
	*pLong			= lData;
	//-----------------------------------------------------------------
	//***************************************************************//
	}

void 
AutoSortedArray::Delete(int  nIndex, int  nCount){
	ASSERT( nIndex > -1 && nIndex < m_memBlockCt );
	ASSERT( (nIndex+nCount) <= m_memBlockCt      );

	// delete memory blocks from array./////////////////////////
	void* dest    = (void*)((PTR_HOLDER_INTEGER)m_pBuffer + ASARRAY_BLOCK_SIZE*nIndex);
	long  size    = ASARRAY_BLOCK_SIZE*(m_memBlockCt-nIndex-nCount);
	if( size )
		memmove(dest,(void*)((PTR_HOLDER_INTEGER)dest+ASARRAY_BLOCK_SIZE*nCount),size);
	m_memBlockCt -= nCount;
	m_memUsed     = m_memBlockCt*ASARRAY_BLOCK_SIZE;
	//----------------------------------------------------------
	//********************************************************//
	}

void 
AutoSortedArray::DeleteLast(int nCount){
	// change count of memory blocks.//
	ASSERT( nCount <= m_memBlockCt );
	m_memBlockCt   -= nCount;
	m_memUsed       = m_memBlockCt*ASARRAY_BLOCK_SIZE;
	//---------------------------------
	//*******************************//
	}

int
AutoSortedArray::FindValuePlace(void* lValue, bool& bExist){
	/////////////////////////////////////////////////////////
	bExist                = false;
	if( !m_memBlockCt ) return  0;

	int			   nRet	  = 0;
	int            nLast  = m_memBlockCt-1;
	int            nStart = 0;
	int            nMid;
	void*          lMid;

	while( nLast > nStart+1 ){
		nMid = nStart+(nLast-nStart)/2;
		lMid = *(void**)((PTR_HOLDER_INTEGER)m_pBuffer+nMid*ASARRAY_BLOCK_SIZE);
		nRet = (m_cmpFuncProc)(lMid, (void*)lValue);

		if( !nRet ){
			bExist = true;
			return   nMid;
			}
		else
			( nRet == 1 ) ?	nLast = nMid : nStart = nMid;
		}
	
	lMid = *(void**)((PTR_HOLDER_INTEGER)m_pBuffer+nStart*ASARRAY_BLOCK_SIZE);
	nRet = (m_cmpFuncProc)(lMid, (void*)lValue);
	if( nRet >= 0 ){
		bExist = (nRet == 0);
		return nStart;
		}
	else{
		lMid	= *(void**)((PTR_HOLDER_INTEGER)m_pBuffer+nLast*ASARRAY_BLOCK_SIZE);
		nRet	= (m_cmpFuncProc)(lMid, (void*)lValue);
		bExist	= (nRet == 0);
		return ( nRet < 0 ) ? nLast+1 : nLast;
		}
	//--------------------------------------------------------
	//********************************************************
	}

int		
AutoSortedArray::FindFirst(void* lValue){
	int nIndex		= FindValue(lValue);
	m_nLastFounded	= -1;
	m_lFindValue	= lValue;

	if( nIndex != -1 ){
		while( nIndex > 0 ){
			nIndex --;
			if( (m_cmpFuncProc)(Get(nIndex), lValue) ){
				m_nLastFounded = nIndex + 1;
				return (nIndex + 1);
				}		
			}
		m_nLastFounded = 0;
		return 0;
		}
	m_nLastFounded	= -1;
	m_lFindValue	= (void*)-1;
	return -1;
	}

int		
AutoSortedArray::FindNext(){
	ASSERT( m_nLastFounded > -1 );
	if( m_nLastFounded == GetCount() - 1 ){
		m_nLastFounded	= -1;
		m_lFindValue	= (void*)-1;
		return -1;
		}

	if( !(m_cmpFuncProc)(Get(m_nLastFounded + 1), m_lFindValue) ){
		m_nLastFounded ++;
		return m_nLastFounded;
		}
	
	m_nLastFounded	= -1;
	m_lFindValue	= (void*)-1;
	return -1;
	}

void 
AutoSortedArray::AllocateMemory(int nNewSize){
	if( !m_bOptimizeLargeArray ){
		// allocate memory if needed.///////////
		BYTE* pTemp   = new BYTE[nNewSize];
		// copy & free old allocated memory.////
		if( m_pBuffer ){
			memcpy(pTemp, m_pBuffer, m_memAlloc);
			delete m_pBuffer;
			}
		//--------------------------------------
		m_pBuffer     = pTemp;
		m_memAlloc    = nNewSize;
		ASSERT( m_pBuffer );
		// ####################################
		}
	else{
		if( !m_hHeap ){
//			m_hHeap = HeapCreate(0L, nNewSize, 0L);
//			ASSERT( m_hHeap );
//			m_pBuffer = HeapAlloc(m_hHeap, 0L, nNewSize);
			}

		// Allocate or realocate memory . #####
//		m_pBuffer	= HeapReAlloc(m_hHeap, 0L, m_pBuffer, nNewSize);
		m_memAlloc  = nNewSize;
		ASSERT( m_pBuffer );
		// ####################################
		}
	}

void 
AutoSortedArray::FreeMemory(){
	// free allocated memory.//
	if( m_pBuffer ){
		if( m_bOptimizeLargeArray ){
//			HeapFree	(m_hHeap, 0L, m_pBuffer);
//			HeapDestroy	(m_hHeap);
			}
		else
			delete m_pBuffer;
		}

	m_hHeap		 = NULL;
	m_pBuffer    = NULL;
	m_memAlloc   = 0L;
	m_memUsed    = 0L;
	m_memBlockCt = 0L;
	//-------------------------
	//***********************//
	}

int	
AutoSortedArray::CompareDefault(void* lpValue1, void* lpValue2){
	if( ((PTR_HOLDER_INTEGER)lpValue1) == ((PTR_HOLDER_INTEGER)lpValue2) )
		return 0;
	else
	if( ((PTR_HOLDER_INTEGER)lpValue1) > ((PTR_HOLDER_INTEGER)lpValue2) )
		return 1;
	return -1;
	}

int	
AutoSortedArray::CompareDesc(void* lpValue1, void* lpValue2){
	if( ((PTR_HOLDER_INTEGER)lpValue1) == ((PTR_HOLDER_INTEGER)lpValue2) )
		return 0;
	else
	if( ((PTR_HOLDER_INTEGER)lpValue1) > ((PTR_HOLDER_INTEGER)lpValue2) )
		return -1;
	return 1;
	}

int	
AutoSortedArray::CompareEmpty(void* lpValue1, void* lpValue2){
	return 1;
	}

int	
AutoSortedArray::CompareEmptyReverse(void* lpValue1, void* lpValue2){
	return -1;
	}

BOOL
AutoSortedArray::SetSize(UINT nElemCount){
	DeleteAll();
	FreeExtra();
	AllocateMemory(nElemCount*ASARRAY_BLOCK_SIZE);
	return TRUE;
	}
