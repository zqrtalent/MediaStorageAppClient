#include "Path.h"
#include <stdio.h>
#ifdef _WINDOWS
#include <io.h>
#endif

_string
Path::GetInstancePath(){
    _string sCurrPath = _T("");
#ifdef _WINDOWS
	TCHAR tszFileName[512];
	tszFileName[0] = _T('\0');

	::GetModuleFileName(GetModuleHandle(NULL), tszFileName, 512);
	sCurrPath = tszFileName;

	int nIndex = sCurrPath.rfind('\\');
	if( nIndex > 0 )
		sCurrPath = sCurrPath.substr(0, nIndex);
#elif __APPLE__
    sCurrPath = [[[[[NSBundle mainBundle] bundleURL] URLByDeletingLastPathComponent] path] cStringUsingEncoding:NSUTF8StringEncoding];
#endif
    
    return sCurrPath;
	}

void
Path::BuildFullPath(_string& sPath){
	static _string sCurrentDir;
	if( !sCurrentDir.length() ){
		sCurrentDir = GetInstancePath();
		}

	if( sPath.length() == 0 ){
		sPath = sCurrentDir;
		return;
		}

    
#ifdef _WINDOWS
    if( ((int)sPath.find(_T(':'), 0)) > -1 )
        return; // Full path is already.
    bool addAddinDir = true;
    _string sAddinDir = sCurrentDir;
    while( sPath.substr(0, 3) == _T("..\\") ){
        addAddinDir = true;
        size_t nFind = sAddinDir.rfind(_T('\\'));
        ASSERT( nFind != -1 );
        if( nFind == -1 ) return;
        // Reduce addin root directory.
        sAddinDir = sAddinDir.substr(0, nFind);
        // Delete first 3 symbol.
        sPath.erase(0, 3);
    }
    
    if(addAddinDir)
        sPath = (sAddinDir + _T("\\") + sPath);
#elif __APPLE__
    // First character must be '/' symbol
    if( ((int)sPath.find('/', 0)) == 0 )
        return; // Full path is already.
    
    // Replace "\\" into /
    size_t index = sPath.find(_T("\\"));
    while (index != -1) {
        sPath = sPath.replace(index, 1, "/");
        index = -1;
        index = sPath.find(_T("\\"), index + 1);
    }
    sPath = (sCurrentDir + _T("/") + sPath);
#endif
	}

int
Path::GetFilesByExtention(_string sFolder, _string sExt, CStringArray& arrFileNames, bool bOnlyFileNames /*= false*/, bool bRecursive /*= false*/){
	BuildFullPath(sFolder);
	if( sFolder.at(sFolder.length() - 1) != '\\' ){
		sFolder += _T("\\");
		}

	if( bRecursive ){
		CStringArray arrFolders;
		EnumerateFolders(sFolder, arrFolders);
		GetFilesByExtention(sFolder, sExt, arrFileNames, bOnlyFileNames, false);

		for(UINT i=0; i<arrFolders.size(); i++)
			GetFilesByExtention(arrFolders[i], sExt, arrFileNames, bOnlyFileNames, true);
		}
	else{
#ifdef _WINDOWS
		WIN32_FIND_DATA* pNextInfo = new WIN32_FIND_DATA;
		WIN32_FIND_DATA *pFindData = (WIN32_FIND_DATA *)pNextInfo;
		HANDLE hContext = ::FindFirstFile(_string(sFolder + sExt).c_str(), (WIN32_FIND_DATA*) pNextInfo);

		if (hContext == INVALID_HANDLE_VALUE){
			FindClose(hContext);
			delete pNextInfo;
			return 0;
			}

		BOOL bFind = TRUE;
		while( bFind ){
			if( pFindData->cFileName[0] == '.' ){
				bFind = FindNextFile(hContext, pFindData);
				continue;
				}

			if( pFindData->dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY ){
				continue;
				}

			_string sFile = !bOnlyFileNames ? (sFolder + _string(pFindData->cFileName)) : _string(pFindData->cFileName);
			arrFileNames.push_back(sFile);
			bFind = FindNextFile(hContext, pFindData);
			}

		FindClose(hContext);
		delete pNextInfo;
#elif __APPLE__
#endif
		}
	return (int)arrFileNames.size();
	}

int
Path::EnumerateFolders(_string sFolder, CStringArray& arrFolders){
	BuildFullPath(sFolder);
	if( sFolder.at(sFolder.length() - 1) != '\\' )
		sFolder += _T("\\");

#ifdef _WINDOWS
	WIN32_FIND_DATA*	pNextInfo	= new WIN32_FIND_DATA;
	WIN32_FIND_DATA*	pFindData	= (WIN32_FIND_DATA *)pNextInfo;
	HANDLE				hContext	= ::FindFirstFile(_string(sFolder + _T("*.*")).c_str(), (WIN32_FIND_DATA*) pNextInfo);

	if( hContext == INVALID_HANDLE_VALUE ){
		FindClose(hContext);
		delete pNextInfo;
		return 0;
		}

	BOOL bFind = TRUE;
	while( bFind ){
		if( pFindData->cFileName[0] == '.' ){
			bFind = FindNextFile(hContext, pFindData);
			continue;
			}

		// Skip if it is not directory.
		if( !(pFindData->dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) ){
			bFind = FindNextFile(hContext, pFindData);
			continue;
			}
	
		_string sFile = (sFolder + _string(pFindData->cFileName));
		arrFolders.push_back(sFile);

		bFind = FindNextFile(hContext, pFindData);
		}

	FindClose(hContext);
	delete pNextInfo;
#elif __APPLE__
#endif
	return (int)arrFolders.size();
	}

bool
Path::FileExists(const _string& sFilePath){
#ifdef _MBCS
	int nLen = sFilePath.length() + 1;
	wchar_t* pwFilePath = new wchar_t[nLen];
#ifdef _WINDOWS
	int nRet = (int)::MultiByteToWideChar(CP_UTF8, 0, sFilePath.c_str(), -1, pwFilePath, nLen);
#elif __APPLE__
    //int nRet = (int)::MultiByteToWideChar(CP_UTF8, 0, sFilePath.c_str(), -1, pwFilePath, nLen);
#endif
	if( nRet <= 0 ){
		delete [] pwFilePath;
		return false;
		}
#else
    wchar_t* pwFilePath = (wchar_t*)sFilePath.c_str();
#endif
    
	bool bRet = false;
// Read access.
#ifdef _WINDOWS
	if( _waccess(pwFilePath, 0) == 0 )
		bRet = true;
#elif __APPLE__
    bRet = [[NSFileManager defaultManager] fileExistsAtPath:[NSString stringWithCharacters:(unichar*)pwFilePath length:sFilePath.length()]];
#else
#endif
    /*
	HANDLE hFile = ::CreateFileW(pwFilePath, GENERIC_READ, 0L, NULL, OPEN_EXISTING, 0L, NULL);
	if( hFile != INVALID_HANDLE_VALUE ){
		bRet = true;
		CloseHandle(hFile);
		}*/
#ifdef _MBCS
	delete [] pwFilePath;
#endif
	return bRet;
	}

bool
Path::SaveAsFile(void* lpData, int nDataSize, const _string& sFile){
	if( !lpData || nDataSize <= 0 )
		return false;
    
#ifdef _MBCS
	int nLen = sFilePath.length() + 1;
	wchar_t* pwFilePath = new wchar_t[nLen];
    bool bReleasePathString = true;
#ifdef _WINDOWS
	int nRet = (int)::MultiByteToWideChar(CP_UTF8, 0, sFile.c_str(), -1, pwFilePath, nLen);
#elif __APPLE__
    int nRet = 0; // MBCS not supported!
#endif
	if( nRet <= 0 ){
		delete [] pwFilePath;
		return false;
        }
#else
    wchar_t* pwFilePath = (wchar_t*)sFile.c_str();
    bool bReleasePathString = false;
#endif

    // File exists.
#ifdef _WINDOWS
	if( _waccess(pwFilePath, 0) == 0 ){
        if( bReleasePathString )
            delete [] pwFilePath;
		return false;
        }
#elif __APPLE__
    if( ![[NSFileManager defaultManager] fileExistsAtPath:[NSString stringWithCharacters:(unichar*)pwFilePath length:sFile.length()]] ){
        if( bReleasePathString )
            delete [] pwFilePath;
        return false;
        }
#else
    return false; // Need implementation
#endif
    
	// Open file.
    FILE* file = NULL;
#ifdef _WINDOWS
	_wfopen_s(&file, pwFilePath, L"w+b");
#else
    const char * pszFilePathUTF8 = [NSString stringWithCharacters:(unichar*)pwFilePath length:sFile.length()].UTF8String;
    file = fopen(pszFilePathUTF8, "w+b");
#endif
    if( file == NULL ){
		if( bReleasePathString )
            delete [] pwFilePath;
		return false;
        }
    
	bool bRet = false;
	if( fwrite(lpData, nDataSize, 1, file) == nDataSize )
		bRet = true;
    
	if( bReleasePathString )
        delete [] pwFilePath;
	fclose(file);
	return bRet;
	}

bool
Path::ReadFileContent(const _string& sFile, BYTE*& lpData, int& nSize){
#ifdef _MBCS
	int nLen = sFilePath.length() + 1;
	wchar_t* pwFilePath = new wchar_t[nLen];
    bool bReleasePathString = true;
#ifdef _WINDOWS
	int nRet = (int)::MultiByteToWideChar(CP_UTF8, 0, sFile.c_str(), -1, pwFilePath, nLen);
#elif __APPLE__
    int nRet = 0; // MBCS not supported!
#endif
	if( nRet <= 0 ){
		delete [] pwFilePath;
		return false;
    }
#else
    wchar_t* pwFilePath = (wchar_t*)sFile.c_str();
    bool bReleasePathString = false;
#endif

	bool	bRet = false;
	FILE*	file = NULL;

#ifdef _WINDOWS
	_wfopen_s(&file, pwFilePath, L"r+b");
#else
    const char * pszFilePathUTF8 = sFile.c_str();//[NSString stringWithCharacters:(unichar*)pwFilePath length:sFile.length()].UTF8String;
    file = fopen(pszFilePathUTF8, "r+b");
#endif
    
	if( file != NULL ){
		fseek	(file, 0, SEEK_END);
		DWORD dwSize = (DWORD)ftell	(file);
		rewind	(file);

		if( dwSize ){
			lpData	= new BYTE[dwSize];
			nSize	= (int)dwSize;

			DWORD dwRead = dwSize;
			if( fread(lpData, 1, dwRead, file) != dwRead ){
				bRet	= false;
				delete [] lpData;
				lpData	= NULL;
				nSize	= 0;
				}
			else
				bRet = true;
			}
		fclose(file);
		}
	

	if( bReleasePathString )
        delete [] pwFilePath;
	return bRet;
	}