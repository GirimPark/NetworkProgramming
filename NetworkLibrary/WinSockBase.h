#pragma once
#include <winsock2.h>

constexpr int BUFSIZE = 1024;

class WinSockBase
{
protected:
    const char* m_peerIP;
    short m_peerPort;
    SOCKET m_socket;
    WSAEVENT m_event;
    WSANETWORKEVENTS m_networkEvents;

public:
    virtual ~WinSockBase();

public:
    /// E_FAIL: continue
	/// E_ABORT: return
    virtual HRESULT Update();
    virtual void Finalize();

private:
    virtual HRESULT Receive() = 0;
    virtual HRESULT Send() = 0;
};