#pragma once
#include "../_platformCompat/PlatformCompat.h"

// #############################################################
//	class: List
//	author: ZqrTalent
//	date: 6/15/2011
//	requirements: struct or class Elem must have named members prev and next.
//	purpose: Useful to create chain of objects.
// #############################################################

#pragma pack(1)

template <class Elem>
class List
{
public:
	List ();
	~List();

	typedef struct ListElemTag{
		ListElemTag*	prev;
		ListElemTag*	next;
		Elem*			val;
	} ListElem, *PListElem;

	struct HeadElem{
		ListElem* pFirstElem;
	};

	// Retrieves element by index.
	Elem*	operator[]		(int nIndex);
	// Retrieves element by index.
	Elem*	GetAt			(int nIndex);
	// Set element at
	bool	SetAt					(int nIndex, Elem* pNewElem);
	// Finds element by one of the elements memeber which is string type. {Case sensitive}
	int		Find					(Elem* pFind);
	int		IndexOf					(Elem* pFind) {return Find(pFind);};
	// Finds element by one of the elements memeber which is string type. {Case sensitive}
	Elem*	FindWithCharArrayCase	(int offset, const TCHAR* pszFind);
	// Finds element by one of the elements memeber which is string type.
	Elem*	FindWithCharArray		(int offset, const TCHAR* pszFind);
	// Removes element from chain by element.
	bool	Remove					(Elem* pElem, bool bDeleteObject = true);
	// Removes element from chain by index.
	bool	RemoveAt				(int nIndex, bool bDeleteObject = true);
	// Adds element or chain of elements.
	// Returns added elements count.
	int		Add						(Elem* pElem, bool bAddAsFirst = false);	
	// Retrieves count of elements.
	int		GetCount				();
	// Retrieves current element index.
	int		GetCurrentIdx			();
	// Retrives current element.
	Elem*	GetCurrent				();
	// Deletes all elements.
	// Returns deleted elem count.
	int		RemoveAll				(bool bDestroyValues = true);
	// Set this flag to destroy objects automatically.
	void	SetDestroyObjects		(bool bDestroy){m_bDestroyObjects = bDestroy;};

private:
	HeadElem	m_headElem;
	ListElem*	m_pLastElem;
	ListElem*	m_pCurrElem;
	int			m_nCurrentIdx;
	int			m_nCount;
	int			m_nNextPtrOffset;
	int			m_nPrevPtrOffset;
	bool		m_bDestroyObjects;
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
template <class Elem>
List<Elem>::List(){
	m_headElem.pFirstElem	= NULL;
	m_pCurrElem				= NULL;
	m_pLastElem				= NULL;
	m_nCount				= 0;
	m_nCurrentIdx			= -1;
	m_nNextPtrOffset		= offsetof(ListElem, next);
	m_nPrevPtrOffset		= offsetof(ListElem, prev);
	m_bDestroyObjects		= true;
	}

template <class Elem>
List<Elem>::~List(){
	RemoveAll(m_bDestroyObjects);
	}

template <class Elem> Elem*	
List<Elem>::operator[](int nIndex){
	return GetAt(nIndex);
	}

template <class Elem> bool	
List<Elem>::SetAt(int nIndex, Elem* pNewElem){
	if( nIndex < 0 )			
		return false;	
	Elem* pOld = GetAt(nIndex);
	if( m_nCurrentIdx == nIndex ){
		if( pOld ) delete pOld;
		m_pCurrElem->val = pNewElem;
		return true;
		}
	return false;
	}

template <class Elem> Elem*	
List<Elem>::GetAt(int nIndex){
	if( m_nCount <= nIndex || nIndex < 0 )
		return NULL;

	ListElem*	pElem = m_pCurrElem;
	int			nMove = (nIndex - m_nCurrentIdx);
	if( nMove > 0 ){
		while( nMove > 0 ){
			ASSERT( pElem );
			pElem = (ListElem*)((PTR_HOLDER_INTEGER*)((PTR_HOLDER_INTEGER)(pElem) + m_nNextPtrOffset))[0];
			nMove --;
			}
		}
	else{
		while( nMove < 0 ){
			ASSERT( pElem );
			pElem = (ListElem*)((PTR_HOLDER_INTEGER*)((PTR_HOLDER_INTEGER)(pElem) + m_nPrevPtrOffset))[0];
			nMove ++;
			}
		}

	if( pElem ){
		m_nCurrentIdx	= nIndex;
		m_pCurrElem		= pElem;
		return pElem->val;
		}
	return NULL;
	}

template <class Elem> Elem*	
List<Elem>::FindWithCharArrayCase(int offset, const TCHAR* pszFind){
	ListElem*	pElem = m_headElem.pFirstElem;
	while( pElem ){
		if( !_tcsicmp((TCHAR*)((PTR_HOLDER_INTEGER*)((PTR_HOLDER_INTEGER)(pElem) + offset)), pszFind) ){
			return pElem->val;
			}
		pElem = (ListElem*)((PTR_HOLDER_INTEGER*)((PTR_HOLDER_INTEGER)(pElem) + m_nNextPtrOffset))[0];
		}
	return NULL;
	}

template <class Elem> Elem*	
List<Elem>::FindWithCharArray(int offset, const TCHAR* pszFind){
	ListElem*	pElem = m_headElem.pFirstElem;
	while( pElem ){
		if( !_tcsicmp((TCHAR*)((PTR_HOLDER_INTEGER*)((PTR_HOLDER_INTEGER)(pElem) + offset)), pszFind) )
			return pElem->val;
		pElem = (ListElem*)((PTR_HOLDER_INTEGER*)((PTR_HOLDER_INTEGER)(pElem) + m_nNextPtrOffset))[0];
		}
	return NULL;
	}

template <class Elem> int
List<Elem>::Add(Elem* pElem, bool bAddAsFirst /*= false*/){
	// ASSERT(pElem);
	// Determine count of added element(s).
#ifdef _DEBUG
	ListElem* pFirst	= (ListElem*)_malloc_dbg(sizeof(ListElem), _NORMAL_BLOCK, __FILE__, __LINE__);
#else
	ListElem*	pFirst	= new ListElem;
#endif
	pFirst->next		= NULL;
	pFirst->prev		= NULL;
	pFirst->val			= pElem;

	ListElem*	pLast	= (ListElem*)((PTR_HOLDER_INTEGER*)((PTR_HOLDER_INTEGER)(pFirst) + m_nNextPtrOffset))[0];
	int			nCount	= 1;
	if( pLast ){
		nCount ++;
		while( (ListElem*)((PTR_HOLDER_INTEGER*)((PTR_HOLDER_INTEGER)(pLast) + m_nNextPtrOffset))[0] != NULL ){
			pLast	= (ListElem*)((PTR_HOLDER_INTEGER*)((PTR_HOLDER_INTEGER)(pLast) + m_nNextPtrOffset))[0];
			nCount ++;
			}
		}
	// ###################################

	if( !m_nCount || bAddAsFirst){
		if( pLast ){
			((PTR_HOLDER_INTEGER*)((PTR_HOLDER_INTEGER)(pLast) + m_nNextPtrOffset))[0]	= (PTR_HOLDER_INTEGER)m_headElem.pFirstElem;
			if( m_headElem.pFirstElem )
				((PTR_HOLDER_INTEGER*)((PTR_HOLDER_INTEGER)(m_headElem.pFirstElem) + m_nPrevPtrOffset))[0] = (PTR_HOLDER_INTEGER)pLast;
			}
		else{
			((PTR_HOLDER_INTEGER*)((PTR_HOLDER_INTEGER)(pFirst) + m_nNextPtrOffset))[0] = (PTR_HOLDER_INTEGER)m_headElem.pFirstElem;
			if( m_headElem.pFirstElem )
				((PTR_HOLDER_INTEGER*)((PTR_HOLDER_INTEGER)(m_headElem.pFirstElem) + m_nPrevPtrOffset))[0] = (PTR_HOLDER_INTEGER)pLast;
			}

		m_headElem.pFirstElem	= pFirst;
		m_pCurrElem				= pFirst;
		m_nCurrentIdx			= 0;
		m_nCount				+= nCount;

		if( !m_pLastElem )
			m_pLastElem = pFirst;
		}
	else{
		/* In that case when we can't add NULL object.
		if( GetAt(m_nCount-1) == NULL )
			return 0; // Fail to get last element.
			*/

		// Old code. 
		/*GetAt(m_nCount-1);
		if( m_nCurrentIdx != (m_nCount - 1) ){
			return -1;
			}*/

		// Replacement code.
		m_pCurrElem		=  m_pLastElem;
		m_nCurrentIdx	= m_nCount - 1;

		((PTR_HOLDER_INTEGER*)((PTR_HOLDER_INTEGER)(pLast ? pLast : pFirst) + m_nPrevPtrOffset))[0]	= (PTR_HOLDER_INTEGER)m_pCurrElem;	
		((PTR_HOLDER_INTEGER*)((PTR_HOLDER_INTEGER)(m_pCurrElem) + m_nNextPtrOffset))[0]				= (PTR_HOLDER_INTEGER)(pLast ? pLast : pFirst);
		m_pCurrElem		= pFirst;
		m_nCurrentIdx	++;
		m_nCount		+= nCount;
		m_pLastElem		= pFirst;
		}
	return nCount;
	}

template <class Elem> bool 
List<Elem>::Remove(Elem* pElem , bool bDeleteObject /*= true*/){
	if( !pElem )		return false;
	int		nLoop			= 0;
	Elem*	pValElemByIndex	= GetAt(nLoop);
	if( !pValElemByIndex ) return false;
	ListElem* pElemByIndex	= m_pCurrElem;

	while( pElemByIndex ){
		if( pElemByIndex->val == pElem ){
			// For fast performance. {{
			m_pCurrElem		= pElemByIndex; 
			m_nCurrentIdx	= nLoop;
			RemoveAt(nLoop, bDeleteObject);
			// }}
			return true;
			}
		// Goto next pointer.
		pElemByIndex = (ListElem*)((PTR_HOLDER_INTEGER*)((PTR_HOLDER_INTEGER)(pElemByIndex) + m_nNextPtrOffset))[0];
		nLoop ++;
		}
	return false;
	}

template <class Elem> bool 	
List<Elem>::RemoveAt(int nIndex , bool bDeleteObject /*= true*/){
	Elem*		pElemVal = GetAt(nIndex);
	if( !pElemVal ) return false;

	ListElem*	pElem		= m_pCurrElem;
	ListElem*	pElemNext	= pElem->next;
	ListElem*	pElemPrev	= pElem->prev;

	if( pElemNext )
		pElemNext->prev = pElemPrev;

	if( pElemPrev )
		pElemPrev->next = pElemNext;
	else // Set as first element.
		m_headElem.pFirstElem = pElemNext;

	if( m_nCount > 0 )
		m_nCount --;

	if( pElemNext == NULL ){ // When remove last element.
		m_pCurrElem		= pElemPrev; 
		m_nCurrentIdx	= m_nCount - 1;
		m_pLastElem		= pElemPrev;
		}
	else
		m_pCurrElem		= pElemNext; 

	if( bDeleteObject ){
		if( m_bDestroyObjects && pElemVal )
		delete pElemVal;
		}
		
	delete pElem;
	return true;
	}

template <class Elem> int
List<Elem>::GetCount(){
	return m_nCount;
	}

// Retrieves current element index.
template <class Elem> int
List<Elem>::GetCurrentIdx(){
	return m_nCurrentIdx;
	}

// Retrives current element.
template <class Elem> Elem*	
List<Elem>::GetCurrent(){
	return m_pCurrElem;
	}

// Deletes all elements.
// Returns deleted elem count.
template <class Elem> int		
List<Elem>::RemoveAll(bool bDestroyValues /*= true*/){
	int nCt = 0;
	ListElem* pElem = m_headElem.pFirstElem;
	
	if( bDestroyValues ){
		while( pElem ){
			ListElem* p = (ListElem*)((PTR_HOLDER_INTEGER*)((PTR_HOLDER_INTEGER)(pElem) + m_nNextPtrOffset))[0];
			if( pElem->val ){
				delete pElem->val;
				}
			delete pElem;
			pElem	= p;
			nCt		++;
			}
		}
	else{
		while( pElem ){
			ListElem* p = (ListElem*)((PTR_HOLDER_INTEGER*)((PTR_HOLDER_INTEGER)(pElem) + m_nNextPtrOffset))[0];
			delete pElem;
			pElem	= p;
			nCt		++;
			}
		}

	m_headElem.pFirstElem	= NULL;
	m_pLastElem				= NULL;
	m_nCount				= 0;
	m_nCurrentIdx			= -1;
	m_pCurrElem				= NULL;
	return 0;
	}

template <class Elem> int	
List<Elem>::Find(Elem* pFind){
	ListElem* pElem = m_headElem.pFirstElem;
	int nLoop = 0;
	while( pElem ){
		ListElem* p = (ListElem*)((PTR_HOLDER_INTEGER*)((PTR_HOLDER_INTEGER)(pElem) + m_nNextPtrOffset))[0];
		if( pElem->val == pFind )
			return nLoop;
		pElem	= p;
		nLoop ++;
		}
	return -1;
	}