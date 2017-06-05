#pragma once
#include "../Array/List.h"

class	VirtualRowSet;
class	Serializable;
struct	CopyFieldInfo;

typedef bool (*Copy_Type)(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo);

struct CopyFieldInfo{
	int				nVirtualField;				// Virtual field index.
	VariableInfo*	pVarInfo;					// Variable info.
	Copy_Type		copyIntoObjectMethod;		// Method by specified field type, to copy into object variable.
	Copy_Type		copyIntoVRowMethod;			// Method by specified field type, to copy into virtual row field.
};

class DataFieldCopier
{
public:
	DataFieldCopier(VirtualRowSet* pVRowSet, Serializable* pDataObject, bool bUseDbFieldName = true);
	virtual	~DataFieldCopier();

	int				CopyFromVRow				(VirtualRowSet* pVRowSet, Serializable* pDataObject);	// Returns number of fields copied.
	int				CopyFromVRowIntoEnumerable	(VirtualRowSet* pVRowSet, EnumerableObject<Serializable>* pEnum);	// Returns number objects set into enumerable.
	int				CopyFromSerializable		(VirtualRowSet* pVRowSet, Serializable* pDataObject);	// Returns number of fields copied.

protected:
	bool			Initialize					(VirtualRowSet* pVRowSet, Serializable* pDataObject, bool bUseDbFieldName = true);

	static bool		CopyIntoObject_Bool			(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo);
	static bool		CopyIntoObject_Int8			(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo);
	static bool		CopyIntoObject_Int16		(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo);
	static bool		CopyIntoObject_Int32		(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo);
	static bool		CopyIntoObject_Int64		(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo);
	static bool		CopyIntoObject_Float		(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo);
	static bool		CopyIntoObject_Double		(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo);
	static bool		CopyIntoObject_DateTime		(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo);
	static bool		CopyIntoObject_Binary		(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo);
	static bool		CopyIntoObject_String		(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo);

	static bool		CopyIntoVRow_Bool			(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo);
	static bool		CopyIntoVRow_Int8			(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo);
	static bool		CopyIntoVRow_Int16			(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo);
	static bool		CopyIntoVRow_Int32			(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo);
	static bool		CopyIntoVRow_Int64			(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo);
	static bool		CopyIntoVRow_Float			(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo);
	static bool		CopyIntoVRow_Double			(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo);
	static bool		CopyIntoVRow_DateTime		(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo);
	static bool		CopyIntoVRow_Binary			(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo);
	static bool		CopyIntoVRow_String			(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo);


protected:
	List<CopyFieldInfo>						m_listCopyInfos;
};