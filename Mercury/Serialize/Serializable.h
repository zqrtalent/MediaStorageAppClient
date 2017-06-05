// ===========================================================================
// About: MFC new generation data serialization class library.
// Author:	Zaqro Butskhrikidze
// Version: 1.0
// Started: 12/21/2010
// ===========================================================================

#pragma once
#include "../_platformCompat/PlatformCompat.h"
#include "../_platformCompat/PlatformPoint.h"
#include "../_platformCompat/PlatformSize.h"
#include "../_platformCompat/PlatformRect.h"
#include "../_platformCompat/PlatformFont.h"
#include "../_platformCompat/PlatformLongBinary.h"
#include "../_platformCompat/PlatformDateTime.h"

#include "../Data/VRowSet.h"
#include "../Array/AutoSortedArray.h"
#include "../Array/StringToIntArray.h"
#include "../Array/StringToStringArray.h"
#include "../Array/List.h"

#if !defined(MAKEINT64)
#define MAKEINT64(low,high) \
        ((((__int64)low)&0xFFFFFFFF) | (((__int64)high)<<32))
#endif

#if !defined(IS_DEFAULT_VALUE_TYPE)
#define IS_DEFAULT_VALUE_TYPE(type) \
		(((BYTE)type)&0x80) >> 7
#endif

#if !defined(SET_DEFAULT_VALUE_TYPE)
#define SET_DEFAULT_VALUE_TYPE(type) \
		(((BYTE)type)|0x80)
#endif

#if !defined(REMOVE_DEFAULT_VALUE_TYPE)
#define REMOVE_DEFAULT_VALUE_TYPE(type) \
		(((BYTE)type)&0x7F)
#endif

// Variable type definitions.
enum VariableType{
	VT_Bool = 0,
	VT_Int8,
	VT_Int16,
	VT_Int32,
	VT_Int64,
	VT_String,
	VT_DateTime,
	VT_Date,
	VT_Time,
	VT_Float,
	VT_Double,
	VT_Binary,
	VT_Rect,
	VT_Point,
	VT_Size,
	VT_Image,
	VT_Font,
	VT_Color32,
	VT_Enumerable,
    VT_UInt64,
    VT_VoidPtr,
    VT_Chips,
	VT_None = 100,
	VT_Array
};

class CMarkup;
class Serializable;
class GrowableMemory;
class Image;

template <class VALUE>
class EnumerableObject;

struct VariableInfo{
	const _string	m_sAttributeName;
	const _string	m_sDbFieldName;
	VariableType    m_type;
    long            m_nOffset;
	bool			m_bNullable;
	bool			m_bComplexType;
	bool			m_bEnumerable;	
	};

struct SQLExecParamInfo{
    _string	sName;				// Parameter name.
	int			nField;				// Field index.
	void*		lpValue;			// Parameter value pointer.
	DWORD		cValSize;			// Parameter value size in bytes.
	SQLSMALLINT	wSQLType;			// Parameter SQL type.
	SQLSMALLINT	wSQLCType;			// Parameter SQL C Type.
	DWORD		dwParameterType;	// SQL_PARAM_OUTPUT or SQL_PARAM_INPUT
	};

typedef BOOL (_cdecl *MethodGetVariableAsString)(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
typedef bool (_cdecl *MethodWriteVariableInBuffer)(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
typedef bool (_cdecl *MethodWriteVariableInVar)(Serializable* pThis, VariableInfo* pInfo, Serializable* pInto);
typedef bool (_cdecl *MethodReadVariableFromBuffer)(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
typedef BOOL (_cdecl *MethodSetVariableFromString)(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
typedef bool (_cdecl *MethodZeroInitVariable_Type)(Serializable* pThis, VariableInfo* pInfo);
typedef bool (_cdecl *VariableValueModifyCallback)(Serializable* pObject, VariableInfo* pInfo, void* lpValueNew);

static const TCHAR* VariableTypeNames[] = {
	_T("Bool"),
	_T("Int8"),
	_T("Int16"),
	_T("Int32"),
	_T("Int64"),
	_T("String"),
	_T("DateTime"),
	_T("Date"),
	_T("Time"),
	_T("Float"),
	_T("Double"),
	_T("Binary"),
	_T("Rect"),
	_T("Point"),
	_T("Size"),
	_T("Image"),
	_T("Font"),
	_T("Color32"),
	_T("Enumerable"),
    _T("UInt64")
};

// Macros
struct AFX_VARINFOMAP{
	const AFX_VARINFOMAP* (__stdcall *pfnGetBaseVarInfoMap)();
	const VariableInfo* lpVarInfos;
	};

#define INIT_RUNTIME_VARIABLE()\
protected: \
	static const AFX_VARINFOMAP* __stdcall GetThisVariableInfoMap(); \
	virtual const AFX_VARINFOMAP* GetVariableInfoMap() const;\
															 \
	virtual AutoSortedArray* GetArrayOfVariableInfosByName(){\
		static AutoSortedArray _arrVariableInfosByName;\
		return &_arrVariableInfosByName;\
		}\


#define BEGIN_DECLARE_VAR(theClass, baseClass) \
	const AFX_VARINFOMAP* theClass::GetVariableInfoMap() const \
		{ return GetThisVariableInfoMap(); } \
	const AFX_VARINFOMAP* __stdcall theClass::GetThisVariableInfoMap() \
	{ \
		typedef theClass ThisClass;						   \
		typedef baseClass TheBaseClass;					   \
		static const VariableInfo _varInfoEntries[] =  \
		{

#define END_DECLARE_VAR() \
		{_T(""), _T(""), VariableType::VT_None, -1, false, false, false }, \
	}; \
		static const AFX_VARINFOMAP varInfoMap = \
		{ &TheBaseClass::GetThisVariableInfoMap, &_varInfoEntries[0] }; \
		return &varInfoMap; \
	}								  

#define DECLARE_VAR(attributeName, dbFieldName, varType, varOffset, nullable, complexType, enumerable) \
	{attributeName, dbFieldName, varType, varOffset, nullable, complexType, enumerable},\

#pragma pack(1)

class Serializable /*: public CObject*/
{
public:
	Serializable()
	{
		m_pVariableModifyCallbacks	= NULL;
	};

	virtual ~Serializable()
	{
		if( m_pVariableModifyCallbacks )
			delete m_pVariableModifyCallbacks;
	};

	friend class ESChildControlPropertyGrid;

	// Compare static methods.
	static int									Compare_Int8				(void* lpValue1, void* lpValue2);
	static int									Compare_Int8_Desc			(void* lpValue1, void* lpValue2);
	static int									Compare_Int16				(void* lpValue1, void* lpValue2);
	static int									Compare_Int16_Desc			(void* lpValue1, void* lpValue2);
	static int									Compare_Int32				(void* lpValue1, void* lpValue2);
	static int									Compare_Int32_Desc			(void* lpValue1, void* lpValue2);
	static int									Compare_Int64				(void* lpValue1, void* lpValue2);
	static int									Compare_Int64_Desc			(void* lpValue1, void* lpValue2);
    static int									Compare_UInt64				(void* lpValue1, void* lpValue2);
    static int									Compare_UInt64_Desc			(void* lpValue1, void* lpValue2);
	static int									Compare_Float				(void* lpValue1, void* lpValue2);
	static int									Compare_Float_Desc			(void* lpValue1, void* lpValue2);
	static int									Compare_Double				(void* lpValue1, void* lpValue2);
	static int									Compare_Double_Desc			(void* lpValue1, void* lpValue2);
	static int									Compare_String				(void* lpValue1, void* lpValue2);
	static int									Compare_String_Desc			(void* lpValue1, void* lpValue2);
	static int									Compare_DateTime			(void* lpValue1, void* lpValue2);
	static int									Compare_DateTime_Desc		(void* lpValue1, void* lpValue2);

	virtual Serializable*						CreateSerializableObject	(){ASSERT(FALSE);return NULL;};
	virtual EnumerableObject<Serializable>*		CreateEnumerableObject		(){ASSERT(FALSE);return NULL;};
	virtual const VariableInfo*					GetVariableInfo				(){ASSERT(FALSE);return NULL;};

	virtual BOOL								SerializeAsXML				(_string& xmlSource);
	virtual BOOL								SerializeAsXML				(const _string sXmlFile, BOOL bReplaceExisting);
	virtual BOOL								Serialize					(GrowableMemory* pMem);
	virtual BOOL								Serialize					(const _string sOutFile, BOOL bReplaceExisting);
	virtual BOOL								SerializeWithVariableName	(GrowableMemory* pMem);

	virtual BOOL								Deserialize					(const _string sFile);
	virtual BOOL								Deserialize					(LongBinary* pBinary);
	virtual BOOL								Deserialize					(GrowableMemory* pMem);
	virtual BOOL								DeserializeAsXML			(const _string& sXmlFile);
	virtual BOOL								DeserializeAsXML			(_string* psXmlSource);
	virtual BOOL								DeserializeWithVariableName	(GrowableMemory* pMem);
	virtual BOOL								DeserializeWithVariableName	(LongBinary* pBinary);

	// Generate source code for differenet languages. {{
	virtual bool								GenerateSourceCodeInCSharp	(_string& source, bool bGenerateNameSpace);
	/*
	arrSourceCodeByFileName - array generated source codes by file name. [OUT]
	pszNameSpace - namespace of generated classes. (eg. Poker.Net.Packets)
	pszSerializeBasePackage - Serialization base classes namespace.
	*/
	virtual bool								GenerateSourceCodeInAS3		(StringToStringArray& arrSourceCodeByFileName, const TCHAR* pszPackage, const TCHAR* pszSerializeBasePackage);
	virtual bool								GenerateSourceCodeInObjC	(_string& source, bool bGenerateNameSpace);
	// }}

	bool										ZeroInit					(); // Zero Initialize object variables.

	BOOL										SetVariableFromString		(VariableInfo* pVarInfo, _string& sValue);
	bool										SetVariable					(VariableInfo* pVarInfo, void* lpVariablePtr);

	int											GetFieldIndex				(const _string sField);
	bool										GetFieldAsString			(int nIndex, _string& sValue);
	bool										GetFieldAsString			(const _string sFieldName, _string& sValue);

	bool										AddVariableModifyCallBack	(void* lpVariablePtr, VariableValueModifyCallback callback);
	bool										CallModifyCallback			(void* lpVariablePtr, void* lpValuePtrNew);

	VariableInfo*								GetThisInfo					();
	VariableInfo*								GetDbFieldInfo				(const _string sDbField);
	VariableInfo*								GetVariableInfo				(const _string sAttribName);
	void*										GetVariablePtr				(const _string sAttribName);

	int											GetIdFieldValue				(VariableInfo** ppInfo = NULL);
	
	virtual bool								Copy						(Serializable* pInto);
	_string										GetClassName				();

	bool										GetSelectSQL				(_string& sSQL, const _string sTop /*= _T("")*/);
	bool										GetDeleteSQL				(_string& sDeleteSQL);
	bool										GetUpdateSQL				(_string& sUpdateSQL, List<SQLExecParamInfo>* pParameters, CStringArray* pArrDirtyFieldNames = NULL);
	bool										GetAddNewSQL				(_string& sAddNewSQL, List<SQLExecParamInfo>* pParameters, bool bReturnIdentity, CStringArray* pArrDirtyFieldNames = NULL);
	bool										GetProcedureSQL				(_string& sProcSQL, List<SQLExecParamInfo>* pParameters);

	static VirtualRowSet*						SerializableObjectToVRowSet	(EnumerableObject<Serializable>* pObjects);
//	static bool									FreeBinary					(LongBinary* pBinary);	
//	static bool									GlobalReAlloc				(LongBinary* pBinary, int nSizeNew, void* lpCopy = NULL);
//	static bool									LoadJpgImage				(Image* pImage, _string sJpgFile);
//	static bool									SaveJpgImage				(Image* pImage, _string sJpgFileSaveAs, bool bReplace);
//	static bool									LoadImageData				(const _string sImageFile, LongBinary& binImage);
//	static bool									ImageFromBinary				(LongBinary* binImage, Image* pImage, int& nImageDataSize);
//	static bool									CopyImage					(Image* pSrc, Image* pDest);
//	static bool									CopyFontObject				(Font* pSrc, Font* pDest);
//	static bool									BinaryFromImage				(Image* pImage, LongBinary* pBinData, ImageAsBinaryType binaryAs = ImageAsBinaryType::ImageAsPNG);

	static	int									ConvertMoney				(double dMoney);

protected:
	static __int64								SerializeDateTimeObject		(DateTime* pDateTime);
	static void									DeserializeDateTimeObject	(__int64 nDateTimeSerialized, DateTime* pDateTime);


protected:
	static const AFX_VARINFOMAP* __stdcall GetThisVariableInfoMap(){
		static AFX_VARINFOMAP varInfoMap = {NULL, NULL};
		return &varInfoMap;
		}

	virtual const AFX_VARINFOMAP* GetVariableInfoMap() const{
		return GetThisVariableInfoMap();
		}

	virtual AutoSortedArray* GetArrayOfVariableInfosByName(){
		return NULL;
		}

protected:
	static MethodZeroInitVariable_Type GetZeroInitMethod(int nIndex)
	{
		static MethodZeroInitVariable_Type _methods_ZeroInit[] = 
		{
			ZeroInitVariable_Bool,	
			ZeroInitVariable_Int8,		
			ZeroInitVariable_Int16,		
			ZeroInitVariable_Int32,		
			ZeroInitVariable_Int64,		
			ZeroInitVariable_String,		
			ZeroInitVariable_DateTime,	
			ZeroInitVariable_Date,	
			ZeroInitVariable_Time,		
			ZeroInitVariable_Float,		
			ZeroInitVariable_Double,		
			ZeroInitVariable_Binary,	
			ZeroInitVariable_Rect,	
			ZeroInitVariable_Point,	
			ZeroInitVariable_Size,
			ZeroInitVariable_Image,
			ZeroInitVariable_Font,
			ZeroInitVariable_Color32,
			ZeroInitVariable_Enum,
            ZeroInitVariable_UInt64,
		};
		return (MethodZeroInitVariable_Type)_methods_ZeroInit[nIndex];
	}

	static MethodGetVariableAsString GetVariableMethod(int nIndex)
	{
		static MethodGetVariableAsString _methods_GetVariableAsString[] = 
		{
			GetVariableAsString_Bool,	
			GetVariableAsString_Int8,	
			GetVariableAsString_Int16,	
			GetVariableAsString_Int32,	
			GetVariableAsString_Int64,	
			GetVariableAsString_String,	
			GetVariableAsString_DateTime,
			GetVariableAsString_Date,	
			GetVariableAsString_Time,	
			GetVariableAsString_Float,	
			GetVariableAsString_Double,	
			GetVariableAsString_Binary,	
			GetVariableAsString_Rect,
			GetVariableAsString_Point,
			GetVariableAsString_Size,	
			GetVariableAsString_Image,	
			GetVariableAsString_Font,	
			GetVariableAsString_Color32,
			GetVariableAsString_Enum,
            GetVariableAsString_UInt64,
		};
		return (MethodGetVariableAsString)_methods_GetVariableAsString[nIndex];
	}

	static MethodSetVariableFromString SetVariableFromString(int nIndex)
	{
		static MethodSetVariableFromString _methods_SetVariableFromString[] = 
		{
			SetVariableFromString_Bool,	
			SetVariableFromString_Int8,	
			SetVariableFromString_Int16,	
			SetVariableFromString_Int32,	
			SetVariableFromString_Int64,	
			SetVariableFromString_String,	
			SetVariableFromString_DateTime,
			SetVariableFromString_Date,	
			SetVariableFromString_Time,	
			SetVariableFromString_Float,	
			SetVariableFromString_Double,	
			SetVariableFromString_Binary,	
			SetVariableFromString_Rect,
			SetVariableFromString_Point,
			SetVariableFromString_Size,	
			SetVariableFromString_Image,	
			SetVariableFromString_Font,	
			SetVariableFromString_Color32,
			SetVariableFromString_Enum,
            SetVariableFromString_UInt64,
		};
		return (MethodSetVariableFromString)_methods_SetVariableFromString[nIndex];
	}

	static MethodWriteVariableInBuffer AddVariableMethod(int nIndex)
	{
		static MethodWriteVariableInBuffer _methods_WriteVariableInBuffer[] = 
		{
			WriteVariableInBuffer_Bool,	
			WriteVariableInBuffer_Int8,		
			WriteVariableInBuffer_Int16,		
			WriteVariableInBuffer_Int32,		
			WriteVariableInBuffer_Int64,		
			WriteVariableInBuffer_String,		
			WriteVariableInBuffer_DateTime,	
			WriteVariableInBuffer_Date,		
			WriteVariableInBuffer_Time,		
			WriteVariableInBuffer_Float,		
			WriteVariableInBuffer_Double,		
			WriteVariableInBuffer_Binary,		
			WriteVariableInBuffer_Rect,		
			WriteVariableInBuffer_Point,		
			WriteVariableInBuffer_Size,
			WriteVariableInBuffer_Image,
			WriteVariableInBuffer_Font,
			WriteVariableInBuffer_Color32,
			WriteVariableInBuffer_Enum,
            WriteVariableInBuffer_UInt64,
		};
		return (MethodWriteVariableInBuffer)_methods_WriteVariableInBuffer[nIndex];
	}

	static MethodWriteVariableInVar WriteVariableIntoVarMethod(int nIndex)
	{
		static MethodWriteVariableInVar _methods_WriteVariableInVar[] = 
		{
			WriteVariableInVar_Bool,	
			WriteVariableInVar_Int8,		
			WriteVariableInVar_Int16,		
			WriteVariableInVar_Int32,		
			WriteVariableInVar_Int64,		
			WriteVariableInVar_String,		
			WriteVariableInVar_DateTime,	
			WriteVariableInVar_Date,		
			WriteVariableInVar_Time,		
			WriteVariableInVar_Float,		
			WriteVariableInVar_Double,		
			WriteVariableInVar_Binary,	
			WriteVariableInVar_Rect,	
			WriteVariableInVar_Point,	
			WriteVariableInVar_Size,
			WriteVariableInVar_Image,
			WriteVariableInVar_Font,
			WriteVariableInVar_Color32,
			WriteVariableInVar_Enum,
            WriteVariableInVar_UInt64,
		};
		return (MethodWriteVariableInVar)_methods_WriteVariableInVar[nIndex];
	}

	static MethodReadVariableFromBuffer SetVariableMethod(int nIndex)
	{
		static MethodReadVariableFromBuffer _methods_ReadVariableFromBuffer[] = 
		{
			ReadVariableFromBuffer_Bool,		
			ReadVariableFromBuffer_Int8,		
			ReadVariableFromBuffer_Int16,		
			ReadVariableFromBuffer_Int32,	
			ReadVariableFromBuffer_Int64,		
			ReadVariableFromBuffer_String,	
			ReadVariableFromBuffer_DateTime,	
			ReadVariableFromBuffer_Date,		
			ReadVariableFromBuffer_Time,		
			ReadVariableFromBuffer_Float,		
			ReadVariableFromBuffer_Double,
			ReadVariableFromBuffer_Binary,
			ReadVariableFromBuffer_Rect,
			ReadVariableFromBuffer_Point,
			ReadVariableFromBuffer_Size,
			ReadVariableFromBuffer_Image,
			ReadVariableFromBuffer_Font,
			ReadVariableFromBuffer_Color32,
			ReadVariableFromBuffer_Enum,
            ReadVariableFromBuffer_UInt64
		};
		return (MethodReadVariableFromBuffer)_methods_ReadVariableFromBuffer[nIndex];
	}

public:
	static FieldType FieldTypeFromVarType(VariableType varType)
	{
		static AutoSortedArray arrVarTypeToFieldType;
		if( !arrVarTypeToFieldType.GetCount() ){
			arrVarTypeToFieldType.AddAsLong(VariableType::VT_Bool,		FieldType::FTBool);
			arrVarTypeToFieldType.AddAsLong(VariableType::VT_String,	FieldType::FTChar);
			arrVarTypeToFieldType.AddAsLong(VariableType::VT_Int8,		FieldType::FTInt8);
			arrVarTypeToFieldType.AddAsLong(VariableType::VT_Int16,		FieldType::FTInt16);
			arrVarTypeToFieldType.AddAsLong(VariableType::VT_Int32,		FieldType::FTInt32);
			arrVarTypeToFieldType.AddAsLong(VariableType::VT_Int64,		FieldType::FTInt64);
            arrVarTypeToFieldType.AddAsLong(VariableType::VT_UInt64,	FieldType::FTInt64);
			arrVarTypeToFieldType.AddAsLong(VariableType::VT_Float,		FieldType::FTFloat);
			arrVarTypeToFieldType.AddAsLong(VariableType::VT_Double,	FieldType::FTDouble);
			arrVarTypeToFieldType.AddAsLong(VariableType::VT_Date,		FieldType::FTDateTime);
			arrVarTypeToFieldType.AddAsLong(VariableType::VT_DateTime,	FieldType::FTDateTime);
			arrVarTypeToFieldType.AddAsLong(VariableType::VT_Binary,	FieldType::FTBinary);
			}

		int nIndex = arrVarTypeToFieldType.IndexOf((void*)varType);
		if( nIndex == -1 )
			return FieldType::FTNone;
		return (FieldType)arrVarTypeToFieldType.GetDataAsLong(nIndex);
	}
	/*
	static DWORD	SQLCTypeFromVarType(VariableType varType)
	{
		static AutoSortedArray arrVarTypeToSQLCType;
		if( !arrVarTypeToSQLCType.GetCount() ){
			arrVarTypeToSQLCType.AddAsLong(VariableType::VT_Bool,		SQL_C_BIT);
			arrVarTypeToSQLCType.AddAsLong(VariableType::VT_String,		SQL_C_CHAR);
			arrVarTypeToSQLCType.AddAsLong(VariableType::VT_Int8,		SQL_C_UTINYINT);
			arrVarTypeToSQLCType.AddAsLong(VariableType::VT_Int16,		SQL_C_SSHORT);
			arrVarTypeToSQLCType.AddAsLong(VariableType::VT_Int32,		SQL_C_SLONG);
			arrVarTypeToSQLCType.AddAsLong(VariableType::VT_Int64,		SQL_C_SBIGINT);
			arrVarTypeToSQLCType.AddAsLong(VariableType::VT_Float,		SQL_C_DOUBLE);
			arrVarTypeToSQLCType.AddAsLong(VariableType::VT_Double,		SQL_C_DOUBLE);
			arrVarTypeToSQLCType.AddAsLong(VariableType::VT_Date,		SQL_C_TIMESTAMP);
			arrVarTypeToSQLCType.AddAsLong(VariableType::VT_DateTime,	SQL_C_TIMESTAMP);
			arrVarTypeToSQLCType.AddAsLong(VariableType::VT_Binary,		SQL_C_BINARY);
			}
		int nIndex = arrVarTypeToSQLCType.IndexOf((void*)varType);
		if( nIndex == -1 )
			return 0;
		return (DWORD)arrVarTypeToSQLCType.GetData(nIndex);
	}

	static DWORD SQLTypeFromVarType(VariableType varType)
	{
		static AutoSortedArray arrVarTypeToSQLType;
		if( !arrVarTypeToSQLType.GetCount() ){
			arrVarTypeToSQLType.AddAsLong(VariableType::VT_Bool,		SQL_BIT);
			arrVarTypeToSQLType.AddAsLong(VariableType::VT_String,		SQL_CHAR);
			arrVarTypeToSQLType.AddAsLong(VariableType::VT_Int8,		SQL_TINYINT);
			arrVarTypeToSQLType.AddAsLong(VariableType::VT_Int16,		SQL_SMALLINT);
			arrVarTypeToSQLType.AddAsLong(VariableType::VT_Int32,		SQL_INTEGER);
			arrVarTypeToSQLType.AddAsLong(VariableType::VT_Int64,		SQL_BIGINT);
			arrVarTypeToSQLType.AddAsLong(VariableType::VT_Float,		SQL_FLOAT);
			arrVarTypeToSQLType.AddAsLong(VariableType::VT_Double,		SQL_DOUBLE);
			arrVarTypeToSQLType.AddAsLong(VariableType::VT_Date,		SQL_TIMESTAMP);
			arrVarTypeToSQLType.AddAsLong(VariableType::VT_DateTime,	SQL_TIMESTAMP);
			arrVarTypeToSQLType.AddAsLong(VariableType::VT_Binary,		SQL_BINARY);
			}
		int nIndex = arrVarTypeToSQLType.IndexOf((void*)varType);
		if( nIndex == -1 )
			return 0;
		return (DWORD)arrVarTypeToSQLType.GetData(nIndex);
	}*/

protected:
	AutoSortedArray*	m_pVariableModifyCallbacks; // <VariableInfo*><VariableValueModifyCallback>

protected:
	int GetVariableInfos(VariableInfo** ppInfos){
		int nCt = 0;
		const AFX_VARINFOMAP* pInfoMap = GetVariableInfoMap();
		if( !pInfoMap )
			return nCt;
		while( pInfoMap->pfnGetBaseVarInfoMap != NULL ){
			ppInfos[nCt]	= (VariableInfo*)pInfoMap->lpVarInfos;
			pInfoMap		= (pInfoMap->pfnGetBaseVarInfoMap)();
			nCt ++;
			}
		return nCt;
		}

protected:
	void			SerializeAsXML_Rec				(_string& xmlSource, const AFX_VARINFOMAP* pInfoMap = NULL);			// Serialize recursively as xml content.
	BOOL			DeserializeAsXML_Rec			(CMarkup* xml, const AFX_VARINFOMAP* pInfoMap = NULL);					// Deserialize recursively as xml content.
	
	void			Serialize_Rec					(GrowableMemory* pMem);
	BOOL			Deserialize_Rec					(GrowableMemory* pMem);

	void			SerializeWithVariableName_Rec	(GrowableMemory* pMem);
	BOOL			DeserializeWithVariableName_Rec	(GrowableMemory* pMem);
	
	_string		GetVariableAsString				(VariableInfo* pInfo);					// Retrieve variable as string giving variable order index.

	void			Copy_Rec						(Serializable* pInto);
	bool			ZeroInit_Rec					();

	static BOOL 	GetVariableAsString_Bool		(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
	static BOOL 	GetVariableAsString_Int8		(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
	static BOOL 	GetVariableAsString_Int16		(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
	static BOOL 	GetVariableAsString_Int32		(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
	static BOOL 	GetVariableAsString_Int64		(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
    static BOOL 	GetVariableAsString_UInt64		(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
	static BOOL 	GetVariableAsString_String		(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
	static BOOL 	GetVariableAsString_DateTime	(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
	static BOOL 	GetVariableAsString_Date		(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
	static BOOL 	GetVariableAsString_Time		(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
	static BOOL 	GetVariableAsString_Float		(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
	static BOOL 	GetVariableAsString_Double		(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
	static BOOL 	GetVariableAsString_Binary		(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
	static BOOL 	GetVariableAsString_Rect		(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
	static BOOL 	GetVariableAsString_Point		(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
	static BOOL 	GetVariableAsString_Size		(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
	static BOOL 	GetVariableAsString_Image		(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
	static BOOL 	GetVariableAsString_Font		(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
	static BOOL 	GetVariableAsString_Color32		(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
	static BOOL 	GetVariableAsString_Enum		(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
    
	static BOOL 	SetVariableFromString_Bool		(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
	static BOOL 	SetVariableFromString_Int8		(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
	static BOOL 	SetVariableFromString_Int16		(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
	static BOOL 	SetVariableFromString_Int32		(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
	static BOOL 	SetVariableFromString_Int64		(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
    static BOOL 	SetVariableFromString_UInt64	(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
	static BOOL 	SetVariableFromString_String	(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
	static BOOL 	SetVariableFromString_DateTime	(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
	static BOOL 	SetVariableFromString_Date		(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
	static BOOL 	SetVariableFromString_Time		(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
	static BOOL 	SetVariableFromString_Float		(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
	static BOOL 	SetVariableFromString_Double	(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
	static BOOL 	SetVariableFromString_Binary	(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
	static BOOL 	SetVariableFromString_Rect		(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
	static BOOL 	SetVariableFromString_Point		(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
	static BOOL 	SetVariableFromString_Size		(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
	static BOOL 	SetVariableFromString_Image		(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
	static BOOL 	SetVariableFromString_Font		(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
	static BOOL 	SetVariableFromString_Color32	(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
	static BOOL 	SetVariableFromString_Enum		(Serializable* pThis, VariableInfo* pInfo, _string& sValue);
	
	static bool 	WriteVariableInBuffer_Bool		(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
	static bool 	WriteVariableInBuffer_Int8		(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
	static bool 	WriteVariableInBuffer_Int16		(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
	static bool 	WriteVariableInBuffer_Int32		(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
	static bool 	WriteVariableInBuffer_Int64		(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
    static bool 	WriteVariableInBuffer_UInt64		(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
	static bool 	WriteVariableInBuffer_String	(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
	static bool 	WriteVariableInBuffer_DateTime	(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
	static bool 	WriteVariableInBuffer_Date		(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
	static bool 	WriteVariableInBuffer_Time		(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
	static bool 	WriteVariableInBuffer_Float		(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
	static bool 	WriteVariableInBuffer_Double	(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
	static bool 	WriteVariableInBuffer_Binary	(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
	static bool 	WriteVariableInBuffer_Rect		(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
	static bool 	WriteVariableInBuffer_Point		(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
	static bool 	WriteVariableInBuffer_Size		(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
	static bool 	WriteVariableInBuffer_Image		(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
	static bool 	WriteVariableInBuffer_Font		(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
	static bool 	WriteVariableInBuffer_Color32	(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
	static bool 	WriteVariableInBuffer_Enum		(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);

	static bool 	WriteVariableInVar_Bool			(Serializable* pThis, VariableInfo* pInfo, Serializable* pInto);
	static bool 	WriteVariableInVar_Int8			(Serializable* pThis, VariableInfo* pInfo, Serializable* pInto);
	static bool 	WriteVariableInVar_Int16		(Serializable* pThis, VariableInfo* pInfo, Serializable* pInto);
	static bool 	WriteVariableInVar_Int32		(Serializable* pThis, VariableInfo* pInfo, Serializable* pInto);
	static bool 	WriteVariableInVar_Int64		(Serializable* pThis, VariableInfo* pInfo, Serializable* pInto);
    static bool 	WriteVariableInVar_UInt64		(Serializable* pThis, VariableInfo* pInfo, Serializable* pInto);
	static bool 	WriteVariableInVar_String		(Serializable* pThis, VariableInfo* pInfo, Serializable* pInto);
	static bool 	WriteVariableInVar_DateTime		(Serializable* pThis, VariableInfo* pInfo, Serializable* pInto);
	static bool 	WriteVariableInVar_Date			(Serializable* pThis, VariableInfo* pInfo, Serializable* pInto);
	static bool 	WriteVariableInVar_Time			(Serializable* pThis, VariableInfo* pInfo, Serializable* pInto);
	static bool 	WriteVariableInVar_Float		(Serializable* pThis, VariableInfo* pInfo, Serializable* pInto);
	static bool 	WriteVariableInVar_Double		(Serializable* pThis, VariableInfo* pInfo, Serializable* pInto);
	static bool 	WriteVariableInVar_Binary		(Serializable* pThis, VariableInfo* pInfo, Serializable* pInto);
	static bool 	WriteVariableInVar_Rect			(Serializable* pThis, VariableInfo* pInfo, Serializable* pInto);
	static bool 	WriteVariableInVar_Point		(Serializable* pThis, VariableInfo* pInfo, Serializable* pInto);
	static bool 	WriteVariableInVar_Size			(Serializable* pThis, VariableInfo* pInfo, Serializable* pInto);
	static bool 	WriteVariableInVar_Image		(Serializable* pThis, VariableInfo* pInfo, Serializable* pInto);
	static bool 	WriteVariableInVar_Font			(Serializable* pThis, VariableInfo* pInfo, Serializable* pInto);
	static bool 	WriteVariableInVar_Color32		(Serializable* pThis, VariableInfo* pInfo, Serializable* pInto);
	static bool 	WriteVariableInVar_Enum			(Serializable* pThis, VariableInfo* pInfo, Serializable* pInto);

	static bool 	ReadVariableFromBuffer_Bool		(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
	static bool 	ReadVariableFromBuffer_Int8		(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
	static bool 	ReadVariableFromBuffer_Int16	(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
	static bool 	ReadVariableFromBuffer_Int32	(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
	static bool 	ReadVariableFromBuffer_Int64	(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
    static bool 	ReadVariableFromBuffer_UInt64	(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
	static bool 	ReadVariableFromBuffer_String	(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
	static bool 	ReadVariableFromBuffer_DateTime	(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
	static bool 	ReadVariableFromBuffer_Date		(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
	static bool 	ReadVariableFromBuffer_Time		(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
	static bool 	ReadVariableFromBuffer_Float	(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
	static bool 	ReadVariableFromBuffer_Double	(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
	static bool 	ReadVariableFromBuffer_Binary	(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
	static bool 	ReadVariableFromBuffer_Rect		(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
	static bool 	ReadVariableFromBuffer_Point	(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
	static bool 	ReadVariableFromBuffer_Size		(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
	static bool 	ReadVariableFromBuffer_Image	(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
	static bool 	ReadVariableFromBuffer_Font		(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
	static bool 	ReadVariableFromBuffer_Color32	(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);
	static bool 	ReadVariableFromBuffer_Enum		(Serializable* pThis, VariableInfo* pInfo, GrowableMemory* pMem);

	static bool 	ZeroInitVariable_Bool			(Serializable* pThis, VariableInfo* pInfo);
	static bool 	ZeroInitVariable_Int8			(Serializable* pThis, VariableInfo* pInfo);
	static bool 	ZeroInitVariable_Int16			(Serializable* pThis, VariableInfo* pInfo);
	static bool 	ZeroInitVariable_Int32			(Serializable* pThis, VariableInfo* pInfo);
	static bool 	ZeroInitVariable_Int64			(Serializable* pThis, VariableInfo* pInfo);
    static bool 	ZeroInitVariable_UInt64			(Serializable* pThis, VariableInfo* pInfo);
	static bool 	ZeroInitVariable_String			(Serializable* pThis, VariableInfo* pInfo);
	static bool 	ZeroInitVariable_DateTime		(Serializable* pThis, VariableInfo* pInfo);
	static bool 	ZeroInitVariable_Date			(Serializable* pThis, VariableInfo* pInfo);
	static bool 	ZeroInitVariable_Time			(Serializable* pThis, VariableInfo* pInfo);
	static bool 	ZeroInitVariable_Float			(Serializable* pThis, VariableInfo* pInfo);
	static bool 	ZeroInitVariable_Double			(Serializable* pThis, VariableInfo* pInfo);
	static bool 	ZeroInitVariable_Binary			(Serializable* pThis, VariableInfo* pInfo);
	static bool 	ZeroInitVariable_Rect			(Serializable* pThis, VariableInfo* pInfo);
	static bool 	ZeroInitVariable_Point			(Serializable* pThis, VariableInfo* pInfo);
	static bool 	ZeroInitVariable_Size			(Serializable* pThis, VariableInfo* pInfo);
	static bool 	ZeroInitVariable_Image			(Serializable* pThis, VariableInfo* pInfo);
	static bool 	ZeroInitVariable_Font			(Serializable* pThis, VariableInfo* pInfo);
	static bool 	ZeroInitVariable_Color32		(Serializable* pThis, VariableInfo* pInfo);
	static bool 	ZeroInitVariable_Enum			(Serializable* pThis, VariableInfo* pInfo);
};

//=============================================
//
//	VariableInfoObject
//
//=============================================

class VariableInfoObject : public Serializable
{
public:
	VariableInfoObject();
	virtual ~VariableInfoObject();

public:
	_string		m_sName;
	_string		m_sDbName;
	BYTE        m_btType;

public:
	virtual Serializable*	CreateSerializableObject	(){return new VariableInfoObject();};

protected: // Serialization.	
	INIT_RUNTIME_VARIABLE()
};


//=============================================
//
//	Enumerable object
//
//=============================================

// define compare function.
typedef int (*CompareProc)(void* lpValue1, void* lpValue2);

#include "EnumerableObject_IndexExtension.h"

template <class VALUE>
class EnumerableObject 
{
public:
	EnumerableObject();
	~EnumerableObject();

	friend Serializable;

	VALUE*						operator[]				(int nIndex);
	VALUE*						GetAt					(int nIndex);
	bool						SetAt					(int nIndex, VALUE* pVal, bool bDestroyObject = true);
	bool						RemoveAt				(int nIndex, bool bDestroyObject);
	bool						Remove					(VALUE* pVal, bool bDestroyObject);
	void						Add						(VALUE* pObject);
	int							GetCount				();
	void						SetCount				(int nCount);
	void						RemoveAll				(bool bDestroyObjects = true);
	int							IndexOf					(VALUE* val);

	VALUE*						CreateSerializableObject();
	EnumerableObject<VALUE>*	Copy					();
	int							Copy_Into				(EnumerableObject<VALUE>* pInto);
	const _string               GetClassName			();

	// Indexed variables for fast search option.
	bool						CreateVariableIndex		(TCHAR* pszVariableName, CompareProc compareMethod);
	VALUE*						FindOneVariable			(TCHAR* pszVariableName, void* lpValuePointer, int* nIndexOfFoundVariable = NULL);
	int							FindMultiVariable		(TCHAR* pszVariableName, void* lpValuePointer, CPtrArray& arrResult);
	bool						ModifyIndexVariable		(TCHAR* pszVariableName, VALUE* pObjectModified, void* lpValuePointerNew);

	int							GetObjectsByVariableIndex(TCHAR* pszVariableName, int nIndexStart, int nIndexEnd, 
												EnumerableObject<VALUE>& listObjects, bool bMakeACopyOfObject = false);

protected:
	int									m_nCount;
	VALUE**								m_objects;
	VALUE*								m_pValueObject; // Needed to create object for deserialization.
	EnumerableObject_IndexExtension*	m_pIndexMan;
};

template<class VALUE>
EnumerableObject<VALUE>::EnumerableObject(){
	m_pValueObject	= new VALUE();
	m_objects		= NULL;
	m_nCount		= 0;
	m_pIndexMan		= NULL;
	}

template<class VALUE>
EnumerableObject<VALUE>::~EnumerableObject(){
	// Remove all keys.
	if( m_pIndexMan ){
		delete m_pIndexMan;
		m_pIndexMan = NULL;
		}
	delete m_pValueObject;
	RemoveAll();
	}

template<class VALUE> VALUE*
EnumerableObject<VALUE>::operator[](int nIndex){
	if( nIndex > -1 && nIndex < m_nCount )
		return m_objects[nIndex];
	return NULL;
	}

template<class VALUE> VALUE*
EnumerableObject<VALUE>::GetAt(int nIndex){
	if( nIndex > -1 && nIndex < m_nCount )
		return m_objects[nIndex];
	return NULL;
	}

template<class VALUE> bool
EnumerableObject<VALUE>::SetAt(int nIndex, VALUE* pVal, bool bDestroyObject /*= true*/){
	if( nIndex > -1 && nIndex < m_nCount ){
		if( m_objects[nIndex] == pVal ){
			return true;
			}

		if( m_pIndexMan ){
			// Remove
			m_pIndexMan->RemoveFromIndex((Serializable*)m_objects[nIndex]);
			// Add
			m_pIndexMan->AddInIndex((Serializable*)pVal);
			}

		if( bDestroyObject && m_objects[nIndex] != NULL ){
			delete m_objects[nIndex];
			}
		m_objects[nIndex] = pVal;
		return true;
		}
	return false;
	}

template<class VALUE> void
EnumerableObject<VALUE>::Add(VALUE* pObject){
	void* lpNew = new VALUE*[m_nCount + 1];
	if( m_nCount > 0 ){
		memcpy(lpNew, m_objects, sizeof(void*)*m_nCount);
		delete [] m_objects;
		}

	if( m_pIndexMan )
		m_pIndexMan->AddInIndex((Serializable*)pObject);

	m_objects = (VALUE**)lpNew;
	m_objects[m_nCount] = pObject;
	m_nCount ++;
	}

template<class VALUE> void
EnumerableObject<VALUE>::RemoveAll(bool bDestroyObjects /*= true*/){
	// Clear index arrays. {{
	if( m_pIndexMan )
		m_pIndexMan->ClearIndexArrays();
	// }}

	if( m_nCount > 0 ){
		int nLoop = 0;
		if( bDestroyObjects ){
			while( nLoop < m_nCount ){
				Serializable* pObject = (Serializable*)m_objects[nLoop];
				if( pObject )
					delete pObject;
				nLoop ++;
				}
			}
		
		delete [] m_objects;
		m_objects	= NULL;
		m_nCount	= 0;
		}
	}

template<class VALUE> int
EnumerableObject<VALUE>::GetCount(){
	return m_nCount;
	}

template<class VALUE> bool
EnumerableObject<VALUE>::RemoveAt(int nIndex, bool bDestroyObject){
	if( nIndex < 0 || nIndex >= m_nCount )
		return false;

	VALUE* pVal = m_objects[nIndex];
	if( m_nCount == 1 ){
		delete [] m_objects;
		m_objects	= NULL;
		m_nCount	= 0;
		}
	else{
		if( nIndex != (m_nCount - 1) )
			memmove(&m_objects[nIndex], &m_objects[nIndex + 1], sizeof(void*)*(m_nCount - nIndex - 1));
		m_nCount --;
		}

	if( m_pIndexMan )
		m_pIndexMan->RemoveFromIndex(pVal);

	if( bDestroyObject )
		delete pVal;
	return true;
	}

template<class VALUE> bool
EnumerableObject<VALUE>::Remove(VALUE* pVal, bool bDestroyObject){
	int nLoop = 0, nCt = m_nCount;
	while( nLoop < nCt ){
		if( m_objects[nLoop] == pVal ){
			if( m_nCount == 1 ){
				delete [] m_objects;
				m_objects	= NULL;
				m_nCount	= 0;
				}
			else{
				if( nLoop != (m_nCount - 1) )
					memmove(&m_objects[nLoop], &m_objects[nLoop + 1], sizeof(void*)*(m_nCount - nLoop - 1));
				m_nCount --;
				}

			if( m_pIndexMan )
				m_pIndexMan->RemoveFromIndex(pVal);

			if( bDestroyObject )
				delete pVal;
			return true;
			}
		nLoop ++;
		}
	return false;
	}


template<class VALUE> VALUE*
EnumerableObject<VALUE>::CreateSerializableObject(){
	return m_pValueObject->CreateSerializableObject();
	}

template<class VALUE> EnumerableObject<VALUE>*
EnumerableObject<VALUE>::Copy(){
	EnumerableObject<VALUE>* pCopy = new EnumerableObject<VALUE>();

	if( pCopy->m_pValueObject ){
		delete pCopy->m_pValueObject;
		pCopy->m_pValueObject = m_pValueObject->CreateSerializableObject();
		}

	for(int i=0; i<GetCount(); i++){
		VALUE* pVal = GetAt(i);
		if( pVal ){
			VALUE* pValCopy = pVal->CreateSerializableObject();
			pVal->Copy(pValCopy);
			pCopy->Add(pValCopy);
			}
		else
			pCopy->Add(NULL);
		}
	return pCopy;
	}

template<class VALUE> int
EnumerableObject<VALUE>::Copy_Into(EnumerableObject<VALUE>* pInto){
	if( !pInto ) return 0;
	for(int i=0; i<GetCount(); i++){
		VALUE* pVal = GetAt(i);
		if( pVal ){
			VALUE* pValCopy = (VALUE*)pVal->CreateSerializableObject();
			pVal->Copy(pValCopy);
			pInto->Add(pValCopy);
			}
		else
			pInto->Add(NULL);
		}
	return GetCount();
	}

template<class VALUE> const _string
EnumerableObject<VALUE>::GetClassName(){
	_string sClassName;
	if( m_pValueObject )
		sClassName = m_pValueObject->GetClassName(); 
	return sClassName;
	}

template<class VALUE> int
EnumerableObject<VALUE>::IndexOf(VALUE* val){
	int nLoop = 0;
	while( nLoop < m_nCount ){
		if( val == m_objects[nLoop] ){
			return nLoop;
			}
		nLoop ++;
		}
	return -1;
	}

//////////////////////////////////////////////////
//
// Indexed variables for fast search option.
//

template<class VALUE> bool
EnumerableObject<VALUE>::CreateVariableIndex(TCHAR* pszVariableName, CompareProc compareMethod /*= (CompareProc)Serializable::Compare_Int32*/){
	if( !m_pIndexMan ){	
		m_pIndexMan = new EnumerableObject_IndexExtension();
		}
	return m_pIndexMan->CreateIndex(m_pValueObject, pszVariableName, compareMethod);
	}

template<class VALUE> bool
EnumerableObject<VALUE>::ModifyIndexVariable(TCHAR* pszVariableName, VALUE* pObjectModified, void* lpValuePointerNew){
	if( !m_pIndexMan ) return false;
	return m_pIndexMan->ModifyIndexVariable(m_pValueObject, pszVariableName, (Serializable*)pObjectModified, lpValuePointerNew);
	}

template<class VALUE> VALUE*
EnumerableObject<VALUE>::FindOneVariable(TCHAR* pszVariableName, void* lpValuePointer, int* nIndexOfFoundVariable /*= NULL*/){
	if( !m_pIndexMan ) return NULL;
	return (VALUE*)m_pIndexMan->FindOne(pszVariableName, lpValuePointer, nIndexOfFoundVariable);
	}

template<class VALUE> int
EnumerableObject<VALUE>::FindMultiVariable(TCHAR* pszVariableName, void* lpValuePointer, CPtrArray& arrResult){
	if( !m_pIndexMan ) return 0;
	return m_pIndexMan->FindMulti(pszVariableName, lpValuePointer, arrResult);
	}

template<class VALUE> int
EnumerableObject<VALUE>::GetObjectsByVariableIndex(TCHAR* pszVariableName, int nIndexStart, int nIndexEnd, EnumerableObject<VALUE>& listObjects, bool bMakeACopyOfObject /*= false*/){
	if( !m_pIndexMan ) return 0;
	AutoSortedArray* pArrIndex = m_pIndexMan->GetVariableIndexArray(pszVariableName);
	if( !pArrIndex ) return 0;
    
	int nRet = 0;
	nIndexEnd = _min(nIndexEnd, ((int)pArrIndex->GetCount()) - 1);
	for(int i=nIndexStart; i<=nIndexEnd; i++){
		Serializable* pVal = (Serializable*)pArrIndex->GetData(i);
		if( pVal ){
			if( bMakeACopyOfObject ){
				Serializable* pCopy = pVal->CreateSerializableObject();
				pVal->Copy(pCopy);
				listObjects.Add((VALUE*)pCopy);
				}
			else
				listObjects.Add((VALUE*)pVal);
			}
		else
			listObjects.Add(NULL);
		nRet ++;
		}
	return nRet;
	}
