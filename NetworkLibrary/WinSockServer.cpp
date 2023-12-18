#include "pch.h"
#include "WinSockServer.h"

#include <iostream>

using namespace std;

WinSockServer::WinSockServer(short listeningPort)
	: m_listeningPort(listeningPort)
{
	SetupListen();
}

bool WinSockServer::SetupListen()
{
	/// 리슨 소켓 만들고 포트에 바인딩
	m_listeningSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (m_listeningSocket == INVALID_SOCKET)
		return 0;

	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	serverAddr.sin_port = ::htons(m_listeningPort);

	if (::bind(m_listeningSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		return 0;

	if (::listen(m_listeningSocket, SOMAXCONN) == SOCKET_ERROR)
		return 0;

	cout << "Listen" << endl;

	/// 리슨소켓에 리슨이벤트 등록
	m_sessions.reserve(100);
	m_listeningEvent = ::WSACreateEvent();
	m_wsaEvents.push_back(m_listeningEvent);
	m_sessions.push_back(Session{ m_listeningSocket });

	if (::WSAEventSelect(m_listeningSocket, m_listeningEvent, FD_ACCEPT) == SOCKET_ERROR)
		return 0;
}

HRESULT WinSockServer::Update()
{
    // 소켓 수신 버퍼(커널 메모리)에 읽어올 데이터가 있다면
    HRESULT rt;
    if (m_networkEvents.lNetworkEvents & FD_READ || m_networkEvents.lNetworkEvents & FD_WRITE)
    {
		rt = __super::Update();
    }
    else
    {
        rt = E_FAIL;
    }

    return rt;
}

/// E_FAIL: continue
/// E_ABORT: return
HRESULT WinSockServer::Accept()
{
    /// WSAWaitForMultipleEvents: 이벤트에 대한 통보를 받는다. 이때 인덱스로 어떤 이벤트인지 알 수 있다
    m_index = ::WSAWaitForMultipleEvents(m_wsaEvents.size(), &m_wsaEvents[0], FALSE, WSA_INFINITE, FALSE);
    if (m_index == WSA_WAIT_FAILED)
        return E_FAIL;

    m_index -= WSA_WAIT_EVENT_0;

    /// WSAEventSelect: 소켓에 등록된 이벤트가 발생하면 어떤 네트워크 이벤트인지 확인
    if (::WSAEnumNetworkEvents(m_sessions[m_index].socket, m_wsaEvents[m_index], &m_networkEvents) == SOCKET_ERROR)
        return E_ABORT;

    if (m_networkEvents.lNetworkEvents & FD_ACCEPT)
    {
        if (m_networkEvents.iErrorCode[FD_ACCEPT_BIT] != 0) // FD_ACCEPT에 맞는 에러
            return E_FAIL;

        SOCKADDR_IN clientAddr;
        int addrLen = sizeof(clientAddr);
        SOCKET newSocket = ::accept(m_listeningSocket, (SOCKADDR*)&clientAddr, &addrLen);

        if (newSocket != INVALID_SOCKET)
        {
            cout << "Client Connected" << endl;

            WSAEVENT newEvent = ::WSACreateEvent();
            m_wsaEvents.push_back(newEvent);
            m_sessions.push_back(Session{ newSocket });


            if (::WSAEventSelect(newSocket, newEvent, FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR)
                return E_FAIL;
        }
    }

    return S_OK;
}

HRESULT WinSockServer::Receive()
{
    if ((m_networkEvents.lNetworkEvents & FD_READ) && m_networkEvents.iErrorCode[FD_READ_BIT] != 0)
        return E_FAIL;


    if ((m_networkEvents.lNetworkEvents & FD_WRITE) && m_networkEvents.iErrorCode[FD_WRITE_BIT] != 0)
        return E_FAIL;


    Session& s = m_sessions[m_index];

    // Receive
    if (s.recvBytes == 0)
    {
        int recvLen = ::recv(s.socket, s.recvBuffer, BUFSIZE, 0);
        if (recvLen == SOCKET_ERROR && ::WSAGetLastError() != WSAEWOULDBLOCK) {
            return E_FAIL;
        }

        if (recvLen > 0)
        {
            s.recvBytes = recvLen;
        }
        cout << "Server : Recv Data= " << recvLen << " : " << s.recvBuffer << endl;
    }

    return S_OK;
}

HRESULT WinSockServer::Send()
{
    Session& s = m_sessions[m_index];

    if (s.recvBytes > s.sendBytes) {
        // 보내야할 데이터를 모두 보내지 않았다면, 남은 데이터를 다시 보내도록 시도
        int sendLen = ::send(s.socket, &s.recvBuffer[s.sendBytes], s.recvBytes - s.sendBytes, 0);
        if (sendLen == SOCKET_ERROR && ::WSAGetLastError() != WSAEWOULDBLOCK) {
            return E_FAIL;
        }

        s.sendBytes += sendLen;
        if (s.recvBytes == s.sendBytes) {
            s.recvBytes = 0;
            s.sendBytes = 0;
        }

        cout << "Server : Send Data= " << sendLen << endl;
    }

    return S_OK;
}
