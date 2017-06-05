#include "StringHelper.h"

int
StringHelper::Split(_string* pString, TCHAR* pszDelimiter, CStringArray& arrStrings){
	int nDelimiterLen	= (int)_tcslen(pszDelimiter);
	int nFindStart		= 0;

	if( !pString || !nDelimiterLen )
		return 0;

	int nFind			= (int)pString->find(pszDelimiter, 0);
	while (nFind != -1){
		_string sFind = pString->substr(nFindStart, (nFind - nFindStart));
		arrStrings.push_back(sFind);

		nFindStart	= nFind + nDelimiterLen;
		nFind		= (int)pString->find(pszDelimiter, nFindStart + 1);
		}

	_string sRemain = stringEx::Right(*pString, (int)pString->length() - nFindStart);
	if( sRemain.length() )
		arrStrings.push_back(sRemain);

	return (int)arrStrings.size();
	}

int
StringHelper::HexStringIntoInt(TCHAR* pszHexString, int nLen){
	if( !pszHexString ) 
		return 0;
	int nIndex = nLen - 1, nVal = 0, nMultiplier = 1;
	while(nIndex >= 0){
		char cVal = pszHexString[nIndex];
		if( cVal >= '0' && cVal <= '9' )
			nVal += (cVal - '0')*nMultiplier;
		else
		if( cVal >= 'A' && cVal <= 'F' )
			nVal += (cVal - 'A' + 10)*nMultiplier;
		else
		if( cVal >= 'a' && cVal <= 'f' )
			nVal += (cVal - 'a' + 10)*nMultiplier;
		else
			break;
		nMultiplier *= 16;
		nIndex		--;
		}
	return nVal;
	}

int
StringHelper::UTF8ToUnicode(const char* pString, wchar_t* pwszUnicode, int nLen){
#ifdef _WINDOWS
	int		nLoop	= 0;
	int		nCt		= strlen(pString);
	int		nRet	= (int)::MultiByteToWideChar(CP_UTF8, 0, pString, -1, pwszUnicode, nLen);
	return (nRet - 1);
#elif __APPLE__
   NSString* unicodeString = [NSString stringWithUTF8String:pString];
   wchar_t* wstring = (wchar_t*)[unicodeString cStringUsingEncoding:NSUnicodeStringEncoding];
    _tcscpy_((char*)pwszUnicode, (char*)wstring);
    return (int)[unicodeString length] - 1;
#else
    
#endif
    return 0;
	}

int
StringHelper::UnicodeToUTF8(wchar_t* pwszUnicode, int nLen, std::string& pString){
    /*
	BOOL	bUsedDefChar	= FALSE;
	static  char szUtf8[512];
	szUtf8[0] = '\0';
	int		nRet = WideCharToMultiByte(CP_UTF8, 0, pwszUnicode, -1, szUtf8, 512, NULL, NULL);
	if( nRet > 0 ){
		nLen	= strnlen_s(szUtf8, 511);
		pString = szUtf8;
		}
	return nRet;*/
    return 0;
	}
