#pragma once

#include <process.h>
#include <Windows.h>

class CEvent
{
public:
	CEvent(bool bInitialState = false, bool bManualReset = false, TCHAR* pEventName = nullptr)
	{
		m_hEvent = CreateEvent(NULL, bManualReset, bInitialState, pEventName);
	}

	~CEvent()
	{
		if (m_hEvent != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_hEvent);
		}
	}

	void	Set() { SetEvent(m_hEvent); }
	HANDLE	GetHandle() { return m_hEvent; }

private:
	HANDLE	m_hEvent;
};

class CLoopThreadBase
{
public:
	CLoopThreadBase(void);
	virtual ~CLoopThreadBase(void);

	virtual	void	DoUpdate() = 0;

	bool	Start(bool bBeginAndRun = true);
	bool	Stop(DWORD dwWaitTime = INFINITE); // millisec

protected:
	HANDLE	m_hThread;
	UINT	m_ThreadID;

private:
	static	unsigned int __stdcall	ThreadFunction(void* lpParam);

	CEvent	m_RunEvent;
	CEvent	m_QuitEvent;

	CLoopThreadBase(const CLoopThreadBase& rhs) = delete;
	CLoopThreadBase& operator=(const CLoopThreadBase& rhs) = delete;
};

CLoopThreadBase::CLoopThreadBase(void)
{
	m_hThread = INVALID_HANDLE_VALUE;
	m_ThreadID = 0;
}

CLoopThreadBase::~CLoopThreadBase(void)
{
	CloseHandle(m_hThread);
}

bool CLoopThreadBase::Start(bool bBeginAndRun)
{
	if (m_hThread != INVALID_HANDLE_VALUE) return false; // Already running...

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadFunction, this, 0, &m_ThreadID);
	if (m_hThread == INVALID_HANDLE_VALUE) return false;

	if (bBeginAndRun)	m_RunEvent.Set();

	return true;
}

bool CLoopThreadBase::Stop(DWORD dwWaitTime)
{
	if (m_hThread == INVALID_HANDLE_VALUE) return false; // Not running

	m_QuitEvent.Set();
	WaitForSingleObject(m_hThread, dwWaitTime);

	return true;
}

unsigned int __stdcall	CLoopThreadBase::ThreadFunction(void* lpParam)
{
	CLoopThreadBase* lpThread = reinterpret_cast<CLoopThreadBase*>(lpParam);
	WaitForSingleObject(lpThread->m_RunEvent.GetHandle(), INFINITE);

	HANDLE hQuitEvent = lpThread->m_QuitEvent.GetHandle();
	while (true)
	{
		if (WAIT_TIMEOUT == WaitForSingleObject(hQuitEvent, 1))
		{
			lpThread->DoUpdate();
		}
		else
		{
			break;
		}
	}

	return 0;
}