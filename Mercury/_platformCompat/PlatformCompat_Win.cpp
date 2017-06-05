//
//  PlatformCompat.h
//  TestApp
//
//  Created by Zaqro Butskhrikidze on 7/30/14.
//  Copyright (c) 2014 Zaqro Butskhrikidze. All rights reserved.
//

#include "PlatformCompat_Win.h"

void __cdecl
stringEx::Format(std::string& resultString, const TCHAR* fmt_str, ...){
	int final_n, n = ((int)strlen(fmt_str)) * 2; /* reserve 2 times as much as the length of the fmt_str */
	std::string str;
	std::unique_ptr<char[]> formatted;
	va_list args;
	while(1) {
		va_start(args, fmt_str);
		formatted.reset(new TCHAR[n]); /* wrap the plain char array into the unique_ptr */
		//strcpy_s(&formatted[0], n, fmt_str);
		final_n = vsnprintf_s(&formatted[0], n, _TRUNCATE, fmt_str, args);
		va_end(args);
		if (final_n < 0 || final_n >= n)
			n += abs(final_n - n + 1);
		else
			break;
		}
	resultString = std::string(formatted.get());
	}

void 
stringEx::MakeUpper(std::string& str){
	for(std::string::iterator i=str.begin(); i!=str.end(); ++i){
		*i = toupper(*i);
		}
	};
    
void
stringEx::MakeLower(std::string& str){
	for(std::string::iterator i=str.begin(); i!=str.end(); ++i){
		*i = tolower(*i);
		}
	};
    
const TCHAR* 
stringEx::GetBuffer(std::string& str){
	return str.c_str();
	}

void 
stringEx::Remove(std::string& str, TCHAR cSymbol){
	//str.erase(std::remove(str.begin(), str.end(), cSymbol), str.end());
	}

std::string 
stringEx::Right(std::string& sVal, int nCt){
	std::string result;
	int len = sVal.length();
	if( len < nCt )
		result = sVal;
	else{
		result = sVal.substr(len - nCt, nCt);
		}
	return result;
	}

std::string 
stringEx::TrimLeft(std::string& sVal){
	std::string result;
	int nOmitCt = 0;
	for(std::string::iterator i=sVal.begin(); i!=sVal.end(); i++){
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

std::string 
stringEx::TrimRight(std::string& sVal){
	std::string result;
	int nOmitCt = 0;
	for(std::string::reverse_iterator i=sVal.rbegin(); i!=sVal.rend(); ++i){
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