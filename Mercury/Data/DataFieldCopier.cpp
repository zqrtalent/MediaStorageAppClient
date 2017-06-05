#include "VRowSet.h"
#include "../Array/AutoSortedArray.h"
#include "../Serialize/Serializable.h"
#include "DataFieldCopier.h"

//==========================================================
//
//	class DataFieldCopier
//
//==========================================================

DataFieldCopier::DataFieldCopier(VirtualRowSet* pVRowSet, Serializable* pDataObject, bool bUseDbFieldName /*= true*/){
	ASSERT(pVRowSet && pDataObject);
	Initialize(pVRowSet, pDataObject, bUseDbFieldName);
	}

DataFieldCopier::~DataFieldCopier(){
	}

bool
DataFieldCopier::Initialize(VirtualRowSet* pVRowSet, Serializable* pDataObject, bool bUseDbFieldName /*= true*/){
	m_listCopyInfos.RemoveAll(true);
	AutoSortedArray	arrTypeToCopyIntoObjectMethod;
	arrTypeToCopyIntoObjectMethod.Add((void*)VariableType::VT_Bool,		(void*)DataFieldCopier::CopyIntoObject_Bool);
	arrTypeToCopyIntoObjectMethod.Add((void*)VariableType::VT_Int8,		(void*)DataFieldCopier::CopyIntoObject_Int8);
	arrTypeToCopyIntoObjectMethod.Add((void*)VariableType::VT_Int16,	(void*)DataFieldCopier::CopyIntoObject_Int16);
	arrTypeToCopyIntoObjectMethod.Add((void*)VariableType::VT_Int32,	(void*)DataFieldCopier::CopyIntoObject_Int32);
	arrTypeToCopyIntoObjectMethod.Add((void*)VariableType::VT_Int64,	(void*)DataFieldCopier::CopyIntoObject_Int64);
	arrTypeToCopyIntoObjectMethod.Add((void*)VariableType::VT_String,	(void*)DataFieldCopier::CopyIntoObject_String);
	arrTypeToCopyIntoObjectMethod.Add((void*)VariableType::VT_Date,		(void*)DataFieldCopier::CopyIntoObject_DateTime);
	arrTypeToCopyIntoObjectMethod.Add((void*)VariableType::VT_DateTime,(void*)DataFieldCopier::CopyIntoObject_DateTime);
	arrTypeToCopyIntoObjectMethod.Add((void*)VariableType::VT_Float,	(void*)DataFieldCopier::CopyIntoObject_Float);
	arrTypeToCopyIntoObjectMethod.Add((void*)VariableType::VT_Double,	(void*)DataFieldCopier::CopyIntoObject_Double);
	arrTypeToCopyIntoObjectMethod.Add((void*)VariableType::VT_Binary,	(void*)DataFieldCopier::CopyIntoObject_Binary);

	AutoSortedArray	arrTypeToCopyIntoVRowMethod;
	arrTypeToCopyIntoVRowMethod.Add((void*)VariableType::VT_Bool,		(void*)DataFieldCopier::CopyIntoVRow_Bool);
	arrTypeToCopyIntoVRowMethod.Add((void*)VariableType::VT_Int8,		(void*)DataFieldCopier::CopyIntoVRow_Int8);
	arrTypeToCopyIntoVRowMethod.Add((void*)VariableType::VT_Int16,		(void*)DataFieldCopier::CopyIntoVRow_Int16);
	arrTypeToCopyIntoVRowMethod.Add((void*)VariableType::VT_Int32,		(void*)DataFieldCopier::CopyIntoVRow_Int32);
	arrTypeToCopyIntoVRowMethod.Add((void*)VariableType::VT_Int64,		(void*)DataFieldCopier::CopyIntoVRow_Int64);
	arrTypeToCopyIntoVRowMethod.Add((void*)VariableType::VT_String,		(void*)DataFieldCopier::CopyIntoVRow_String);
	arrTypeToCopyIntoVRowMethod.Add((void*)VariableType::VT_Date,		(void*)DataFieldCopier::CopyIntoVRow_DateTime);
	arrTypeToCopyIntoVRowMethod.Add((void*)VariableType::VT_DateTime,	(void*)DataFieldCopier::CopyIntoVRow_DateTime);
	arrTypeToCopyIntoVRowMethod.Add((void*)VariableType::VT_Float,		(void*)DataFieldCopier::CopyIntoVRow_Float);
	arrTypeToCopyIntoVRowMethod.Add((void*)VariableType::VT_Double,		(void*)DataFieldCopier::CopyIntoVRow_Double);
	arrTypeToCopyIntoVRowMethod.Add((void*)VariableType::VT_Binary,		(void*)DataFieldCopier::CopyIntoVRow_Binary);

	AutoSortedArray	arrTypes;
	arrTypes.AddAsLong(FieldType::FTBool,		VariableType::VT_Bool);
	arrTypes.AddAsLong(FieldType::FTInt8,		VariableType::VT_Int8);
	arrTypes.AddAsLong(FieldType::FTInt16,		VariableType::VT_Int16);
	arrTypes.AddAsLong(FieldType::FTInt32,		VariableType::VT_Int32);
	arrTypes.AddAsLong(FieldType::FTInt64,		VariableType::VT_Int64);
	arrTypes.AddAsLong(FieldType::FTFloat,		VariableType::VT_Float);
	arrTypes.AddAsLong(FieldType::FTDouble,		VariableType::VT_Double);
	arrTypes.AddAsLong(FieldType::FTDate,		VariableType::VT_Date);
	arrTypes.AddAsLong(FieldType::FTDateTime,	VariableType::VT_DateTime);
	arrTypes.AddAsLong(FieldType::FTBinary,		VariableType::VT_Binary);
#ifndef _UNICODE
	arrTypes.AddAsLong(FieldType::FTChar,		VariableType::VT_String);
#else
	arrTypes.AddAsLong(FieldType::FTWchar,	VariableType::VT_String);
#endif

	int	nLoop = 0, nCount	= pVRowSet->GetFieldCount();
	while( nLoop < nCount ){
		VRowSetFieldInfo* pVField = pVRowSet->GetFieldInfo(nLoop);
		ASSERT( pVField );

		int nIndex = arrTypes.IndexOf((void*)pVField->type);
		if( nIndex == -1 ){
			nLoop ++;
			continue;
			}

		VariableInfo* pVarInfo = bUseDbFieldName ? pDataObject->GetDbFieldInfo(pVField->szName) : pDataObject->GetVariableInfo(pVField->szName);
		if( !pVarInfo || (pVarInfo->m_type != (long)arrTypes.GetData(nIndex) && pVarInfo->m_type != VariableType::VT_Date) /*type mismatch*/){
			nLoop ++;
			continue;
			}

		// Save copy info. {{
		CopyFieldInfo* pCopyInfo	= new CopyFieldInfo;
		pCopyInfo->nVirtualField	= nLoop;
		pCopyInfo->pVarInfo			= pVarInfo;
		nIndex						= arrTypeToCopyIntoObjectMethod.IndexOf((void*)pVarInfo->m_type);
		if( nIndex != -1 ){
			pCopyInfo->copyIntoObjectMethod	= (Copy_Type)arrTypeToCopyIntoObjectMethod.GetData(nIndex);
			pCopyInfo->copyIntoVRowMethod	= (Copy_Type)arrTypeToCopyIntoVRowMethod.GetData(nIndex);
			}
		m_listCopyInfos.Add(pCopyInfo);
		// }}

		nLoop ++;
		}
	return true;
	}

int
DataFieldCopier::CopyFromVRow(VirtualRowSet* pVRowSet, Serializable* pDataObject){
	if( !pDataObject || !pVRowSet )
		return 0;

	int nLoop = 0, nRet = 0, nCt = m_listCopyInfos.GetCount();
	while( nLoop < nCt ){
		CopyFieldInfo* pCopyInfo = m_listCopyInfos[nLoop];
		if( !pCopyInfo || !pCopyInfo->copyIntoObjectMethod ){
			nLoop ++;
			continue;
			}
		// Copy into variable method. {{
		if( (pCopyInfo->copyIntoObjectMethod)(pVRowSet, pDataObject, pCopyInfo) )
			nRet ++;
		// }}
		nLoop ++;
		}
	return nRet;
	}

int
DataFieldCopier::CopyFromVRowIntoEnumerable(VirtualRowSet* pVRowSet, EnumerableObject<Serializable>* pEnum){
	if( !pEnum || !pVRowSet )
		return 0;

	int nLoop = 0, nRet = 0, nCt = m_listCopyInfos.GetCount();
	int nLoopRow = 0, nRowCt	= pVRowSet->GetRowCount();
	while( nLoopRow < nRowCt ){
		if( !pVRowSet->SetCurrentRow(nLoopRow) ){
			nLoop		= 0;
			nLoopRow	++;
			continue;
			}

		Serializable* pObject = pEnum->CreateSerializableObject();
		if( pObject ){
			while( nLoop < nCt ){
				CopyFieldInfo* pCopyInfo = m_listCopyInfos[nLoop];
				if( !pCopyInfo || !pCopyInfo->copyIntoObjectMethod ){
					nLoop ++;
					continue;
					}
				// Copy into variable method. {{
				(pCopyInfo->copyIntoObjectMethod)(pVRowSet, pObject, pCopyInfo);
				// }}
				nLoop ++;
				}
			pEnum->Add(pObject);
			}

		nRet		++;
		nLoop		= 0;
		nLoopRow	++;
		}
	return nRet;
	}

int
DataFieldCopier::CopyFromSerializable(VirtualRowSet* pVRowSet, Serializable* pDataObject){
	if( !pDataObject || !pVRowSet )
		return 0;

	int nLoop = 0, nRet = 0, nCt = m_listCopyInfos.GetCount();
	while( nLoop < nCt ){
		CopyFieldInfo* pCopyInfo = m_listCopyInfos[nLoop];
		if( !pCopyInfo || !pCopyInfo->copyIntoVRowMethod ){
			nLoop ++;
			continue;
			}
		// Copy into virtual field method. {{
		if( (pCopyInfo->copyIntoVRowMethod)(pVRowSet, pDataObject, pCopyInfo) )
			nRet ++;
		// }}
		nLoop ++;
		}
	return nRet;
	}

bool
DataFieldCopier::CopyIntoObject_Bool(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo){
	bool* pBoolVal	=  (bool*)(&((BYTE*)pDataObject)[pCopyInfo->pVarInfo->m_nOffset]);
	if( pVRowSet->IsFieldNull(pCopyInfo->nVirtualField) )
		*pBoolVal = false;
	else
		*pBoolVal = pVRowSet->GetBool(pCopyInfo->nVirtualField);
	return true;
	}

bool
DataFieldCopier::CopyIntoObject_Int8(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo){
	char* pCharVal	=  (char*)(&((BYTE*)pDataObject)[pCopyInfo->pVarInfo->m_nOffset]);
	if( pVRowSet->IsFieldNull(pCopyInfo->nVirtualField) )
		*pCharVal = 0;
	else
		*pCharVal = pVRowSet->GetInt8(pCopyInfo->nVirtualField);
	return true;
	}

bool
DataFieldCopier::CopyIntoObject_Int16(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo){
	short* pShortVal =  (short*)(&((BYTE*)pDataObject)[pCopyInfo->pVarInfo->m_nOffset]);
	if( pVRowSet->IsFieldNull(pCopyInfo->nVirtualField) )
		*pShortVal = 0;
	else
		*pShortVal = pVRowSet->GetInt16(pCopyInfo->nVirtualField);
	return true;
	}

bool
DataFieldCopier::CopyIntoObject_Int32(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo){
	int* pIntVal =  (int*)(&((BYTE*)pDataObject)[pCopyInfo->pVarInfo->m_nOffset]);
	if( pVRowSet->IsFieldNull(pCopyInfo->nVirtualField) )
		*pIntVal = 0;
	else
		*pIntVal = pVRowSet->GetInt32(pCopyInfo->nVirtualField);
	return true;
	}

bool
DataFieldCopier::CopyIntoObject_Int64(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo){
	__int64* pInt64Val =  (__int64*)(&((BYTE*)pDataObject)[pCopyInfo->pVarInfo->m_nOffset]);
	if( pVRowSet->IsFieldNull(pCopyInfo->nVirtualField) )
		*pInt64Val = 0;
	else
		*pInt64Val = pVRowSet->GetInt64(pCopyInfo->nVirtualField);
	return true;
	}

bool
DataFieldCopier::CopyIntoObject_Float(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo){
	float* pFloatVal =  (float*)(&((BYTE*)pDataObject)[pCopyInfo->pVarInfo->m_nOffset]);
	if( pVRowSet->IsFieldNull(pCopyInfo->nVirtualField) )
		*pFloatVal = 0;
	else
		*pFloatVal = pVRowSet->GetFloat(pCopyInfo->nVirtualField);
	return true;
	}

bool
DataFieldCopier::CopyIntoObject_Double(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo){
	double* pDoubleVal =  (double*)(&((BYTE*)pDataObject)[pCopyInfo->pVarInfo->m_nOffset]);
	if( pVRowSet->IsFieldNull(pCopyInfo->nVirtualField) )
		*pDoubleVal = 0;
	else
		*pDoubleVal = pVRowSet->GetDouble(pCopyInfo->nVirtualField);
	return true;
	}

bool
DataFieldCopier::CopyIntoObject_DateTime(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo){
	DateTime* pDateVal =  (DateTime*)(&((BYTE*)pDataObject)[pCopyInfo->pVarInfo->m_nOffset]);
	if( !pVRowSet->IsFieldNull(pCopyInfo->nVirtualField) ){
		return pVRowSet->GetDate(pCopyInfo->nVirtualField, *pDateVal);
		}
	else
		pDateVal->SetStatusNull();
	return true;
	}

bool
DataFieldCopier::CopyIntoObject_Binary(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo){
	LongBinary* pLongBinary =  (LongBinary*)(&((BYTE*)pDataObject)[pCopyInfo->pVarInfo->m_nOffset]);
	if( !pVRowSet->IsFieldNull(pCopyInfo->nVirtualField) ){
		BinaryData binData;
		if( !pVRowSet->GetBinary(pCopyInfo->nVirtualField, binData) )
			return false;
		if( binData.length > 0 )
			pLongBinary->SetBinarySize(binData.length, binData.lpBinary);
		else{
			pLongBinary->SetBinarySize(0);
			}
		return true;
		}
	return false;
	}

bool
DataFieldCopier::CopyIntoObject_String(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo){
	_string* pStrVal =  (_string*)(&((BYTE*)pDataObject)[pCopyInfo->pVarInfo->m_nOffset]);
	if( pVRowSet->IsFieldNull(pCopyInfo->nVirtualField) )
		*pStrVal = _T("");
	else
		pVRowSet->GetString(pCopyInfo->nVirtualField, *pStrVal);
	return true;
	}

bool
DataFieldCopier::CopyIntoVRow_Bool(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo){
	bool* pBoolVal	=  (bool*)(&((BYTE*)pDataObject)[pCopyInfo->pVarInfo->m_nOffset]);
	return pVRowSet->SetBool(pCopyInfo->nVirtualField, *pBoolVal);
	}

bool
DataFieldCopier::CopyIntoVRow_Int8(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo){
	char* pCharVal	=  (char*)(&((BYTE*)pDataObject)[pCopyInfo->pVarInfo->m_nOffset]);
	return pVRowSet->SetInt8(pCopyInfo->nVirtualField, *pCharVal);
	}

bool
DataFieldCopier::CopyIntoVRow_Int16(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo){
	short* pShortVal =  (short*)(&((BYTE*)pDataObject)[pCopyInfo->pVarInfo->m_nOffset]);
	return pVRowSet->SetInt16(pCopyInfo->nVirtualField, *pShortVal);
	}

bool
DataFieldCopier::CopyIntoVRow_Int32(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo){
	int* pIntVal =  (int*)(&((BYTE*)pDataObject)[pCopyInfo->pVarInfo->m_nOffset]);
	return pVRowSet->SetInt32(pCopyInfo->nVirtualField, *pIntVal);
	}

bool
DataFieldCopier::CopyIntoVRow_Int64(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo){
	__int64* pInt64Val =  (__int64*)(&((BYTE*)pDataObject)[pCopyInfo->pVarInfo->m_nOffset]);
	return pVRowSet->SetInt64(pCopyInfo->nVirtualField, *pInt64Val);
	}

bool
DataFieldCopier::CopyIntoVRow_Float(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo){
	float* pFloatVal =  (float*)(&((BYTE*)pDataObject)[pCopyInfo->pVarInfo->m_nOffset]);
	return pVRowSet->SetFloat(pCopyInfo->nVirtualField, *pFloatVal);
	}

bool
DataFieldCopier::CopyIntoVRow_Double(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo){
	double* pDoubleVal =  (double*)(&((BYTE*)pDataObject)[pCopyInfo->pVarInfo->m_nOffset]);
	return pVRowSet->SetDouble(pCopyInfo->nVirtualField, *pDoubleVal);
	}

bool
DataFieldCopier::CopyIntoVRow_DateTime(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo){
	DateTime* pDateVal =  (DateTime*)(&((BYTE*)pDataObject)[pCopyInfo->pVarInfo->m_nOffset]);
	return pVRowSet->SetDate(pCopyInfo->nVirtualField, *pDateVal);
	}

bool
DataFieldCopier::CopyIntoVRow_Binary(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo){
	LongBinary*		pLongBinary =  (LongBinary*)(&((BYTE*)pDataObject)[pCopyInfo->pVarInfo->m_nOffset]);
	unsigned long	nSize		= pLongBinary->GetBinarySize();
	void*			lpData		= pLongBinary->LockMemory();
	bool			bRet		= pVRowSet->SetBinary(pCopyInfo->nVirtualField, lpData, nSize, TRUE);
	pLongBinary->UnlockMemory();
	return bRet;
	}

bool
DataFieldCopier::CopyIntoVRow_String(VirtualRowSet* pVRowSet, Serializable* pDataObject, CopyFieldInfo* pCopyInfo){
	_string* pStrVal =  (_string*)(&((BYTE*)pDataObject)[pCopyInfo->pVarInfo->m_nOffset]);
	return pVRowSet->SetString(pCopyInfo->nVirtualField, *pStrVal);
	}