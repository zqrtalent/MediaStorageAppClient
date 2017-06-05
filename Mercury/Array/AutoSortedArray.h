//////////////////////////////////////////////////////
//                                                  //
// started [22.09.2006]                             // 
//                                                  //
// Automatically sorted array.                      //
//                                                  //
// Author:  ZqrTalent                               //
//                                                  //
//////////////////////////////////////////////////////

#if !defined(AFX_AUTOSORTEDARRAY_H_INCLUDED)
#define      AFX_AUTOSORTEDARRAY_H_INCLUDED

#include "../_platformCompat/PlatformCompat.h"

#ifndef ASARRAY_BLOCK_SIZE
	#define ASARRAY_BLOCK_SIZE 2*sizeof(void*)
#endif

// define compare function.
typedef int (*CompareProc)(void* lpValue1, void* lpValue2);

// disable typecast warnings.
#pragma warning(disable:4311 4312)
#pragma pack(1)

class AutoSortedArray : BaseClass
{
public:
	AutoSortedArray ();
	virtual ~AutoSortedArray();

	void*	operator[]			(int nIndex){return Get(nIndex);};
	void	Copy				(AutoSortedArray* src);
	void	Add					(void* lValue, void* lData  = 0L);
	void	AddAsLong			(long lValue, long lData  = 0L);
	void	Set					(int  nIndex, void* lValue, void* lData = 0L);
	void	DeleteLast			(int  nCount);
	void	DeleteAll			(){m_memUsed = 0; m_memBlockCt = 0;};
	void	FreeExtra			(){FreeMemory();};
	void	Delete				(int  nIndex, int  nCount = 1);
	void*	Get					(int  nIndex)
    {
        ASSERT( nIndex > -1 && nIndex < m_memBlockCt );
        return *(void**)((PTR_HOLDER_INTEGER)m_pBuffer + ASARRAY_BLOCK_SIZE*nIndex);
    };
	long	GetAsLong			(int  nIndex){return (long)Get(nIndex);};
	BOOL	GetData				(void* lValue, void*& lData, int& nIndex);
	void*	GetData				(int  nIndex)
    {
        ASSERT( nIndex > -1 && nIndex < m_memBlockCt );
        return *(void**)((PTR_HOLDER_INTEGER)m_pBuffer + ASARRAY_BLOCK_SIZE*nIndex + sizeof(void*));
    };
    
	long	GetDataAsLong		(int  nIndex){return (long)GetData(nIndex);};
	void*	GetWithData			(int  nIndex, void*& lData)
    {
        ASSERT( nIndex > -1 && nIndex < m_memBlockCt );
        void** pLong	= (void**)((PTR_HOLDER_INTEGER)m_pBuffer + ASARRAY_BLOCK_SIZE*nIndex);
        void* lRet		= *pLong;
        pLong ++;
        lData			= *pLong;
        return lRet;
    };
	UINT	GetCount			(){return m_memBlockCt;};
	void*	GetBuffer			(){return m_pBuffer;};

	BOOL	SetSize				(UINT nElemCount);

	// 2/7/2009 [ZqrTalent]
	BOOL	GetData				(void* lValue, void*& lData);
	// 10/7/2009 [ZqrTalent]
	BOOL	SetData				(int nIndex, void* lData)
    {
        if( nIndex < 0 || nIndex > m_memBlockCt )
            return FALSE;
        *(void**)((PTR_HOLDER_INTEGER)m_pBuffer + ASARRAY_BLOCK_SIZE*nIndex + sizeof(void*)) = lData;
        return TRUE;
	};

	// Find multiply values. ##################################
	int		FindFirst			(void* lValue);
	int		FindNext			();
	// ########################################################

	// finds value into array. returns -1 if not find.
	int		FindValue			(void* lValue)
    {
        bool bExist;
        int nIndex = FindValuePlace(lValue,bExist);
        return bExist ? nIndex : -1;
    };
	int		FindValuePlace		(void* lValue, bool& bExist);

	int		GetValueCount		(void* lValue){
		int nIndex = FindFirst(lValue);
		if( nIndex == -1 ) return 0;
		int nCt = 1;
		while( FindNext() != -1 ){
			nCt ++;
			}
		return nCt;
		}
	int		IndexOf				(void* lValue) { return FindValue(lValue);};

	void	OptimizeForLargeData(BOOL bOptimize){m_bOptimizeLargeArray = bOptimize;};

	void		SetCompareFunction	(CompareProc cmpProc){m_cmpFuncProc = cmpProc;};
    
	CompareProc GetCompareFunc		(){return m_cmpFuncProc;}
	void		SetParam			(void* lParam){m_lParam = lParam;}
	void*		GetParam			(){return m_lParam;}

public:
	static int	CompareDefault		(void* lpValue1, void* lpValue2);
	static int	CompareDesc			(void* lpValue1, void* lpValue2);
	static int	CompareEmpty		(void* lpValue1, void* lpValue2);
	static int	CompareEmptyReverse	(void* lpValue1, void* lpValue2);

private:
	BYTE*		m_pBuffer;				// allocated memory.
	int			m_memAlloc;				// allocated memory size.
	int			m_memUsed;				// used memory size.
	int			m_memBlockCt;			// count of memory blocks in array.
	BOOL		m_bOptimizeLargeArray;	// Optimize for large array.
	int			m_nGrowBy;
	HANDLE		m_hHeap;				// Handle of created Heap. (Used only in OptimizeForLargeData mode)
	int			m_nLastFounded;			// Last founded value index.
	void*		m_lFindValue;			// Find value.
	void*		m_lParam;				// Param of sorted array;

	CompareProc	m_cmpFuncProc;			// Compare function proc.
    //24 + 40 + 1
    
protected:
	void AllocateMemory	(int nNewSize);
	void FreeMemory		();
};
#endif // !defined(AFX_AUTOSORTEDARRAY_H_INCLUDED)

