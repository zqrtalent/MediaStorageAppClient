#pragma once
#include "../_platformCompat/PlatformCompat.h"
#include <stddef.h>

// #############################################################
//	class: ChainTempl
//	author: ZqrTalent
//	date: 6/15/2011
//	requirements: struct or class Elem must have named members prev and next.
//	purpose: Useful to create chain of objects.
// #############################################################

#pragma pack(1)

template <class Elem>
class ChainTempl
{
public:
	ChainTempl ();
	~ChainTempl();
	
	// Retrieves element by index.
	Elem*	operator[]		(int nIndex);
	// Retrieves element by index.
	Elem*	GetAt			(int nIndex);
	// Finds element by one of the elements memeber which is string type. {Case sensitive}
	Elem*	FindWithCharArrayCase	(int offset, const TCHAR* pszFind);
	// Finds element by one of the elements memeber which is string type.
	Elem*	FindWithCharArray		(int offset, const TCHAR* pszFind);
	
	// Removes element from chain by element.
	bool	Remove					(Elem* pElem);
	// Removes element from chain by index.
	bool	RemoveAt				(int nIndex);
	// Adds element or chain of elements.
	// Returns added elements count.
	int		Add				(Elem* pElem, bool bAddAsFirst = false);
	// Retrieves count of elements.
	int		GetCount		();
	// Retrieves current element index.
	int		GetCurrentIdx	();
	// Retrives current element.
	Elem*	GetCurrent		();
	// Deletes all elements.
	// Returns deleted elem count.
	int		DeleteAll		();

private:

	struct HeadElem
	{
		Elem* pFirstElem;
	};

	HeadElem	m_headElem;
	Elem*		m_pCurrElem;
	int			m_nCurrentIdx;
	int			m_nCount;
	int			m_nNextPtrOffset;
	int			m_nPrevPtrOffset;
protected:
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
template <class Elem>
ChainTempl<Elem>::ChainTempl(){
	m_headElem.pFirstElem	= NULL;
	m_pCurrElem				= NULL;
	m_nCount				= 0;
	m_nCurrentIdx			= -1;
	m_nNextPtrOffset		= offsetof(Elem, next);
	m_nPrevPtrOffset		= offsetof(Elem, prev);
	}

template <class Elem>
ChainTempl<Elem>::~ChainTempl(){
	DeleteAll();
	}

template <class Elem> Elem*	
ChainTempl<Elem>::operator[](int nIndex){
	return GetAt(nIndex);
	}

template <class Elem> Elem*	
ChainTempl<Elem>::GetAt(int nIndex){
	if( m_nCount <= nIndex || nIndex < 0 )
		return NULL;

	Elem*	pElem = m_pCurrElem;
	int		nMove = (nIndex - m_nCurrentIdx);
	if( nMove > 0 ){
		while( nMove > 0 ){
			ASSERT( pElem );
			pElem = (Elem*)((PTR_HOLDER_INTEGER*)((PTR_HOLDER_INTEGER)(pElem) + m_nNextPtrOffset))[0];
			nMove --;
			}
		}
	else{
		while( nMove < 0 ){
			ASSERT( pElem );
			pElem = (Elem*)((PTR_HOLDER_INTEGER*)((PTR_HOLDER_INTEGER)(pElem) + m_nPrevPtrOffset))[0];
			nMove ++;
			}
		}

	if( pElem ){
		m_nCurrentIdx	= nIndex;
		m_pCurrElem		= pElem;
		}
	return pElem;
	}

template <class Elem> Elem*
ChainTempl<Elem>::FindWithCharArrayCase(int offset, const TCHAR* pszFind){
	Elem*	pElem = m_headElem.pFirstElem;
	while( pElem ){
		if( !_tcsicmp((TCHAR*)((PTR_HOLDER_INTEGER*)((PTR_HOLDER_INTEGER)(pElem) + offset)), pszFind) ){
			return pElem;
			}

		pElem = (Elem*)((PTR_HOLDER_INTEGER*)((PTR_HOLDER_INTEGER)(pElem) + m_nNextPtrOffset))[0];
		}
	return NULL;
	}

template <class Elem> Elem*	
ChainTempl<Elem>::FindWithCharArray(int offset, const TCHAR* pszFind){
	Elem*	pElem = m_headElem.pFirstElem;
	while( pElem ){
		if( !_tcsicmp((TCHAR*)((PTR_HOLDER_INTEGER*)((PTR_HOLDER_INTEGER)(pElem) + offset)), pszFind) ){
			return pElem;
			}
		pElem = (Elem*)((PTR_HOLDER_INTEGER*)((PTR_HOLDER_INTEGER)(pElem) + m_nNextPtrOffset))[0];
		}
	return NULL;
	}

template <class Elem> int
ChainTempl<Elem>::Add(Elem* pElem, bool bAddAsFirst /*= false*/){
	ASSERT(pElem);
	// Determine count of added element(s).
	Elem*	pFirst	= pElem;
	Elem*	pLast	= (Elem*)((PTR_HOLDER_INTEGER*)((PTR_HOLDER_INTEGER)(pElem) + m_nNextPtrOffset))[0];
	int		nCount	= 1;

	if( pLast ){
		nCount ++;
		while( (Elem*)((PTR_HOLDER_INTEGER*)((PTR_HOLDER_INTEGER)(pLast) + m_nNextPtrOffset))[0] != NULL ){
			pLast	= (Elem*)((PTR_HOLDER_INTEGER*)((PTR_HOLDER_INTEGER)(pLast) + m_nNextPtrOffset))[0];
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

		m_headElem.pFirstElem	= pElem;
		m_pCurrElem				= pElem;
		m_nCurrentIdx			= 0;
		m_nCount				+= nCount;
		}
	else{
		if( GetAt(m_nCount-1) == NULL ){
			return 0; // Fail to get last element.
			}

		((PTR_HOLDER_INTEGER*)((PTR_HOLDER_INTEGER)(pLast ? pLast : pFirst) + m_nPrevPtrOffset))[0]	= (PTR_HOLDER_INTEGER)m_pCurrElem;	
		((PTR_HOLDER_INTEGER*)((PTR_HOLDER_INTEGER)(m_pCurrElem) + m_nNextPtrOffset))[0]				= (PTR_HOLDER_INTEGER)(pLast ? pLast : pFirst);
		m_pCurrElem		= pFirst;
		m_nCurrentIdx	++;
		m_nCount		+= nCount;
		}
	return nCount;
	}

template <class Elem> bool 
ChainTempl<Elem>::Remove(Elem* pElem){
	if( !pElem )		return false;
	int		nLoop			= 0;
	Elem*	pElemByIndex	= GetAt(nLoop);
	if( !pElemByIndex ) return false;

	while( pElemByIndex ){
		if( pElemByIndex == pElem ){
			// For fast performance. {{
			m_pCurrElem		= pElemByIndex; 
			m_nCurrentIdx	= nLoop;
			RemoveAt(nLoop);
			// }}
			return true;
			}
		// Goto next pointer.
		pElemByIndex = (Elem*)((PTR_HOLDER_INTEGER*)((PTR_HOLDER_INTEGER)(pElemByIndex) + m_nNextPtrOffset))[0];
		nLoop ++;
		}
	return false;
	}

template <class Elem> bool 	
ChainTempl<Elem>::RemoveAt(int nIndex){
	Elem*	pElem	  = GetAt(nIndex);
	if( !pElem ) return false;

	Elem*	pElemNext = (Elem*)((PTR_HOLDER_INTEGER*)((PTR_HOLDER_INTEGER)(pElem) + m_nNextPtrOffset))[0];
	Elem*	pElemPrev = (Elem*)((PTR_HOLDER_INTEGER*)((PTR_HOLDER_INTEGER)(pElem) + m_nPrevPtrOffset))[0];
	if( pElemNext )
		((PTR_HOLDER_INTEGER*)((PTR_HOLDER_INTEGER)(pElemNext) + m_nPrevPtrOffset))[0] = (PTR_HOLDER_INTEGER)pElemPrev;
	if( pElemPrev )
		((PTR_HOLDER_INTEGER*)((PTR_HOLDER_INTEGER)(pElemPrev) + m_nNextPtrOffset))[0] = (PTR_HOLDER_INTEGER)pElemNext;

	m_nCount --;
	if( !m_nCount ){
		m_headElem.pFirstElem	= NULL;
		m_pCurrElem				= NULL;
		m_nCurrentIdx			= -1;
		}
	else{// Remove first element of array.
		if( m_headElem.pFirstElem == pElem ){
			m_headElem.pFirstElem = pElemNext;
			m_pCurrElem				= NULL;
			m_nCurrentIdx			= 0;
			}
		else// Remove last element of array.
		if( m_nCount == nIndex ){
			m_pCurrElem				= pElemPrev;
			m_nCurrentIdx			= nIndex - 1;
			}
		}

	delete pElem;
	return true;
	}


template <class Elem> int
ChainTempl<Elem>::GetCount(){
	return m_nCount;
	}
	// Retrieves current element index.
template <class Elem> int
ChainTempl<Elem>::GetCurrentIdx(){
	return m_nCurrentIdx;
	}
	// Retrives current element.
template <class Elem> Elem*	
ChainTempl<Elem>::GetCurrent(){
	return m_pCurrElem;
	}

	// Deletes all elements.
	// Returns deleted elem count.
template <class Elem> int		
ChainTempl<Elem>::DeleteAll(){
	int nCt = 0;
	Elem* pElem = m_headElem.pFirstElem;
	while( pElem ){
		Elem* p = (Elem*)((PTR_HOLDER_INTEGER*)((PTR_HOLDER_INTEGER)(pElem) + m_nNextPtrOffset))[0];
		delete pElem;
		pElem	= p;
		nCt		++;
		}

	m_headElem.pFirstElem	= NULL;
	m_nCount				= 0;
	m_nCurrentIdx			= -1;
	m_pCurrElem				= NULL;
	return 0;
	}