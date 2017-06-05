// ===========================================================================
// About: MFC new generation data management class library.
// Author:	Zaqro Butskhrikidze
// Version: 1.0
// Started: 12/21/2010
// ===========================================================================

#pragma once
#include "../Array/ChainTempl.h"
#include "../_platformCompat/PlatformDateTime.h"

class VirtualRowSet;
class Serializable;

// Fixed field size in bytes by field type.
const int _field_default_size_by_type[] = 
{
	//sizeof(bool),1,2,4,8,sizeof(double),sizeof(double),sizeof(TIMESTAMP_STRUCT),sizeof(TIMESTAMP_STRUCT),sizeof(char),sizeof(wchar_t),sizeof(void*)
	sizeof(bool),1,2,4,8,sizeof(double),sizeof(double),sizeof(time_t),sizeof(time_t),sizeof(char),sizeof(wchar_t),sizeof(void*)
};

#ifndef FIELD_LENGTH_TYPE
	#ifdef _WIN64
		#define FIELD_LENGTH_TYPE __int64
	#elif _WIN32
		#define FIELD_LENGTH_TYPE long
	#else
		#define FIELD_LENGTH_TYPE __int64
	#endif
#endif

enum FieldType{
	FTNone = -1,
	FTBool = 0,
	FTInt8,
	FTInt16,
	FTInt32,
	FTInt64,
	FTFloat,
	FTDouble,
	FTDate,
	FTDateTime,
	FTChar,
	FTWchar,
	FTBinary
	};

struct BinaryData{
	void* lpBinary;
	int	  length;
	};

typedef struct VRowHeaderTag{
	VirtualRowSet*	pOwner;
	VRowHeaderTag*	pParentRow;
	VRowHeaderTag*	pChildRow;
	VRowHeaderTag*	pNextRow;
	VRowHeaderTag*	pPrevRow;
	int				nFieldCt;
	int				nMemAlloc;
	int				nMemUsage;
	} VRowHeader;


typedef struct VRowSetFieldInfoTag{
	int							nOrdinal;				// Ordinal index of field.
	TCHAR						szName[128];			// Field name.
	FieldType					type;					// Field type.
	int							nSize;					// Field size in bytes.
	BOOL						bIsBlob;				// Determines is blob field or not.
	DWORD						dwValueOffset;			// Field value offset in bytes.
	DWORD						dwValueLengthOffset;	// Field value length offset in bytes.
	VRowSetFieldInfoTag*		next;					// Next field info pointer.
	VRowSetFieldInfoTag*		prev;					// Prev field info pointer.

	// Get field value methods by field type.
	typedef bool			(*GetFieldAsBool)	(VRowSetFieldInfoTag* pInfo, VRowHeader* pRow);
	typedef char			(*GetFieldAsInt8)	(VRowSetFieldInfoTag* pInfo, VRowHeader* pRow);
	typedef short			(*GetFieldAsInt16)	(VRowSetFieldInfoTag* pInfo, VRowHeader* pRow);
	typedef int				(*GetFieldAsInt32)	(VRowSetFieldInfoTag* pInfo, VRowHeader* pRow);
	typedef __int64			(*GetFieldAsInt64)	(VRowSetFieldInfoTag* pInfo, VRowHeader* pRow);
	typedef float			(*GetFieldAsFloat)	(VRowSetFieldInfoTag* pInfo, VRowHeader* pRow);
	typedef double			(*GetFieldAsDouble)	(VRowSetFieldInfoTag* pInfo, VRowHeader* pRow);
	typedef DateTime		(*GetFieldAsDate)	(VRowSetFieldInfoTag* pInfo, VRowHeader* pRow);
	typedef BinaryData		(*GetFieldAsBinary)	(VRowSetFieldInfoTag* pInfo, VRowHeader* pRow);
	typedef _string		(*GetFieldAsString)	(VRowSetFieldInfoTag* pInfo, VRowHeader* pRow);

	// Set field value methods by field type.
	typedef bool			(*SetFieldAsBool)	(VRowSetFieldInfoTag* pInfo, VRowHeader* pRow, bool bValue);
	typedef bool			(*SetFieldAsInt8)	(VRowSetFieldInfoTag* pInfo, VRowHeader* pRow, char cValue);
	typedef bool			(*SetFieldAsInt16)	(VRowSetFieldInfoTag* pInfo, VRowHeader* pRow, short shValue);
	typedef bool			(*SetFieldAsInt32)	(VRowSetFieldInfoTag* pInfo, VRowHeader* pRow, int nValue);
	typedef bool			(*SetFieldAsInt64)	(VRowSetFieldInfoTag* pInfo, VRowHeader* pRow, __int64 nValue);
	typedef bool			(*SetFieldAsFloat)	(VRowSetFieldInfoTag* pInfo, VRowHeader* pRow, float fValue);
	typedef bool			(*SetFieldAsDouble)	(VRowSetFieldInfoTag* pInfo, VRowHeader* pRow, double dValue);
	typedef bool			(*SetFieldAsDate)	(VRowSetFieldInfoTag* pInfo, VRowHeader* pRow, DateTime& dtValue);
	typedef bool			(*SetFieldAsBinary)	(VRowSetFieldInfoTag* pInfo, VRowHeader* pRow, BinaryData& binValue, BOOL bCopyData);
	typedef bool			(*SetFieldAsString)	(VRowSetFieldInfoTag* pInfo, VRowHeader* pRow, const _string sValue);

	// Data retieve methods.
	GetFieldAsBool				methodGetAsBool;		
	GetFieldAsInt8				methodGetAsInt8;
	GetFieldAsInt16				methodGetAsInt16;
	GetFieldAsInt32				methodGetAsInt32;
	GetFieldAsInt64				methodGetAsInt64;
	GetFieldAsFloat				methodGetAsFloat;
	GetFieldAsDouble			methodGetAsDouble;
	GetFieldAsDate				methodGetAsDate;
	GetFieldAsBinary			methodGetAsBinary;
	GetFieldAsString			methodGetAsString;

	SetFieldAsBool				methodSetAsBool;		
	SetFieldAsInt8				methodSetAsInt8;
	SetFieldAsInt16				methodSetAsInt16;
	SetFieldAsInt32				methodSetAsInt32;
	SetFieldAsInt64				methodSetAsInt64;
	SetFieldAsFloat				methodSetAsFloat;
	SetFieldAsDouble			methodSetAsDouble;
	SetFieldAsDate				methodSetAsDate;
	SetFieldAsBinary			methodSetAsBinary;
	SetFieldAsString			methodSetAsString;
	} VRowSetFieldInfo;

class VirtualRowSet
{
public:
	VirtualRowSet();
	virtual ~VirtualRowSet();

	friend class VirtualRowSetIndex;
	friend class iODBCRst;

	static VirtualRowSet*	CreateFromSerializable		(Serializable* pObject, CStringArray* pArrDirtyFieldNames, VirtualRowSet* pRowSet = NULL, bool bUseDbFieldName = true, int nStringLength = 64);

protected:
	ChainTempl<VRowSetFieldInfo>*	m_pFields;
	VRowHeader*						m_pRow;				// Current virtual row.
	VRowHeader*						m_pRowRoot;			// Root virtual row.
	BOOL							m_bCalcRows;		// Flag to calculate rows when retrieve row count.
	int								m_nCurrentRow;		// Current row index.
	int								m_nRowCt;			// Row count indicator.
	HWND							m_wndOwner;			// Owner window class.
protected:

	// Returns row pointer by index.
	VRowHeader*	GetRowByIndex(int nRow);

public:
	static void	SetRowCt	(VirtualRowSet* pRowSet, int nRowCt);

	VirtualRowSet*			GetInstanceCopy();
	bool					CopyCurrentRowInto(VirtualRowSet* pRowSetCopyInto);

	// Returns added field index.
	int		AddField		(const TCHAR* pszName, FieldType fType); // +
	// Returns added field index.
	int		AddFieldEx		(const TCHAR* pszName, FieldType fType, int nLength);// +
	bool	RemoveField		(const TCHAR* pszName); // +
	bool	RemoveField		(int nFieldIndex); // +
	bool	FieldExists		(const TCHAR* pszName); // +
	int		GetOrdinalIndex	(const TCHAR* pszName, VRowSetFieldInfo** ppFieldInfo = NULL); // +
	int		GetFieldCount	(); // +

	bool	SortByField		(const TCHAR* pszName, int& nCurrentRow, bool bAsc = true); // +

	static int CompareAsc_Int(void* lpValue1, void* lpValue2);
	static int CompareAsc_Int64(void* lpValue1, void* lpValue2);
	static int CompareAsc_Float(void* lpValue1, void* lpValue2);
	static int CompareAsc_Double(void* lpValue1, void* lpValue2);
	static int CompareAsc_Datetime(void* lpValue1, void* lpValue2);
	static int CompareAsc_String(void* lpValue1, void* lpValue2);

	static int CompareDesc_Int(void* lpValue1, void* lpValue2);
	static int CompareDesc_Int64(void* lpValue1, void* lpValue2);
	static int CompareDesc_Float(void* lpValue1, void* lpValue2);
	static int CompareDesc_Double(void* lpValue1, void* lpValue2);
	static int CompareDesc_Datetime(void* lpValue1, void* lpValue2);
	static int CompareDesc_String(void* lpValue1, void* lpValue2);

	bool	SetFieldNull	(const TCHAR* pszName);// +
	bool	SetFieldNull	(int nField);// +
	bool	IsFieldNull		(const TCHAR* pszName);// +
	bool	IsFieldNull		(int nField);// +

	VRowSetFieldInfo*		GetFieldInfo(const TCHAR* pszName); // +
	VRowSetFieldInfo*		GetFieldInfo(int nFieldIdx); // +

	// Returns count of created fields of row.
	int		Create			(); // +
	// Adds new virtual row at the end of rows.
	bool	AddRow			();

	// Delete virtual row.
	bool	DeleteRow		(int nRow);

	// Retrieves row count.
	int		GetRowCount		(); // +

	// Set current row.
	bool	SetCurrentRow	(int nRow);

	// Get current row.
	int		GetCurrentRow	(){return m_nCurrentRow;};

	// Set class owner.
	void	SetOwner		(HWND hWnd){m_wndOwner = hWnd;};
	HWND	GetOwner		(){return m_wndOwner;};

	// Set methods.
	bool	SetBool			(const TCHAR* pszName, bool bVal);
	bool	SetBool			(int nFieldIndex, bool bVal);
	bool	SetInt8			(const TCHAR* pszName, char cVal);
	bool	SetInt8			(int nFieldIndex, char cVal);
	bool	SetInt16		(const TCHAR* pszName, short shVal);
	bool	SetInt16		(int nFieldIndex, short shVal);
	bool	SetInt32		(const TCHAR* pszName, int nVal);
	bool	SetInt32		(int nFieldIndex, int nVal);
	bool	SetInt64		(const TCHAR* pszName, __int64 nVal);
	bool	SetInt64		(int nFieldIndex, __int64 nVal);
	bool	SetFloat		(const TCHAR* pszName, float fVal);
	bool	SetFloat		(int nFieldIndex, float fVal);
	bool	SetDouble		(const TCHAR* pszName, double dVal);
	bool	SetDouble		(int nFieldIndex, double dVal);
	bool	SetDate			(const TCHAR* pszName, DateTime dtVal);
	bool	SetDate			(int nFieldIndex, DateTime dtVal);
	bool	SetDate			(const TCHAR* pszName, int nYear, int nMonth, int nDay);
	bool	SetDate			(int nFieldIndex, int nYear, int nMonth, int nDay);
	bool	SetDateTime		(const TCHAR* pszName, DateTime dtVal);
	bool	SetDateTime		(int nFieldIndex, DateTime dtVal);
	bool	SetDateTime		(const TCHAR* pszName, int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond);
	bool	SetDateTime		(int nFieldIndex, int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond);
	bool	SetString		(const TCHAR* pszName, const _string sVal);
	bool	SetString		(int nFieldIndex, const _string sVal);
	bool	SetBinary		(const TCHAR* pszName, void* lpData, int nSize, BOOL bCopyData = FALSE);
	bool	SetBinary		(int nFieldIndex, void* lpData, int nSize, BOOL bCopyData = FALSE);

	// Get methods.
	bool	GetBool			(const TCHAR* pszName);
	bool	GetBool			(int nFieldIndex);
	char	GetInt8			(const TCHAR* pszName);
	char	GetInt8			(int nFieldIndex);
	short	GetInt16		(const TCHAR* pszName);
	short	GetInt16		(int nFieldIndex);
	int		GetInt32		(const TCHAR* pszName);
	int		GetInt32		(int nFieldIndex);
	__int64	GetInt64		(const TCHAR* pszName);
	__int64	GetInt64		(int nFieldIndex);
	float	GetFloat		(const TCHAR* pszName);
	float	GetFloat		(int nFieldIndex);
	double	GetDouble		(const TCHAR* pszName);
	double	GetDouble		(int nFieldIndex);
	bool	GetDate			(const TCHAR* pszName, DateTime& dtDate);
	bool	GetDate			(int nFieldIndex, DateTime& dtDate);
	bool	GetDateTime		(const TCHAR* pszName, DateTime& dtDate);
	bool	GetDateTime		(int nFieldIndex, DateTime& dtDate);
	int		GetYear			(const TCHAR* pszName);
	int		GetYear			(int nFieldIndex);
	int		GetMonth		(const TCHAR* pszName);
	int		GetMonth		(int nFieldIndex);
	int		GetDay			(const TCHAR* pszName);
	int		GetDay			(int nFieldIndex);
	int		GetHour			(const TCHAR* pszName);
	int		GetHour			(int nFieldIndex);
	int		GetMinute		(const TCHAR* pszName);
	int		GetMinute		(int nFieldIndex);
	int		GetSecond		(const TCHAR* pszName);
	int		GetSecond		(int nFieldIndex);
	_string	GetString		(const TCHAR* pszName);
	_string	GetString		(int nFieldIndex);
	bool	GetString		(const TCHAR* pszName, _string& strValue);
	bool	GetString		(int nFieldIndex, _string& strValue);

	/* returns length of binary data. */
	int		GetBinary		(const TCHAR* pszName, BinaryData& binaryValue);
	/* returns length of binary data. */
	int		GetBinary		(int nFieldIndex, BinaryData& binaryValue);

protected:
	// ---------------------------------------------------------------------------
	// Data convertion methods.
	// ---------------------------------------------------------------------------

	// Bool
	static bool			GetBoolAsBool	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static _string	GetBoolAsString	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static char			GetBoolAsInt8	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static short		GetBoolAsInt16	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static int			GetBoolAsInt32	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static __int64		GetBoolAsInt64	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static float		GetBoolAsFloat	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static double		GetBoolAsDouble	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static bool			SetBoolAsBool	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, bool bValue);
	static bool			SetBoolAsString	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, const _string sValue);
	static bool			SetBoolAsInt8	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, char cValue);
	static bool			SetBoolAsInt16	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, short shValue);
	static bool			SetBoolAsInt32	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, int nValue);
	static bool			SetBoolAsInt64	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, __int64 nValue);
	static bool			SetBoolAsFloat	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, float fValue);
	static bool			SetBoolAsDouble	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, double dValue);

	// Int8
	static char			GetInt8AsInt8	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static short		GetInt8AsInt16	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static int			GetInt8AsInt32	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static __int64		GetInt8AsInt64	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static bool			GetInt8AsBool	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static _string	GetInt8AsString	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static float		GetInt8AsFloat	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static double		GetInt8AsDouble	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static bool			SetInt8AsBool	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, bool bValue);
	static bool			SetInt8AsString	(VRowSetFieldInfo* pInfo, VRowHeader* pRow,  const _string sValue);
	static bool			SetInt8AsInt8	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, char cValue);
	static bool			SetInt8AsInt16	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, short shValue);
	static bool			SetInt8AsInt32	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, int nValue);
	static bool			SetInt8AsInt64	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, __int64 nValue);
	static bool			SetInt8AsFloat	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, float fValue);
	static bool			SetInt8AsDouble	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, double dValue);

	// Int16
	static char			GetInt16AsInt8	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static short		GetInt16AsInt16	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static int			GetInt16AsInt32	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static __int64		GetInt16AsInt64	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static bool			GetInt16AsBool	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static _string	GetInt16AsString(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static float		GetInt16AsFloat	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static double		GetInt16AsDouble(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static bool			SetInt16AsBool	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, bool bValue);
	static bool			SetInt16AsString(VRowSetFieldInfo* pInfo, VRowHeader* pRow, const _string sValue);
	static bool			SetInt16AsInt8	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, char cValue);
	static bool			SetInt16AsInt16	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, short shValue);
	static bool			SetInt16AsInt32	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, int nValue);
	static bool			SetInt16AsInt64	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, __int64 nValue);
	static bool			SetInt16AsFloat	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, float fValue);
	static bool			SetInt16AsDouble(VRowSetFieldInfo* pInfo, VRowHeader* pRow, double dValue);

	// Int32
	static char			GetInt32AsInt8	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static short		GetInt32AsInt16	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static int			GetInt32AsInt32	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static __int64		GetInt32AsInt64	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static bool			GetInt32AsBool	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static _string	GetInt32AsString(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static float		GetInt32AsFloat	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static double		GetInt32AsDouble(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static bool			SetInt32AsBool	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, bool bValue);
	static bool			SetInt32AsString(VRowSetFieldInfo* pInfo, VRowHeader* pRow, const _string sValue);
	static bool			SetInt32AsInt8	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, char cValue);
	static bool			SetInt32AsInt16	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, short shValue);
	static bool			SetInt32AsInt32	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, int nValue);
	static bool			SetInt32AsInt64	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, __int64 nValue);
	static bool			SetInt32AsFloat	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, float fValue);
	static bool			SetInt32AsDouble(VRowSetFieldInfo* pInfo, VRowHeader* pRow, double dValue);

	// Int64
	static char			GetInt64AsInt8	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static short		GetInt64AsInt16	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static int			GetInt64AsInt32	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static __int64		GetInt64AsInt64	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static bool			GetInt64AsBool	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static _string	GetInt64AsString(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static float		GetInt64AsFloat	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static double		GetInt64AsDouble(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static bool			SetInt64AsBool	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, bool bValue);
	static bool			SetInt64AsString(VRowSetFieldInfo* pInfo, VRowHeader* pRow, const _string sValue);
	static bool			SetInt64AsInt8	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, char cValue);
	static bool			SetInt64AsInt16	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, short shValue);
	static bool			SetInt64AsInt32	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, int nValue);
	static bool			SetInt64AsInt64	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, __int64 nValue);
	static bool			SetInt64AsFloat	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, float fValue);
	static bool			SetInt64AsDouble(VRowSetFieldInfo* pInfo, VRowHeader* pRow, double dValue);

	// Float
	static char			GetFloatAsInt8	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static short		GetFloatAsInt16	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static int			GetFloatAsInt32	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static __int64		GetFloatAsInt64	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static bool			GetFloatAsBool	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static _string	GetFloatAsString(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static float		GetFloatAsFloat	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static double		GetFloatAsDouble(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static bool			SetFloatAsBool	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, bool bValue);
	static bool			SetFloatAsString(VRowSetFieldInfo* pInfo, VRowHeader* pRow, const _string sValue);
	static bool			SetFloatAsInt8	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, char cValue);
	static bool			SetFloatAsInt16	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, short shValue);
	static bool			SetFloatAsInt32	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, int nValue);
	static bool			SetFloatAsInt64	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, __int64 nValue);
	static bool			SetFloatAsFloat	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, float fValue);
	static bool			SetFloatAsDouble(VRowSetFieldInfo* pInfo, VRowHeader* pRow, double dValue);

	// Double
	static char			GetDoubleAsInt8		(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static short		GetDoubleAsInt16	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static int			GetDoubleAsInt32	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static __int64		GetDoubleAsInt64	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static bool			GetDoubleAsBool		(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static _string	GetDoubleAsString	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static float		GetDoubleAsFloat	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static double		GetDoubleAsDouble	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static DateTime		GetDoubleAsDate		(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static bool			SetDoubleAsBool		(VRowSetFieldInfo* pInfo, VRowHeader* pRow, bool bValue);
	static bool			SetDoubleAsString	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, const _string sValue);
	static bool			SetDoubleAsInt8		(VRowSetFieldInfo* pInfo, VRowHeader* pRow, char cValue);
	static bool			SetDoubleAsInt16	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, short shValue);
	static bool			SetDoubleAsInt32	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, int nValue);
	static bool			SetDoubleAsInt64	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, __int64 nValue);
	static bool			SetDoubleAsFloat	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, float fValue);
	static bool			SetDoubleAsDouble	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, double dValue);
	static bool			SetDoubleAsDate		(VRowSetFieldInfo* pInfo, VRowHeader* pRow, DateTime& dtValue);

	// String
	static char			GetStringAsInt8		(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static short		GetStringAsInt16	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static int			GetStringAsInt32	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static __int64		GetStringAsInt64	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static bool			GetStringAsBool		(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static _string	GetStringAsString	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static float		GetStringAsFloat	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static double		GetStringAsDouble	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static DateTime		GetStringAsDate		(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static bool			SetStringAsBool		(VRowSetFieldInfo* pInfo, VRowHeader* pRow, bool bValue);
	static bool			SetStringAsString	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, const _string sValue);
	static bool			SetStringAsInt8		(VRowSetFieldInfo* pInfo, VRowHeader* pRow, char cValue);
	static bool			SetStringAsInt16	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, short shValue);
	static bool			SetStringAsInt32	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, int nValue);
	static bool			SetStringAsInt64	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, __int64 nValue);
	static bool			SetStringAsFloat	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, float fValue);
	static bool			SetStringAsDouble	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, double dValue);
	static bool			SetStringAsDate		(VRowSetFieldInfo* pInfo, VRowHeader* pRow, DateTime& dtValue);

	// Date
	static _string	GetDateAsString	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static double		GetDateAsDouble	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static DateTime		GetDateAsDate	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static bool			SetDateAsString	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, const _string sValue);
	static bool			SetDateAsDouble	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, double dValue);
	static bool			SetDateAsDate	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, DateTime& dtValue);

	// DateTime
	static _string	GetDateTimeAsString	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static double		GetDateTimeAsDouble	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static DateTime		GetDateTimeAsDate	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static bool			SetDateTimeAsString	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, const _string sValue);
	static bool			SetDateTimeAsDouble	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, double dValue);
	static bool			SetDateTimeAsDate	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, DateTime& dtValue);

	// Binary
	static _string	GetBinaryAsString	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static BinaryData	GetBinaryAsBinary	(VRowSetFieldInfo* pInfo, VRowHeader* pRow);
	static bool			SetBinaryAsString	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, const _string sValue, BOOL);
	static bool			SetBinaryAsBinary	(VRowSetFieldInfo* pInfo, VRowHeader* pRow, BinaryData& binValue, BOOL bCopyBinary = FALSE);
	
protected:
	/* Retrieves field pointers by index. */
	void*					GetFieldValuePtr	(const TCHAR* pszName); // +
	void*					GetFieldValuePtr	(int nFieldIndex); // +
	FIELD_LENGTH_TYPE*		GetFieldLengthPtr	(const TCHAR* pszName); // +
	FIELD_LENGTH_TYPE*		GetFieldLengthPtr	(int nFieldIndex); // +
	VRowHeader*				AllocateRow			();
	int						GetRowMemSize		();// +
	bool					DeleteVirtualRow	(VRowHeader* pRow);
public:
	void					EmptyRowSet			(); // ++
};
