#pragma once

class Serializable;
class EnumerableObject_IndexExtension
{
public:
	EnumerableObject_IndexExtension();
	~EnumerableObject_IndexExtension();

	bool				CreateIndex				(Serializable* pIndexContainerObject, TCHAR* pszVariableName, CompareProc compareMethod);
	bool				ModifyIndexVariable		(Serializable* pIndexContainerObject, TCHAR* pszVariableName, Serializable* pObjectModified, void* lpValuePointerNew);
	
	Serializable*		FindOne					(TCHAR* pszVariableName, void* lpValuePointer, int* pIndexOfFoundVariable = NULL);
	int					FindMulti				(TCHAR* pszVariableName, void* lpValuePointer, CPtrArray& arrResult);
	bool				AddInIndex				(Serializable* pObjNew);
	bool				RemoveFromIndex			(Serializable* pObjRemove);
	void				RemoveAll				();
	void				ClearIndexArrays		();
	AutoSortedArray*	GetVariableIndexArray	(TCHAR* pszVariableName);

protected:
	Serializable*		m_pIndexContainerObject;
	AutoSortedArray*	m_pIndexes;				// <VariableInfo* (Index variable info)> <AutoSortedArray* (<void* (Type of it depends on variable type. for int it will be int*)><VALUE*>)>

};