#include "stdafx.h"
#include "Timers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

Timers::Timers(){
	m_hTimer = ::CreateTimerQueue();
	}

Timers::~Timers(){
	KillAllTimers();

	// Delete timer queue.
	::DeleteTimerQueue(m_hTimer);
	m_hTimer = NULL;
	}

int
Timers::SetTimer(int nIdEvent, int nSecElapse, TimerEventProc timerProc, void* lpParam, bool bKillAfter /*= true*/){
	m_lock.Lock();	// Lock
	UINT_PTR	uIdEventHi	= ((DWORD_PTR)m_listTimerInfos.GetCount()) << 16;
	UINT_PTR	uIdEvent	= uIdEventHi|(((DWORD_PTR)uIdEvent)&0xFFFF);

	HANDLE	hQueueTimer = NULL;
	BOOL	bRet		= CreateTimerQueueTimer(&hQueueTimer, m_hTimer, 
		(WAITORTIMERCALLBACK)Timers::WaitOrTimerCallback, (LPVOID)uIdEvent, 1000*nSecElapse, 0, WT_EXECUTEONLYONCE);
	if( !bRet ){
		m_lock.Unlock(); // Unlock
		return 0;
		}

	TimerInfo* pInfoNew		= new TimerInfo;
	pInfoNew->hQueueTimer	= hQueueTimer;
	pInfoNew->nTimerId		= (int)uIdEvent;
	pInfoNew->nIdEvent		= nIdEvent;
	pInfoNew->timerProc		= timerProc;
	pInfoNew->nSecElapse	= nSecElapse;
	pInfoNew->bKillAfter	= bKillAfter;
	pInfoNew->lpParam		= lpParam;

	m_listTimerInfos		.Add(pInfoNew);
	m_arrTimerInfoByTimerId	.Add((__int64)uIdEvent, (__int64)pInfoNew);
	m_lock.Unlock(); // Unlock
	return pInfoNew->nTimerId;
	}

bool
Timers::KillTimer(int nTimerId){
	if( !m_hTimer ) return false;
	m_lock.Lock();		// Lock
	int nIndex = m_arrTimerInfoByTimerId.IndexOf(nTimerId);
	if( nIndex == -1 ){
		m_lock.Unlock();// Unlock
		return false;
		}

	TimerInfo* pInfo = (TimerInfo*)m_arrTimerInfoByTimerId.GetData(nIndex);
	if( !pInfo ){
		m_lock.Unlock();// Unlock
		return false;
		}

	// Destroy timer.
	::DeleteTimerQueueTimer(m_hTimer, pInfo->hQueueTimer, NULL);

	// Destroy timer info. {{
	m_arrTimerInfoByTimerId	.Delete(nIndex);
	m_listTimerInfos		.Remove(pInfo);
	// }}

	m_lock.Unlock();	// Unlock
	return true;
	}

void	__stdcall	
Timers::WaitOrTimerCallback(void* lpParameter, BOOLEAN TimerOrWaitFired){
	UINT_PTR nIdEvent = (UINT_PTR)lpParameter;
	Timers* pInstance = Timers::GetInstance();
	pInstance->OnTimerElapsed(nIdEvent);
	}

void
Timers::OnTimerElapsed(UINT_PTR nTimerId){
	m_lock.Lock();			// Lock
	int nIndex = m_arrTimerInfoByTimerId.IndexOf((__int64)nTimerId);
	if( nIndex == -1 ){	
		m_lock.Unlock();	// Unlock
		return;
		}
	TimerInfo* pInfo = (TimerInfo*)m_arrTimerInfoByTimerId.GetData(nIndex);
	if( !pInfo ){
		m_lock.Unlock();	// Unlock
		return;
		}

	TimerInfo timerInfo;
	memcpy(&timerInfo, pInfo, sizeof(TimerInfo));
	m_lock.Unlock();	// Unlock

	if( timerInfo.bKillAfter ){
		KillTimer(timerInfo.nTimerId);
		}

	if( timerInfo.timerProc ){
		(timerInfo.timerProc)(timerInfo.nIdEvent, timerInfo.lpParam);
		}
	}

bool
Timers::KillAllTimers(){
	m_lock.Lock();		// Lock
	int nLoop	= 0;
	int nCt		= m_listTimerInfos.GetCount();

	while( nLoop < nCt ){
		TimerInfo* pInfo = (TimerInfo*)m_listTimerInfos.GetAt(nLoop);
		if( pInfo ){
			::DeleteTimerQueueTimer(m_hTimer, pInfo->hQueueTimer, NULL);
			}
		nLoop ++;
		}

	m_listTimerInfos		.RemoveAll();
	m_arrTimerInfoByTimerId	.DeleteAll();
	m_lock.Unlock();		// Unlock
	return true;
	}