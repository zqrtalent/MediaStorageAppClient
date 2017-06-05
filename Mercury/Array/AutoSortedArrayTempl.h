//////////////////////////////////////////////////////
//                                                  //
// started [22.09.2006]                             // 
//                                                  //
// Automatically sorted array...                    //
//                                                  //
// Author:  Zqr                                     //
//                                                  //
//////////////////////////////////////////////////////

#pragma once

#include "../_platformCompat/PlatformCompat.h"
#include "../String/StringDecoder.h"

// define compare function.
typedef int (*CompareProc)(void* lpValue1, void* lpValue2);
typedef BOOL (*KeyGenProc)(void* lpGenFrom, void* lpKey);

// disable typecast warnings.
//#pragma warning(disable:4311 4312)
#pragma pack(1)

template <class KEY, class VALUE>
class AutoSortedArrayTempl
{
public:
	AutoSortedArrayTempl (int nSize);
	AutoSortedArrayTempl ();
	~AutoSortedArrayTempl();
    
    #pragma pack(1)
	struct MemBlock
		{
		KEY k;
		VALUE v;
		};

	KEY			operator[]			(int nIndex); 
	void		Copy				(AutoSortedArrayTempl* src);
	void		Add					(KEY lValue, VALUE lData);
	void		Set					(int  nIndex, KEY lValue, VALUE lData);
	BOOL		SetAt				(KEY lValue, VALUE value);
	VALUE&		GetAt				(KEY lValue);

	void		Add_KeyGen			(void* lpGenFrom, VALUE value);
	void		Set_KeyGen			(int nIndex, void* lpGenFrom, VALUE value);
	void		SetAt_KeyGen		(void* lpGenFrom, VALUE value);
	void		SetKeyGenFunction	(KeyGenProc keyGenProc);

	void		DeleteLast			(int  nCount);
	void		DeleteAll			();
	void		FreeExtra			();
	void		Delete				(int  nIndex, int  nCount = 1);
	KEY&		Get					(int  nIndex);
	VALUE&		GetData				(int  nIndex);
	KEY&		GetWithData			(int  nIndex, VALUE& lData);
	
	int			GetCount			();
	void*		GetBuffer			();

	BOOL		SetSize				(int nElemCount);

	// 2/7/2009 [ZqrTalent]
	BOOL		GetData				(KEY lValue, VALUE& lData, int& nIndex);
	// 10/7/2009 [ZqrTalent]
	BOOL		SetData				(int nIndex, VALUE& lData);

	// Find multiply values. ##################################
	int			FindFirst			(KEY lValue);
	int			FindNext			();
	// ########################################################

	// finds value into array. returns -1 if not find.
	int			FindValue			(KEY lValue);
	long		FindValuePlace		(KEY lValue, bool& bExist);
	int			IndexOf				(KEY lValue){return FindValue(lValue);};

	void		OptimizeForLargeData(BOOL bOptimize);

	void		SetCompareFunction	(CompareProc cmpProc);
	CompareProc GetCompareFunc		();

	/* New methods. 10/6/2011 */
	KEY&		GetKey				(int nIndex);
	VALUE&		GetValueByIndex		(int nIndex);
	VALUE&		GetValue			(KEY key);
	VALUE&		GetValue_KeyGen		(void* lpKey);
	BOOL		GetBoth				(int nIndex, KEY& key, VALUE& value);

public:
	static int	CompareDefault		(void* lpValue1, void* lpValue2);
	static int	CompareRects		(RECT* lpValue1, RECT* lpValue2);
	static int	CompareEmpty		(void* lpValue1, void* lpValue2);

	static BOOL	KeyGenFromString	(void* lpGenFrom, void* key);

private:
	MemBlock*			m_pBuffer;				// allocated memory.
	int					m_memAlloc;				// allocated memory size.
	int					m_memUsed;				// used memory size.
	int					m_memBlockCt;			// count of memory blocks in array.
	BOOL				m_bOptimizeLargeArray;	// Optimize for large array.
	int					m_nGrowBy;
#ifdef _WINDOWS
	HANDLE				m_hHeap;				// Handle of created Heap. (Used only in OptimizeForLargeData mode)
#endif
	int					m_nLastFounded;			// Last founded value index.
	KEY					m_lFindValue;			// Find value.
	int					m_nBlockSize;			// Block size in bytes.

	KEY					m_keyDefault;			// Default key.
	VALUE				m_valueDefault;			// Default value.

	CompareProc			m_cmpFuncProc;			// Compare function proc address.
	KeyGenProc			m_keyGenFuncProc;		// Key value generate function proc address.

protected:
	void AllocateMemory	(int nNewSize);
	void FreeMemory		();
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
template <class KEY, class VALUE>
AutoSortedArrayTempl<KEY, VALUE>::AutoSortedArrayTempl(int nSize){
	m_pBuffer				= NULL;
	m_memAlloc				= 0L;
	m_memUsed				= 0L;
	m_memBlockCt			= 0L;
	m_bOptimizeLargeArray	= FALSE;
	m_nGrowBy				= 5;
#ifdef _WINDOWS
	m_hHeap					= NULL;
#endif
	m_cmpFuncProc			= CompareDefault;
	m_keyGenFuncProc		= NULL;
//	m_nBlockSize			= sizeof(KEY) + sizeof(VALUE);
	m_nBlockSize			= sizeof(MemBlock);
	
	// Zero default key and value variables.
	memset(&m_keyDefault, 0, sizeof(KEY));
	memset(&m_valueDefault, 0, sizeof(VALUE));

	if( nSize > 0 )
		SetSize(nSize);
	}

template <class KEY, class VALUE>
AutoSortedArrayTempl<KEY, VALUE>::AutoSortedArrayTempl(){
	m_pBuffer				= NULL;
	m_memAlloc				= 0L;
	m_memUsed				= 0L;
	m_memBlockCt			= 0L;
	m_bOptimizeLargeArray	= FALSE;
	m_nGrowBy				= 10;
#ifdef _WINDOWS
	m_hHeap					= NULL;
#endif
	m_cmpFuncProc			= CompareDefault;
	m_keyGenFuncProc		= NULL;
//	m_nBlockSize			= sizeof(KEY) + sizeof(VALUE);
	m_nBlockSize			= sizeof(MemBlock);

	// Zero default key and value variables.
	memset(&m_keyDefault, 0, sizeof(KEY));
	memset(&m_valueDefault, 0, sizeof(VALUE));
	}

template <class KEY, class VALUE>
AutoSortedArrayTempl<KEY, VALUE>::~AutoSortedArrayTempl(){
	FreeMemory();
	}

template <class KEY, class VALUE>
void  
AutoSortedArrayTempl<KEY, VALUE>::Copy(AutoSortedArrayTempl* src){
	FreeMemory();
	int nCount       = src->GetCount();
	if( nCount ){
		AllocateMemory(nCount*sizeof(MemBlock));
		m_memUsed    = nCount*sizeof(MemBlock);
		m_memBlockCt = nCount;
		memcpy(m_pBuffer, src->GetBuffer(), m_memUsed);
		}
	}

// 2/7/2009 [ZqrTalent]
template <class KEY, class VALUE>
BOOL	
AutoSortedArrayTempl<KEY, VALUE>::GetData(KEY lValue, VALUE& lData, int& nIndex){
	int nFind	= FindValue(lValue);
	if( nFind == -1 )
		return FALSE;
	nIndex		= nFind;
	lData		= GetData(nFind);
	return TRUE;
	}

template <class KEY, class VALUE>
void 
AutoSortedArrayTempl<KEY, VALUE>::Add(KEY lValue, VALUE lData){
	// allocate memory.////////////////////////////////////////
	if( m_memAlloc == m_memUsed )
		AllocateMemory(m_memAlloc+m_nGrowBy*m_nBlockSize);
	//---------------------------------------------------------
	// find insert place(index) of value & insert it. /////////
	bool		bExist;
	long		nInsert		= FindValuePlace(lValue, bExist);
	MemBlock*	dest		= &m_pBuffer[nInsert];
	long		size		= m_nBlockSize*(m_memBlockCt-nInsert);
	if( size > 0 )
		memmove(&m_pBuffer[nInsert + 1], dest, size);
	//---------------------------------------------------------
	// add new values into memory./////////////////////////////
	dest->k			= lValue;
	dest->v			= lData;

	//memcpy((void*)&dest->v, (void*)&lData, sizeof(VALUE));
	m_memUsed		+= m_nBlockSize; 
	m_memBlockCt	++;
	//---------------------------------------------------------
	}

template <class KEY, class VALUE> BOOL
AutoSortedArrayTempl<KEY, VALUE>::SetAt(KEY lValue, VALUE value){
	int nIndex = FindValue(lValue);
	if( nIndex == -1 ) return FALSE;
	m_pBuffer[nIndex].v = value;
	return TRUE;
	}

template <class KEY, class VALUE> VALUE&
AutoSortedArrayTempl<KEY, VALUE>::GetAt(KEY lValue){
	int nIndex = FindValue(lValue);
	if( nIndex == -1 ) return m_valueDefault;
	return m_pBuffer[nIndex].v;
	}

template <class KEY, class VALUE>
void 
AutoSortedArrayTempl<KEY, VALUE>::Set(int nIndex, KEY lValue, VALUE lData){
	ASSERT( nIndex > -1 && nIndex < m_memBlockCt );
	// find place of new value. //////////////////////////////////////
	bool bExist;
	long  nInsert   = FindValuePlace(lValue, bExist);
	//----------------------------------------------------------------
	// correct insert index of new value. ////////////////////////////
	KEY lOldValue = Get(nIndex);
	if( lValue > lOldValue ) nInsert --;
	//----------------------------------------------------------------
	// remove old block from array.///////////////////////////////////
	void* dest     = &m_pBuffer[nInsert+1];
	void* src      = &m_pBuffer[nIndex+1];
	PTR_HOLDER_INTEGER  size     = (PTR_HOLDER_INTEGER)dest-(PTR_HOLDER_INTEGER)src;
	if( size > 0 )
		memmove((void*)((PTR_HOLDER_INTEGER)src-m_nBlockSize), src, size);
	if( size < 0 )
		memmove(dest, (void*)((PTR_HOLDER_INTEGER)dest-m_nBlockSize), -size);
	//-----------------------------------------------------------------
	// set new element into array. ////////////////////////////////////
	m_pBuffer[nInsert].k = lValue;
	m_pBuffer[nInsert].v = lData;
	//-----------------------------------------------------------------
	}

template <class KEY, class VALUE>
void 
AutoSortedArrayTempl<KEY, VALUE>::Delete(int  nIndex,int  nCount){
	ASSERT( nIndex > -1 && nIndex < m_memBlockCt );
	ASSERT( (nIndex+nCount) <= m_memBlockCt      );

	// delete memory blocks from array./////////////////////////
	void* dest    = &m_pBuffer[nIndex];
	long  size    = m_nBlockSize*(m_memBlockCt-nIndex-nCount);
	if( size )
		memmove(dest, &m_pBuffer[nIndex + nCount], size);
	m_memBlockCt -= nCount;
	m_memUsed     = m_memBlockCt*m_nBlockSize;
	//----------------------------------------------------------
	}

template <class KEY, class VALUE>
void 
AutoSortedArrayTempl<KEY, VALUE>::DeleteLast(int nCount){
	// change count of memory blocks.//
	ASSERT( nCount <= m_memBlockCt );
	m_memBlockCt   -= nCount;
	m_memUsed       = m_memBlockCt*m_nBlockSize;
	//---------------------------------
	}

template <class KEY, class VALUE>
long 
AutoSortedArrayTempl<KEY, VALUE>::FindValuePlace(KEY lValue, bool& bExist){
	/////////////////////////////////////////////////////////
	bExist                = false;
	if( !m_memBlockCt ) return  0;

	int			   nRet	  = 0;
	int            nLast  = m_memBlockCt - 1;
	int            nStart = 0;
	int            nMid;
	KEY            lMid;

	while( nLast > nStart+1 ){
		nMid = nStart+(nLast-nStart)/2;
		lMid = m_pBuffer[nMid].k;
		nRet = (m_cmpFuncProc)((void*)&lMid, (void*)&lValue);

		if( !nRet ){
			bExist = true;
			return   nMid;
			}
		else
			( nRet == 1 ) ?	nLast = nMid : nStart = nMid;
		}
	
	lMid = m_pBuffer[nStart].k;
	nRet = (m_cmpFuncProc)((void*)&lMid, (void*)&lValue);
	if( nRet >= 0 ){
		bExist = (nRet == 0);
		return nStart;
		}
	else{
		lMid	= m_pBuffer[nLast].k;
		nRet	= (m_cmpFuncProc)((void*)&lMid, (void*)&lValue);
		bExist	= (nRet == 0);
		return ( nRet < 0 ) ? nLast+1 : nLast;
		}
	//--------------------------------------------------------
	//********************************************************
	}

template <class KEY, class VALUE>
int		
AutoSortedArrayTempl<KEY, VALUE>::FindFirst(KEY lValue){
	int nIndex		= FindValue(lValue);
	m_nLastFounded	= -1;
	m_lFindValue	= lValue;

	if( nIndex != -1 ){
		while( nIndex > 0 ){
			nIndex --;
			if( Get(nIndex) != lValue ){
				m_nLastFounded = nIndex + 1;
				return nIndex + 1;
				}
			}

		m_nLastFounded = 0;
		return 0;
		}

	m_nLastFounded	= -1;
	m_lFindValue	= -1;
	return -1;
	}

template <class KEY, class VALUE>
int		
AutoSortedArrayTempl<KEY, VALUE>::FindNext()
	{
	ASSERT( m_nLastFounded > -1 );
	if( m_nLastFounded == GetCount() - 1 ){
		m_nLastFounded	= -1;
		m_lFindValue	= -1;
		return -1;
		}

	if( Get(m_nLastFounded + 1) == m_lFindValue ){
		m_nLastFounded ++;
		return m_nLastFounded;
		}
	
	m_nLastFounded	= -1;
	m_lFindValue	= -1;
	return -1;
	}

template <class KEY, class VALUE>
void 
AutoSortedArrayTempl<KEY, VALUE>::AllocateMemory(int nNewSize)
	{
	if( !m_bOptimizeLargeArray ){
		// allocate memory if needed.///////////
		void* pTemp   = new BYTE[nNewSize];
		// copy & free old allocated memory.////
		if( m_pBuffer ){
			memcpy(pTemp, m_pBuffer, m_memAlloc);
			delete m_pBuffer;
			}
		//--------------------------------------
		m_pBuffer     = (MemBlock*)pTemp;
		m_memAlloc    = nNewSize;
		ASSERT( m_pBuffer );
		// ####################################
		}
	else{
#ifdef _WINDOWS
		if( !m_hHeap ){
			m_hHeap = HeapCreate(0L, nNewSize, 0L);
			ASSERT( m_hHeap );
			m_pBuffer = (MemBlock*)HeapAlloc(m_hHeap, 0L, nNewSize);
			}

		// Allocate or realocate memory . #####
		m_pBuffer	= (MemBlock*)HeapReAlloc(m_hHeap, 0L, m_pBuffer, nNewSize);
		m_memAlloc  = nNewSize;
#else
        // Allocate or realocate memory . #####
		m_pBuffer	= (MemBlock*)realloc(m_pBuffer, nNewSize);
		m_memAlloc  = nNewSize;
#endif
		ASSERT( m_pBuffer );
		// ####################################
		}
	}

template <class KEY, class VALUE>
void 
AutoSortedArrayTempl<KEY, VALUE>::FreeMemory()
	{
	// free allocated memory.//
#ifdef _WINDOWS
	if( m_pBuffer ){
		if( m_bOptimizeLargeArray ){
			HeapFree	(m_hHeap, 0L, m_pBuffer);
			HeapDestroy	(m_hHeap);
			}
		else
			delete m_pBuffer;
		}
    m_hHeap		 = NULL;
#else
    if( m_pBuffer )
        free(m_pBuffer);
#endif

	m_pBuffer    = NULL;
	m_memAlloc   = 0L;
	m_memUsed    = 0L;
	m_memBlockCt = 0L;
	//-------------------------
	}

template <class KEY, class VALUE>
int	
AutoSortedArrayTempl<KEY, VALUE>::CompareDefault(void* lpValue1, void* lpValue2)
	{        
	if( *((KEY*)lpValue1) == *((KEY*)lpValue2) )
		return 0;
	else
	if( *((KEY*)lpValue1) > *((KEY*)lpValue2) )
		return 1;
	return -1;
	}

template <class KEY, class VALUE>
int	
AutoSortedArrayTempl<KEY, VALUE>::CompareRects(RECT* lpValue1, RECT* lpValue2)
	{
	if( lpValue1->top > lpValue2->top )
		return 1;
	else
	if( lpValue1->top == lpValue2->top ){
		if( lpValue1->left > lpValue2->left )
			return 1;
		else
		if( lpValue1->left < lpValue2->left )
			return -1;
		return 0;
		}
	return -1;
	}

template <class KEY, class VALUE>
int	
AutoSortedArrayTempl<KEY, VALUE>::CompareEmpty(void* lpValue1, void* lpValue2){
	return 1;
	}

template <class KEY, class VALUE>
BOOL	
AutoSortedArrayTempl<KEY, VALUE>::KeyGenFromString(void* lpGenFrom, void* key){
	// lpGenFrom should be pointer to char array.
	if( lpGenFrom == NULL ) return FALSE;
	*((KEY*)key) = (KEY)DecodeString((LPCTSTR)lpGenFrom);
	return TRUE;
	}

template <class KEY, class VALUE>
BOOL
AutoSortedArrayTempl<KEY, VALUE>::SetSize(int nElemCount){
	DeleteAll();
	FreeExtra();

	AllocateMemory(nElemCount*m_nBlockSize);
	return TRUE;
	}

template <class KEY, class VALUE>
KEY
AutoSortedArrayTempl<KEY, VALUE>::operator[](int nIndex) { 
	return Get(nIndex);
	}

template <class KEY, class VALUE>
KEY& 
AutoSortedArrayTempl<KEY, VALUE>::Get(int nIndex){
	ASSERT( nIndex > -1 && nIndex < m_memBlockCt );
	return m_pBuffer[nIndex].k;
	}

template <class KEY, class VALUE>
VALUE&
AutoSortedArrayTempl<KEY, VALUE>::GetData(int  nIndex){
	ASSERT( nIndex > -1 && nIndex < m_memBlockCt );
	return m_pBuffer[nIndex].v;
	}

template <class KEY, class VALUE>
KEY&
AutoSortedArrayTempl<KEY, VALUE>::GetWithData(int  nIndex, VALUE& lData){
	ASSERT( nIndex > -1 && nIndex < m_memBlockCt );
	lData = m_pBuffer[nIndex].v;
	return m_pBuffer[nIndex].v;	
	}

template <class KEY, class VALUE>
int
AutoSortedArrayTempl<KEY, VALUE>::FindValue(KEY lValue){
	bool bExist;
	long nIndex = FindValuePlace(lValue, bExist);
	return bExist ? (int)nIndex : -1;
	}

template <class KEY, class VALUE>
int 
AutoSortedArrayTempl<KEY, VALUE>::GetCount(){
	return m_memBlockCt;
	}

template <class KEY, class VALUE>
void*
AutoSortedArrayTempl<KEY, VALUE>::GetBuffer(){
	return m_pBuffer;
	}

template <class KEY, class VALUE>
void  
AutoSortedArrayTempl<KEY, VALUE>::DeleteAll(){
	m_memUsed	 = 0;
	m_memBlockCt = 0;
	}

template <class KEY, class VALUE>
void
AutoSortedArrayTempl<KEY, VALUE>::FreeExtra(){
	FreeMemory();
	}

template <class KEY, class VALUE>
void		
AutoSortedArrayTempl<KEY, VALUE>::OptimizeForLargeData(BOOL bOptimize){
	m_bOptimizeLargeArray = bOptimize;
	}

template <class KEY, class VALUE>
void	
AutoSortedArrayTempl<KEY, VALUE>::SetCompareFunction(CompareProc cmpProc){
	m_cmpFuncProc = cmpProc;
	}

template <class KEY, class VALUE>
CompareProc
AutoSortedArrayTempl<KEY, VALUE>::GetCompareFunc(){
	return m_cmpFuncProc;
	}

template <class KEY, class VALUE>
BOOL	
AutoSortedArrayTempl<KEY, VALUE>::SetData(int nIndex, VALUE& lData){
	if( nIndex < 0 || nIndex > m_memBlockCt )
		return FALSE;

	m_pBuffer[nIndex].v = lData;
	return TRUE;
	}

template <class KEY, class VALUE>
KEY&
AutoSortedArrayTempl<KEY, VALUE>::GetKey(int nIndex){
	if( nIndex < 0 || nIndex > m_memBlockCt )
		return m_keyDefault;
	return m_pBuffer[nIndex].k;
	}

template <class KEY, class VALUE>
VALUE&	
AutoSortedArrayTempl<KEY, VALUE>::GetValueByIndex(int nIndex){
	if( nIndex < 0 || nIndex > m_memBlockCt )
		return m_valueDefault;
	return m_pBuffer[nIndex].v;
	}

template <class KEY, class VALUE>
VALUE&	
AutoSortedArrayTempl<KEY, VALUE>::GetValue(KEY key){
	int nIndex = FindValue(key);
	if( nIndex == -1 )
		return m_valueDefault;
	return m_pBuffer[nIndex].v;
	}

template <class KEY, class VALUE>
BOOL	
AutoSortedArrayTempl<KEY, VALUE>::GetBoth(int nIndex, KEY& key, VALUE& value){
	if( nIndex < 0 || nIndex > m_memBlockCt )
		return FALSE;
	key		= m_pBuffer[nIndex].k;
	value	= m_pBuffer[nIndex].v;
	return TRUE;
	}

template <class KEY, class VALUE>
void		
AutoSortedArrayTempl<KEY, VALUE>::Add_KeyGen(void* lpGenFrom, VALUE value){
	if( !m_keyGenFuncProc ) return; 
	KEY key;
	if( !(m_keyGenFuncProc)(lpGenFrom, (void*)&key) )
		return;
	Add(key, value);
	}

template <class KEY, class VALUE>
void		
AutoSortedArrayTempl<KEY, VALUE>::SetAt_KeyGen(void* lpGenFrom, VALUE value){
	if( !m_keyGenFuncProc ) return; 
	KEY key;
	if( !(m_keyGenFuncProc)(lpGenFrom, (void*)&key) )
		return;
	SetAt(key, value);
	}

template <class KEY, class VALUE>
void		
AutoSortedArrayTempl<KEY, VALUE>::Set_KeyGen(int nIndex, void* lpGenFrom, VALUE value){
	if( !m_keyGenFuncProc ) return; 
	KEY key;
	if( !(m_keyGenFuncProc)(lpGenFrom, (void*)&key) )
		return;
	SetAt(nIndex, key, value);
	}

template <class KEY, class VALUE>
void		
AutoSortedArrayTempl<KEY, VALUE>::SetKeyGenFunction(KeyGenProc keyGenProc){
	m_keyGenFuncProc = keyGenProc;
	}

template <class KEY, class VALUE>
VALUE&		
AutoSortedArrayTempl<KEY, VALUE>::GetValue_KeyGen(void* lpKey){
	if( !m_keyGenFuncProc ) return m_valueDefault; 
	KEY key;
	if( !(m_keyGenFuncProc)(lpKey, (void*)&key) )
		return m_valueDefault;
	return GetValue(key);
	}
