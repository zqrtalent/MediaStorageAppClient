//
//  PlatformCompat.h
//  TestApp
//
//  Created by Zaqro Butskhrikidze on 7/30/14.
//  Copyright (c) 2014 Zaqro Butskhrikidze. All rights reserved.
//

#include "PlatformCompat_MacOSX.h"

void __cdecl
stringEx::Format(_string& resultString, const TCHAR* fmt_str, ...){
	int final_n, n = ((int)_tcslen(fmt_str)) * 2; // reserve 2 times as much as the length of the fmt_str
	_string str;
	std::unique_ptr<TCHAR[]> formatted;
	va_list args;
	while(1) {
		va_start(args, fmt_str);
		formatted.reset(new TCHAR[n]);
        final_n = vsnprintf(&formatted[0], n, fmt_str, args);
        
//#ifdef _MBCS
//		final_n = vsnprintf(&formatted[0], n, fmt_str, args);
//#else
//        final_n = vswprintf(&formatted[0], n, fmt_str, args);
//#endif
        
		va_end(args);
		if (final_n < 0 || final_n >= n)
			n += abs(final_n - n + 1);
		else
			break;
		}
	resultString = _string(formatted.get());
	}

void 
stringEx::MakeUpper(_string& str){
	for(_string::iterator i=str.begin(); i!=str.end(); ++i){
		*i = toupper(*i);
		}
	};
    
void
stringEx::MakeLower(_string& str){
	for(_string::iterator i=str.begin(); i!=str.end(); ++i){
		*i = tolower(*i);
		}
	};
    
const TCHAR* 
stringEx::GetBuffer(_string& str){
	return str.c_str();
	}

void 
stringEx::Remove(_string& str, TCHAR cSymbol){
	//str.erase(std::remove(str.begin(), str.end(), cSymbol), str.end());
	}

_string 
stringEx::Right(_string& sVal, int nCt){
	_string result;
	size_t len = sVal.length();
	if( len < nCt )
		result = sVal;
	else{
		result = sVal.substr(len - nCt, nCt);
		}
	return result;
	}

_string 
stringEx::TrimLeft(_string& sVal){
	_string result;
	int nOmitCt = 0;
	for(_string::iterator i=sVal.begin(); i!=sVal.end(); i++){
		if( *i == ' ' || *i == '\t' || *i == '\r' || *i == '\n' )
			nOmitCt ++;
		else
			break;
		}

	if( nOmitCt == 0 )
		return sVal;
	result = sVal.substr(nOmitCt, sVal.length()-nOmitCt);
	return result;
	}

_string 
stringEx::TrimRight(_string& sVal){
	_string result;
	int nOmitCt = 0;
	for(_string::reverse_iterator i=sVal.rbegin(); i!=sVal.rend(); ++i){
		if( *i == ' ' || *i == '\t' || *i == '\r' || *i == '\n' )
			nOmitCt ++;
		else
			break;
		}

	if( nOmitCt == 0 )
		return sVal;
	result = sVal.substr(0, sVal.length()-nOmitCt);
	return result;
	}