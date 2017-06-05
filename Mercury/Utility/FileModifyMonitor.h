#pragma once
#include <afxmt.h>
#include "..\Array\List.h"
#include "..\Array\AutoSortedArrayInt64.h"
#include "ThisCallHelper.h"

class FileModifyMonitor
{
public:
	FileModifyMonitor();
	~FileModifyMonitor();

	bool					StartMonitor(CString sFile, FileModifiedMethod callback, void* lpClassObject);
	bool					StopMonitor	();

protected:
	static UINT	__stdcall	OnMonitorThreadProc(FileModifyMonitor* pOwner);
	void					OnFileSizeChanged(HANDLE hFile, DWORD dwSizeNew); // Callback

protected:
	bool				m_bStarted;
	CString				m_sFileMonitor;
	HANDLE				m_hChangeMonitor;
	HANDLE				m_hMonitorThread;
	CEvent*				m_pEventStop;
	DWORD				m_dwThreadId;
	CCriticalSection	m_lock;

	FileModifiedMethod	m_callbackMethod;
	void*				m_lpClass;
};