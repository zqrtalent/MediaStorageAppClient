#pragma once 
#include "../_platformCompat/PlatformCompat.h"

class Path
{
public:
	static _string	GetInstancePath		();
	static void			BuildFullPath		(_string& sPath);
	static int			GetFilesByExtention	(_string sFolder, _string sExt, CStringArray& arrFileNames, bool bOnlyFileNames = false, bool bRecursive = false);
	static int			EnumerateFolders	(_string sFolder, CStringArray& arrFolders);

	static bool			FileExists			(const _string& sFilePath);
	static bool			SaveAsFile			(void* lpData, int nDataSize, const _string& sFile);
	static bool			ReadFileContent		(const _string& sFile, BYTE*& lpData, int& nSize);
};