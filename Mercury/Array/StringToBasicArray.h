#pragma once
#include <string>
#include "List.h"
#include "AutoSortedArrayTempl.h"

#pragma pack(1)

template <class VALUE>
class StringToBasicArray
{
public:
	StringToBasicArray();
	virtual ~StringToBasicArray();
    
    typedef struct StringToBasicItem_tag{
        _string				sKey;
        VALUE					value;
        UINT					nKeyCode; // Decoded key string.
        StringToBasicItem_tag*	next;
        StringToBasicItem_tag*	prev;
    } StringToBasicItem;

public:
	VALUE	operator[]				(const _string sKey);
	BOOL	Add						(const _string sKey, VALUE val);
	BOOL	SetValue				(const _string sKey, VALUE val);
	BOOL	SetValueByIndex			(int nIndex, VALUE val);
	BOOL	SetKeyByIndex			(int nIndex, const _string sKey);
	BOOL	Delete					(const _string sKey);
	BOOL	DeleteByIndex			(int nIndex);
	void	DeleteAll				();
	int		GetCount				(){return m_arrKeys.GetCount();}		

	bool	KeyExists				(const _string sKey, int* pKeyIndex = NULL);
    const _string GetKey		(int nIndex);
	VALUE	GetValueByIndex			(int nIndex);
	VALUE	GetValue				(const _string sKey);

	bool	SetCurrentIndex			(int nIndex);
	bool	SetCurrentValue			(VALUE val);
	int		GetCurrentIndex			();
	VALUE	GetCurrentValue			();
	const _string	GetCurrentKey	();

protected:
	List<StringToBasicItem>							m_listItems;
	AutoSortedArrayTempl<UINT, StringToBasicItem*>	m_arrKeys;
	int												m_nCurrent;
	VALUE											m_defValue;
};

template<class VALUE>
StringToBasicArray<VALUE>::StringToBasicArray(){
	m_nCurrent = -1;
}

template<class VALUE>
StringToBasicArray<VALUE>::~StringToBasicArray(){
	DeleteAll();
}


template<class VALUE> VALUE
StringToBasicArray<VALUE>::operator[](const _string sKey){
	return GetValue(sKey);
}

template<class VALUE> BOOL
StringToBasicArray<VALUE>::Add(const _string sKey, VALUE val){
	_string sKeyLower = sKey;
    stringEx::MakeLower(sKeyLower);
	UINT nCode = DecodeString(stringEx::GetBuffer(sKeyLower));
    
	if( m_arrKeys.FindValue(nCode) != -1 )
		return FALSE; // Key exists.
    
	StringToBasicItem* pItemNew	= new StringToBasicItem();
	pItemNew->nKeyCode			= nCode;
	pItemNew->sKey				= sKey;
	pItemNew->value				= val;
	pItemNew->next				= NULL;
	pItemNew->prev				= NULL;
	m_arrKeys.Add(nCode, pItemNew);
	m_listItems.Add(pItemNew);
	return TRUE;
}

template <class VALUE>
BOOL
StringToBasicArray<VALUE>::SetValue(const _string sKey, VALUE val){
    _string sKeyLower = sKey;
    stringEx::MakeLower(sKeyLower);
	UINT nCode = DecodeString(stringEx::GetBuffer(sKeyLower));
	int nIndex = m_arrKeys.FindValue(nCode);
	if( nIndex == -1 ){
		return FALSE; // Key not exists.
		}
    
	StringToBasicItem* pItem = m_listItems.GetAt(nIndex);
	ASSERT(pItem);
	pItem->value = val;
	return TRUE;
	}

template <class VALUE>
BOOL
StringToBasicArray<VALUE>::SetValueByIndex(int nIndex, VALUE val){
	if( nIndex < 0 || nIndex >= m_arrKeys.GetCount() )
		return FALSE;
    
	StringToBasicItem* pItem = m_listItems.GetAt(nIndex);
	ASSERT(pItem);
	pItem->value = val;
	return TRUE;
}

template <class VALUE>
BOOL
StringToBasicArray<VALUE>::SetKeyByIndex(int nIndex, const _string sKey){
	if( nIndex < 0 || nIndex >= m_arrKeys.GetCount() )
		return FALSE;
    
    _string sKeyLower = sKey;
    stringEx::MakeLower(sKeyLower);
	UINT nCodeNew = DecodeString(stringEx::GetBuffer(sKeyLower));
    
	StringToBasicItem* pItem = m_listItems.GetAt(nIndex);
	ASSERT(pItem);
	
	int nIndexBy = m_arrKeys.FindValue(pItem->nKeyCode);
	ASSERT(nIndexBy != -1);
	if( nIndexBy != -1 ){
		m_arrKeys.Set(nIndexBy, nCodeNew, pItem);
    }
    
	pItem->nKeyCode = nCodeNew;
	pItem->sKey = sKeyLower;
	return TRUE;
}

template <class VALUE>
BOOL
StringToBasicArray<VALUE>::Delete(const _string sKey){
	_string sKeyLower = sKey;
    stringEx::MakeLower(sKeyLower);
	UINT nCode = DecodeString(stringEx::GetBuffer(sKeyLower));
    
	int nIndexBy = m_arrKeys.FindValue(nCode);
	if( nIndexBy != -1 ){
		// Remove from list.
		m_listItems.Remove(m_arrKeys.GetData(nIndexBy));
		// Remove from sorted array.
		m_arrKeys.Delete(nIndexBy);
		return TRUE;
    }
	return FALSE;
}

template <class VALUE>
BOOL
StringToBasicArray<VALUE>::DeleteByIndex(int nIndex){
	if( nIndex < 0 || nIndex >= m_arrKeys.GetCount() )
		return FALSE;
    
	StringToBasicItem* pItem = m_listItems.GetAt(nIndex);
	ASSERT(pItem);
    
	int nIndexBy = m_arrKeys.FindValue(pItem->nKeyCode);
	if( nIndexBy != -1 ){
		// Remove from list.
		m_listItems.Remove(m_arrKeys.GetData(nIndexBy));
		// Remove from sorted array.
		m_arrKeys.Delete(nIndexBy);
		return TRUE;
    }
	return FALSE;
}

template <class VALUE>
void
StringToBasicArray<VALUE>::DeleteAll(){
	// Remove from list.
	m_listItems.RemoveAll(true);
	// Remove from sorted array.
	m_arrKeys.DeleteAll();
	m_nCurrent = -1;
}

template <class VALUE>
bool
StringToBasicArray<VALUE>::KeyExists(const _string sKey, int* pKeyIndex /*= NULL*/){
    _string sKeyLower = sKey;
    stringEx::MakeLower(sKeyLower);
	UINT nCode = DecodeString(stringEx::GetBuffer(sKeyLower));
    
	int nIndexBy = m_arrKeys.FindValue(nCode);
	if( nIndexBy != -1 ){
		if( pKeyIndex != NULL )
			*pKeyIndex = m_listItems.Find((StringToBasicItem*)m_arrKeys.GetData(nIndexBy));
        //		if( pKeyIndex != NULL )
        //			*pKeyIndex = nIndexBy;
		return true;
    }
	return false;
}

template <class VALUE>
const _string
StringToBasicArray<VALUE>::GetKey(int nIndex){
	if( nIndex < 0 || nIndex >= m_arrKeys.GetCount() )
		return _T("");
	StringToBasicItem* pItem = m_listItems.GetAt(nIndex);
	ASSERT(pItem);
	return pItem->sKey;
}

template <class VALUE>
VALUE
StringToBasicArray<VALUE>::GetValueByIndex(int nIndex){
	if( nIndex < 0 || nIndex >= m_arrKeys.GetCount() )
		return m_defValue;
	StringToBasicItem* pItem = m_listItems.GetAt(nIndex);
	ASSERT(pItem);
	return pItem->value;
}

template <class VALUE>
VALUE
StringToBasicArray<VALUE>::GetValue(const _string sKey){
    _string sKeyLower = sKey;
    stringEx::MakeLower(sKeyLower);
	UINT nCode = DecodeString(stringEx::GetBuffer(sKeyLower));
    
	int nIndexBy = m_arrKeys.FindValue(nCode);
	if( nIndexBy != -1 ){
		StringToBasicItem* pItem = m_arrKeys.GetData(nIndexBy);
		ASSERT(pItem);
		return pItem->value;
    }
	return m_defValue;
}

template <class VALUE>
bool
StringToBasicArray<VALUE>::SetCurrentIndex(int nIndex){
	if( nIndex >= 0 && nIndex < GetCount() ){
		m_nCurrent = nIndex;
		return true;
    }
	return false;
}

template <class VALUE>
bool
StringToBasicArray<VALUE>::SetCurrentValue(VALUE val){
	StringToBasicItem* pItem = NULL;
	for(int i=0; i<m_listItems.GetCount(); i++){
		pItem = m_listItems.GetAt(i);
		if( !pItem ) continue;
		if( pItem->value == val ){
			m_nCurrent = i;
			return true;
        }
    }
	return false;
}

template <class VALUE>
int
StringToBasicArray<VALUE>::GetCurrentIndex(){
	if( !GetCount() ) return -1;
	if( m_nCurrent < 0 || m_nCurrent >= GetCount() )
		m_nCurrent = 0;
	return m_nCurrent;
}

template <class VALUE>
VALUE
StringToBasicArray<VALUE>::GetCurrentValue(){
	int nIndex = GetCurrentIndex();
	if( nIndex > -1 )
		return GetValueByIndex(nIndex);
	return 0;
}

template <class VALUE>
const _string
StringToBasicArray<VALUE>::GetCurrentKey(){
	int nIndex = GetCurrentIndex();
	if( nIndex > -1 )
		return GetKey(nIndex);		
	return _T("");
}