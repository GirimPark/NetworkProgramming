#pragma once

#include "WinSockBase.h"

#include <vector>

struct Session
{
	SOCKET socket = INVALID_SOCKET;

	char recvBuffer[BUFSIZE] = {};

	int recvBytes = 0;
	int sendBytes = 0;
};

class WinSockServer : public WinSockBase
{
private:
	short m_listeningPort;
	SOCKET m_listeningSocket;
	
	int m_index;

	std::vector<WSAEVENT> m_wsaEvents;
	std::vector<Session> m_sessions;

public:
	WinSockServer(short listeningPort);

public:
	HRESULT Accept();

public:
	HRESULT Update() override;

private:
	bool SetupListen();

private:
	HRESULT Receive() override;
	HRESULT Send() override;
};

