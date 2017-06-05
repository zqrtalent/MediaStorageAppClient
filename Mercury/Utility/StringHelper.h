#pragma once
#include "../_platformCompat/PlatformCompat.h"

class StringHelper
{
public:
	static int	Split			(_string* pString, TCHAR* pszDelimiter, CStringArray& arrStrings);
	static int	HexStringIntoInt(TCHAR* pszHexString, int nLen);

	static int	UTF8ToUnicode	(const char* pString, wchar_t* pwszUnicode, int nLen);
	static int	UnicodeToUTF8	(wchar_t* pwszUnicode, int nLen, std::string& pString);
};