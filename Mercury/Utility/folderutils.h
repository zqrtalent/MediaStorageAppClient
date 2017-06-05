#ifndef _FOLDER_UTILS
#define _FOLDER_UTILS

#include <windows.h>
#include <stdio.h>
#include <aclapi.h>
#include <sddl.h>

class CFolderUtils
{
public:
	static BOOL	CreateFolder(LPCTSTR pFolder)
	{
		TCHAR szFolder[MAX_PATH];
		_tcscpy(szFolder, pFolder);
		TCHAR *pStart = szFolder;
		TCHAR *pEnd = pStart + _tcslen(szFolder);
		TCHAR *p = pEnd;

		BOOL bRet = FALSE;
		// Try 10 times to create the directory
		for (int i=0;i<10;i++)
		{
			BOOL bOK = CreateDirectory(szFolder, NULL);
			DWORD dwLastError = GetLastError();
			if (!bOK && dwLastError == ERROR_PATH_NOT_FOUND)
			{
				while (*p != '\\')
				{
					if (p == pStart){
						return FALSE;
						}
					p--;
				}
				*p = NULL;
			}
			else if (bOK || (ERROR_ALREADY_EXISTS == dwLastError))
			{
				if (p == pEnd){
					return TRUE;
					}

				*p = '\\';
				while (*p)
					p++;
			}
			else
				break;
		}
		return FALSE;
	}

	static BOOL	CreateFolderW(LPCWSTR pFolder)
	{
		wchar_t szFolder[MAX_PATH];
		wcscpy(szFolder, pFolder);
		wchar_t *pStart = szFolder;
		wchar_t *pEnd = pStart + wcslen(szFolder);
		wchar_t *p = pEnd;

		// Try 10 times to create the directory
		for (int i=0;i<10;i++)
		{
			BOOL bOK = CreateDirectoryW(szFolder, NULL);
			DWORD dwLastError = GetLastError();
			if (!bOK && dwLastError == ERROR_PATH_NOT_FOUND)
			{
				while (*p != L'\\')
				{
					if (p == pStart)
						return FALSE;
					p--;
				}
				*p = NULL;
			}
			else if (bOK || (ERROR_ALREADY_EXISTS == dwLastError))
			{
				if (p == pEnd)
					return TRUE;

				*p = L'\\';
				while (*p)
					p++;
			}
			else
				break;
		}

		return FALSE;
	}

	static DWORD GetSpace(LPCTSTR pFolder)
	{
		TCHAR szFullPathFileName[MAX_PATH];
		TCHAR szFilename[MAX_PATH];
		WIN32_FIND_DATA FileData = {0};
		BOOL bFinished = FALSE; 
		DWORD dwSize = 0;

		_stprintf(szFullPathFileName, TEXT("%s\\*.*"), pFolder);
		HANDLE hSearch = FindFirstFile(szFullPathFileName, &FileData); 
		if (hSearch == INVALID_HANDLE_VALUE) 
			return 0;

		while (!bFinished)
		{
			_stprintf(szFilename, TEXT("%s\\%s"),pFolder,FileData.cFileName);
			if (FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (_tcscmp(FileData.cFileName,TEXT(".")) && _tcscmp(FileData.cFileName,TEXT("..")))
					dwSize += GetSpace(szFilename);
			}
			else
				dwSize += FileData.nFileSizeLow;

			if (!FindNextFile(hSearch, &FileData)) 
			{
				if (GetLastError() == ERROR_NO_MORE_FILES) 
					bFinished = TRUE;
			} 
		}

		FindClose(hSearch);
		return dwSize;
	}

	static BOOL RemoveFolder(LPCTSTR pFolder)
	{
		TCHAR szFullPathFileName[MAX_PATH];
		TCHAR szFilename[MAX_PATH];

		if (!::RemoveDirectory(pFolder))
		{
			DWORD err = GetLastError();
			if (err != ERROR_DIR_NOT_EMPTY)
				return FALSE;
		}
		
		// remove sub folders and files.

		WIN32_FIND_DATA FileData = {0};
		BOOL bFinished = FALSE; 
		DWORD dwSize = 0;

		_stprintf(szFullPathFileName, TEXT("%s\\*.*"), pFolder);
		HANDLE hSearch = FindFirstFile(szFullPathFileName, &FileData); 
		if (hSearch == INVALID_HANDLE_VALUE) 
			return 0;

		while (!bFinished)
		{
			_stprintf(szFilename, TEXT("%s\\%s"),pFolder,FileData.cFileName);
			if (FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (_tcscmp(FileData.cFileName,TEXT(".")) && _tcscmp(FileData.cFileName,TEXT("..")))
				{
					RemoveFolder(szFilename);
					RemoveDirectory(szFilename);
				}
			}
			else
			{
				if (FileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
					SetFileAttributes(szFilename, FILE_ATTRIBUTE_NORMAL);

				if (!::DeleteFile(szFilename))
				{
					FindClose(hSearch);
					return FALSE;
				}
			}
			if (!FindNextFile(hSearch, &FileData)) 
			{
				if (GetLastError() == ERROR_NO_MORE_FILES) 
					bFinished = TRUE;
			} 
		}
		FindClose(hSearch);

		// Here the directory is empty.
		::RemoveDirectory(pFolder);
		return TRUE;
	}

	static BOOL RemoveFolderW(LPCWSTR pFolder)
	{
		wchar_t szFullPathFileName[MAX_PATH];
		wchar_t szFilename[MAX_PATH];

		if (!::RemoveDirectoryW(pFolder))
			{
			DWORD err = GetLastError();
			if (err != ERROR_DIR_NOT_EMPTY)
				return FALSE;
			}
		
		// remove sub folders and files.

		WIN32_FIND_DATAW FileData = {0};
		BOOL bFinished = FALSE; 
		DWORD dwSize = 0;

		swprintf(szFullPathFileName, L"%s\\*.*", pFolder);
		HANDLE hSearch = FindFirstFileW(szFullPathFileName, &FileData); 
		if (hSearch == INVALID_HANDLE_VALUE) 
			return 0;

		while (!bFinished)
		{
			swprintf(szFilename, L"%s\\%s",pFolder,FileData.cFileName);
			if (FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (wcscmp(FileData.cFileName,L".") && wcscmp(FileData.cFileName,L".."))
				{
					RemoveFolderW(szFilename);
					RemoveDirectoryW(szFilename);
				}
			}
			else
			{
				if (FileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
					SetFileAttributesW(szFilename, FILE_ATTRIBUTE_NORMAL);

				if (!::DeleteFileW(szFilename))
				{
					FindClose(hSearch);
					return FALSE;
				}
			}
			if (!FindNextFileW(hSearch, &FileData)) 
			{
				if (GetLastError() == ERROR_NO_MORE_FILES) 
					bFinished = TRUE;
			} 
		}
		FindClose(hSearch);

		// Here the directory is empty.
		::RemoveDirectoryW(pFolder);
		return TRUE;
	}

	static BOOL CopyFolder(LPCTSTR pFromFolder, LPCTSTR pToFolder)
	{
		SHFILEOPSTRUCT fo = {0};

		fo.wFunc = FO_COPY;
		fo.pFrom = pFromFolder;
		fo.pTo = pToFolder;
		fo.fFlags = FOF_SILENT | FOF_NOERRORUI;

		int rc = SHFileOperation(&fo);
		return (rc == 0);
	}
};

#endif