#pragma once
#include <afxmt.h>
#include "..\Array\List.h"
#include "..\Array\AutoSortedArrayInt64.h"

typedef void (__stdcall* TimerEventProc)(int nIdEvent, void* lpParam); 

struct TimerInfo{
	HANDLE			hQueueTimer;
	int				nIdEvent;		// Event id for setting timer using class.
	int				nTimerId;		// Timer id.
	TimerEventProc	timerProc;	
	int				nSecElapse;
	bool			bKillAfter;
	void*			lpParam;
	};

class Timers
{
protected:
	Timers();

public:
	~Timers();
	static Timers* GetInstance(){
		static Timers timerInstance;
		return &timerInstance;
		}

public:
	int		SetTimer		(int nIdEvent, int nSecElapse, TimerEventProc timerProc, void* lpParam, bool bKillAfter = true);
	bool	KillTimer		(int nTimerId);

protected:
	static void	__stdcall	WaitOrTimerCallback	(void* lpParameter, BOOLEAN TimerOrWaitFired);

	void					OnTimerElapsed	(UINT_PTR nTimerId);
	bool					KillAllTimers	();

protected:
	CCriticalSection		m_lock;
	HANDLE					m_hTimer;
	List<TimerInfo>			m_listTimerInfos;
	AutoSortedArrayInt64	m_arrTimerInfoByTimerId;		// <Uniquie timer id event><TimerInfo*>
};