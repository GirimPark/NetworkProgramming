#pragma once

#include <string>
#include <string_view>

#include "WinSockBase.h"

class WinSockClient : public WinSockBase
{
private:
	bool m_bConnected = false;
	WSAEVENT m_clientEvent;
	WSANETWORKEVENTS m_networkEvents;

	std::string m_sendBuffer;

public:
	WinSockClient(const char* serverIP, short serverPort);

public:
	void SetMessage(std::string_view message) { m_sendBuffer = message; }

public:
	bool Connect();

public:
	HRESULT Update() override;

private:
	HRESULT Receive() override;
	HRESULT Send() override;
};

