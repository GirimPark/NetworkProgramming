#pragma once

#include <string>
#include <string_view>

#include "WinSockBase.h"

class WinSockClient : public WinSockBase
{
private:
	bool m_bConnected = false;

	char m_sendBuffer[BUFSIZE];
	int m_sendBytes = 0;

public:
	WinSockClient(const char* serverIP, short serverPort);

public:
	void SetMessage(const char* message, int size) { memcpy(m_sendBuffer, message, size); }
	void SendMessageMy(const char* message, int size) { memcpy(m_sendBuffer + m_sendBytes, message, size); m_sendBytes += size; }

public:
	bool Connect();

	void DisConnect();

	bool GetConnected() const { return m_bConnected; }
	void SetConnected(bool connected) { m_bConnected = connected; }

public:
	HRESULT Update() override;

private:
	HRESULT Receive() override;
	HRESULT Send() override;
};

