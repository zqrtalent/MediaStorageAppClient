#include "Serializable.h"
#include "EnumerableObject_IndexExtension.h"

EnumerableObject_IndexExtension::EnumerableObject_IndexExtension(){
	m_pIndexContainerObject = NULL;
	m_pIndexes				= NULL;
	}

EnumerableObject_IndexExtension::~EnumerableObject_IndexExtension(){
	// Destroy index array.
	if( m_pIndexes ){
		int nLoop = 0, nCt = m_pIndexes->GetCount();
		while( nLoop < nCt ){
			AutoSortedArray* pArrIndex = (AutoSortedArray*)m_pIndexes->GetData(nLoop);
			if( pArrIndex )
				delete pArrIndex;
			nLoop ++;
			}

		delete m_pIndexes;
		m_pIndexes = NULL;
		}
	m_pIndexContainerObject = NULL;
	}

bool
EnumerableObject_IndexExtension::CreateIndex(Serializable* pIndexContainerObject, TCHAR* pszVariableName, CompareProc compareMethod){
	VariableInfo* pInfo = pIndexContainerObject->GetVariableInfo(pszVariableName);
	if( !pInfo ){
		ASSERT(FALSE);
		return false; // Variable with that name not exists !!!.
		}

	if( !m_pIndexes )
		m_pIndexes = new AutoSortedArray();

	AutoSortedArray* pArrForIndex = new AutoSortedArray();
	pArrForIndex->SetCompareFunction(compareMethod);
	m_pIndexes->Add((void*)pInfo, (void*)pArrForIndex);

	m_pIndexContainerObject = pIndexContainerObject;
	return true;
	}

bool
EnumerableObject_IndexExtension::ModifyIndexVariable(Serializable* pIndexContainerObject, TCHAR* pszVariableName, Serializable* pObjectModified, void* lpValuePointerNew){
	if( !m_pIndexes ) return false;
	VariableInfo* pInfo = m_pIndexContainerObject->GetVariableInfo(pszVariableName);
	if( !pInfo ) return false;		// Variable with that name not exists !!!.
	AutoSortedArray*	pArrIndex	=  GetVariableIndexArray(pszVariableName);
	if( !pArrIndex ) return false;

	void*	lpValuePointerOld	=  pObjectModified->GetVariablePtr(pszVariableName);
	int		nIndex				= pArrIndex->FindFirst((void*)lpValuePointerOld);
	while( nIndex != -1 ){
		if( pArrIndex->GetData(nIndex) == (void*)pObjectModified ){
			// Found exact one.
			void*			lpValueOrig = (void*)pArrIndex->Get(nIndex);
			// Set variable value.
			pObjectModified->SetVariable(pInfo, lpValuePointerNew);
			pArrIndex->Delete(nIndex);
			pArrIndex->Add(lpValueOrig, pObjectModified);
			return true;
			}
		nIndex = pArrIndex->FindNext();
		}

	// Temp
	if( nIndex == -1 ){
		int* lpVal;
		Serializable* lpData;
		for(int i=0; i<pArrIndex->GetCount(); i++){
			lpVal = (int*)pArrIndex->Get(i);
			lpData = (Serializable*)pArrIndex->GetData(i);
			
			int n = 9;

			}
		}
	// Temp

	return false;
	}

Serializable*
EnumerableObject_IndexExtension::FindOne(TCHAR* pszVariableName, void* lpValuePointer, int* pIndexOfFoundVariable /*= NULL*/){
	if( !m_pIndexes ) return NULL;

	VariableInfo* pInfo = m_pIndexContainerObject->GetVariableInfo(pszVariableName);
	if( !pInfo ) return NULL;		// Variable with that name not exists !!!.

	int	nIndex	= m_pIndexes->IndexOf((void*)pInfo);
	if( nIndex == -1 ) return NULL; // Variable is not indexed.

	AutoSortedArray*	pArrIndex	= (AutoSortedArray*)m_pIndexes->GetData(nIndex);
	if( !pArrIndex ) return NULL;

	nIndex	= pArrIndex->IndexOf((void*)lpValuePointer);
	if( pIndexOfFoundVariable )
		*pIndexOfFoundVariable = nIndex;
	if( nIndex == -1 ) return NULL; // Not have been found !!!
	return (Serializable*)pArrIndex->GetData(nIndex);
	}

int
EnumerableObject_IndexExtension::FindMulti(TCHAR* pszVariableName, void* lpValuePointer, CPtrArray& arrResult){
	if( !m_pIndexes ) return 0;

	VariableInfo* pInfo = m_pIndexContainerObject->GetVariableInfo(pszVariableName);
	if( !pInfo ) return 0;		// Variable with that name not exists !!!.

	int	nIndex	= m_pIndexes->IndexOf((void*)pInfo);
	if( nIndex == -1 ) return 0; // Variable is not indexed.

	AutoSortedArray*	pArrIndex	= (AutoSortedArray*)m_pIndexes->GetData(nIndex);
	if( !pArrIndex ) return 0;

	nIndex = pArrIndex->FindFirst((void*)lpValuePointer);
	while( nIndex != -1 ){
		arrResult.push_back(pArrIndex->GetData(nIndex));
		nIndex = pArrIndex->FindNext();
		}
	return arrResult.size();
	}

bool
EnumerableObject_IndexExtension::AddInIndex(Serializable* pObjNew){
	if( !m_pIndexes ) return false;
	int nLoop	= 0;
	int nCt		= m_pIndexes->GetCount();
	while( nLoop < nCt ){
		AutoSortedArray*	pArrIndex	= (AutoSortedArray*)m_pIndexes->GetData(nLoop);
		VariableInfo*		pInfo		= (VariableInfo*)m_pIndexes->Get(nLoop);
		if( pArrIndex ){
			// Variable pointer.
			void*	varPtr	= (void*)&((LPBYTE)pObjNew)[pInfo->m_nOffset];
//			ASSERT( pArrIndex->IndexOf((long)varPtr) == -1 );
			pArrIndex->Add(varPtr, pObjNew);
			}
		nLoop ++;
		}
	return true;
	}

bool
EnumerableObject_IndexExtension::RemoveFromIndex(Serializable* pObjRemove){
	if( !m_pIndexes ) return false;
	int nLoop	= 0;
	int nCt		= m_pIndexes->GetCount();
	while( nLoop < nCt ){
		AutoSortedArray*	pArrIndex	= (AutoSortedArray*)m_pIndexes->GetData(nLoop);
		VariableInfo*		pInfo		= (VariableInfo*)m_pIndexes->Get(nLoop);
		if( pArrIndex ){
			// Variable pointer.
			void*	varPtr	= (void*)&((LPBYTE)pObjRemove)[pInfo->m_nOffset];
			int		nIndex	= pArrIndex->FindFirst(varPtr);

			while( nIndex != -1 ){
				if( pArrIndex->GetData(nIndex) == pObjRemove ){
					// Found exact one.
					pArrIndex->Delete(nIndex);
					break;
					}
				nIndex = pArrIndex->FindNext();
				}
			}
		nLoop ++;
		}
	return true;
	}

void
EnumerableObject_IndexExtension::RemoveAll(){
	if( !m_pIndexes ) return;
	int nLoop	= 0;
	int nCt		= m_pIndexes->GetCount();

	while( nLoop < nCt ){
		AutoSortedArray* pArrIndex = (AutoSortedArray*)m_pIndexes->GetData(nLoop);
		if( pArrIndex )
			delete pArrIndex;
		nLoop ++;
		}
	m_pIndexes->DeleteAll();
	}

void
EnumerableObject_IndexExtension::ClearIndexArrays(){
	if( !m_pIndexes ) return;
	int nLoop	= 0;
	int nCt		= m_pIndexes->GetCount();

	while( nLoop < nCt ){
		AutoSortedArray* pArrIndex = (AutoSortedArray*)m_pIndexes->GetData(nLoop);
		if( pArrIndex ){
			pArrIndex->DeleteAll();
			pArrIndex->FreeExtra();
			}
		nLoop ++;
		}
	}

AutoSortedArray*
EnumerableObject_IndexExtension::GetVariableIndexArray(TCHAR* pszVariableName){
	if( !m_pIndexes ) return NULL;
	VariableInfo* pInfo = m_pIndexContainerObject->GetVariableInfo(pszVariableName);
	if( !pInfo ) return NULL;		// Variable with that name not exists !!!.
	int	nIndex	= m_pIndexes->IndexOf(pInfo);
	if( nIndex == -1 ) return NULL; // Variable is not indexed.
	return (AutoSortedArray*)m_pIndexes->GetData(nIndex);
	}