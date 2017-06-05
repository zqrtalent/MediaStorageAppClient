#include "stdafx.h"
#include "FileModifyMonitor.h"
#include "..\Path\Path.h"
#include "..\Log\Logger.h"
#include <io.h>
#include <intrin.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

FileModifyMonitor::FileModifyMonitor(){
	m_hChangeMonitor	= 0L;
	m_hMonitorThread	= 0L;
	m_pEventStop		= NULL;
	m_dwThreadId		= 0L;
	m_bStarted			= false;
	m_callbackMethod	= NULL;
	m_lpClass			= NULL;
	}

FileModifyMonitor::~FileModifyMonitor(){
	StopMonitor();
	}

bool
FileModifyMonitor::StartMonitor(CString sFile, FileModifiedMethod callback, void* lpClassObject){
	m_lock.Lock(); // Lock
	if( m_bStarted ){
		m_lock.Unlock(); // Unlock
		return false; // Monitor already started.
		}
	else
		m_lock.Unlock(); // Unlock

	m_callbackMethod	= callback;
	m_lpClass			= lpClassObject;

	Path::BuildFullPath(sFile);
	m_sFileMonitor = sFile;
	if( _access(sFile.GetBuffer(), 0) )
		return false; // Monitor file not exists.

	CString sDir;
	int nLastSlash = sFile.ReverseFind('\\');
	sDir = sFile.Left(nLastSlash);

	HANDLE hChange = FindFirstChangeNotification(sDir.GetBuffer(), FALSE, FILE_NOTIFY_CHANGE_LAST_WRITE);
	if(hChange == INVALID_HANDLE_VALUE){
		Logger::CreateLogEntry(_T("Couldn't create change notify handle"));
		return false; // Couldn't create change notify handle.
		}

	m_hChangeMonitor	= hChange;
	m_pEventStop		= new CEvent(FALSE, TRUE);
	DWORD dwStackSize	= 128*1024; // 128 KB
	m_hMonitorThread	= ::CreateThread(NULL, dwStackSize, (LPTHREAD_START_ROUTINE)OnMonitorThreadProc, this, 0L, &m_dwThreadId);
	if( !m_hMonitorThread ){
		CloseHandle(m_hChangeMonitor);
		delete m_pEventStop;
		m_pEventStop = NULL;
		return false; // Couldn't create file modify monitor thread.
		}
	return true;
	}

bool
FileModifyMonitor::StopMonitor(){
	m_lock.Lock(); // Lock
	if( m_bStarted ){
		m_lock.Unlock(); // Unlock

		m_pEventStop->SetEvent();
		// Wait for 5 sec
		if( WaitForSingleObject(m_hChangeMonitor, 5000) == WAIT_TIMEOUT){
			::TerminateThread(m_hChangeMonitor, 0);
			}
		}
	else
		m_lock.Unlock(); // Unlock

	if( m_pEventStop ){
		delete m_pEventStop;
		m_pEventStop = NULL;
		}

	m_callbackMethod	= NULL;
	m_lpClass			= NULL;

	if( m_hMonitorThread )
		CloseHandle(m_hMonitorThread);
	m_hMonitorThread	= 0L;
	if( m_hChangeMonitor )
		CloseHandle(m_hChangeMonitor);
	m_hChangeMonitor	= 0L;
	m_bStarted			= false;
	m_dwThreadId		= 0L;
	m_sFileMonitor		= _T("");
	return true;
	}

void
FileModifyMonitor::OnFileSizeChanged(HANDLE hFile, DWORD dwSizeNew){
	if( m_callbackMethod ){
		FileModifiedMethod	proc	= m_callbackMethod;
		void*				lpClass = m_lpClass;
		// Call thiscall method. {{

#ifdef _WIN64
		ThisCallHelper* thisClass = (ThisCallHelper*)lpClass;
		thisClass->Call_FileModifiedMethod(proc, hFile, dwSizeNew);
#else
		__asm{
			push eax;
			push ecx;
			mov eax, lpClass; // method owner class this pointer.
			mov ecx, eax;
			mov eax, dwSizeNew;
			push eax;
			mov eax, hFile;
			push eax;
			mov eax, proc;
			call eax;
			pop ecx;
			pop eax;
			};
#endif
		
		// }}
		}
	}

UINT	__stdcall	
FileModifyMonitor::OnMonitorThreadProc(FileModifyMonitor* pOwner){
	HANDLE				hChange			= pOwner->m_hChangeMonitor;
	CCriticalSection*	pLock			= &pOwner->m_lock;
	CEvent*				pStopEvent		= pOwner->m_pEventStop;
	CString				sFileMonitor	= pOwner->m_sFileMonitor;
	DWORD				dwMilliSec		= 5000;
	DWORD				dwWaitStatus	= 0;
	HANDLE				hObjects[]		= {*pOwner->m_pEventStop, hChange};
	DWORD				dwFileSize		= 0;
	FILETIME			lastWriteTime;

	HANDLE hFileMonitor = ::CreateFile(sFileMonitor.GetBuffer(), GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0L, NULL);
	if( hFileMonitor != INVALID_HANDLE_VALUE ){
		GetFileTime(hFileMonitor, NULL, NULL, &lastWriteTime);
		CloseHandle(hFileMonitor);
		}
	else{
		Logger::CreateLogEntry(_T("Monitor file not exists !!!"));
		return 0; // Monitor file not exists.
		}

	pLock->Lock();		// Lock
	pOwner->m_bStarted = true;
	pLock->Unlock();	// Unlock

	while( 1 ){
		dwWaitStatus = WaitForMultipleObjects(2, hObjects, FALSE, dwMilliSec); 
		if( dwWaitStatus == WAIT_TIMEOUT || dwWaitStatus == WAIT_OBJECT_0 )
			break;
		if( dwWaitStatus == (WAIT_OBJECT_0 + 1) ){
			// Check file size modification.################
			HANDLE hFileMonitor = ::CreateFile(sFileMonitor.GetBuffer(), GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0L, NULL);
			if( hFileMonitor != INVALID_HANDLE_VALUE ){
				FILETIME lastWriteTime1;
				GetFileTime(hFileMonitor, NULL, NULL, &lastWriteTime1);
				if( CompareFileTime(&lastWriteTime, &lastWriteTime1) == -1 ){
					DWORD dwFileSizeNew = GetFileSize(hFileMonitor, NULL);
					pOwner->OnFileSizeChanged(hFileMonitor, dwFileSizeNew);
					lastWriteTime = lastWriteTime1;
					}
				CloseHandle(hFileMonitor);
				}
			// #############################################

			if( !FindNextChangeNotification(hChange) ){
				Logger::CreateLogEntry(_T("FindNextChangeNotification return with fail !!!"));
				pLock->Lock();		// Lock
				pOwner->m_bStarted = false;
				pLock->Unlock();	// Unlock
				return 0L;
				}
			}
		}

	pLock->Lock();		// Lock
	pOwner->m_bStarted = false;
	pLock->Unlock();	// Unlock
	return 0L;
	}