#include "pch.h"
#include "WinSockClient.h"

#include <iostream>
#include <ws2tcpip.h>

using namespace std;

WinSockClient::WinSockClient(const char* serverIP, short serverPort)
{
	m_peerIP = serverIP;
	m_peerPort = serverPort;
}

bool WinSockClient::Connect()
{
	/// ����, �̺�Ʈ ����
	m_socket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == INVALID_SOCKET)
		return false;
	m_clientEvent = ::WSACreateEvent();

	/// Ŭ���̾�Ʈ ���Ͽ� 4���� �̺�Ʈ ����
	///	������ �ͺ�ŷ ���� ��ȯ�Ѵ�.
	if (::WSAEventSelect(m_socket, m_clientEvent, FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR)
		return false;

	/// Ŭ���̾�Ʈ ������ ���� �ּҿ� �����Ѵ�.
	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = ::htons(m_peerPort);
	inet_pton(serverAddr.sin_family, m_peerIP, &(serverAddr.sin_addr));

	if (::connect(m_socket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		return false;

	cout << "Clinet_Connecting" << endl;

	return true;
}

HRESULT WinSockClient::Update()
{
	/// ��ٸ��� �̺�Ʈ
	int index = ::WSAWaitForMultipleEvents(1, &m_clientEvent, FALSE, 1000, FALSE);
	if (index == WSA_WAIT_FAILED)
		return E_ABORT;

	/// ���Ͽ� ���� �̺�Ʈ �˻�
	if (::WSAEnumNetworkEvents(m_socket, m_clientEvent, &m_networkEvents) == SOCKET_ERROR)
	{
		cout << "EnumNetworkEvents Error" << WSAGetLastError() << endl;
		return E_ABORT;
	}

	/// ���ῡ ���� �̺�Ʈ, ���� ���� Ȯ��
	if (m_networkEvents.lNetworkEvents & FD_CONNECT)
	{
		if (m_networkEvents.iErrorCode[FD_CONNECT_BIT] != 0)
		{
			cout << "Connect Error " << m_networkEvents.iErrorCode[FD_CONNECT_BIT] << endl;
			return E_ABORT;
		}

		cout << "Client : Connected" << endl;

		m_bConnected = true;
	}

	__super::Update();
}

HRESULT WinSockClient::Receive()
{
	if (m_networkEvents.lNetworkEvents & FD_READ)
	{
		if (m_networkEvents.iErrorCode[FD_READ_BIT] != 0)
		{
			cout << "Read Error" << endl;
			return E_ABORT;
		}

		char buffer[BUFSIZE] = {};
		int recvBytes = ::recv(m_socket, buffer, BUFSIZE, 0);
		if (recvBytes == SOCKET_ERROR)
		{
			cout << "Read Error" << endl;
			return E_ABORT;
		}

		if (recvBytes == 0)
		{
			cout << "Disconnected" << endl;
			return E_ABORT;
		}

		cout << "Client : Recv " << buffer << endl;
	}

	return S_OK;
}

HRESULT WinSockClient::Send()
{
	if (!m_bConnected)
		return E_ABORT;

	Sleep(1000);

	int sendLen = ::send(m_socket, m_sendBuffer.c_str(), m_sendBuffer.size(), 0);
	if (sendLen == SOCKET_ERROR && ::WSAGetLastError() != WSAEWOULDBLOCK) {
		cout << "send Error " << ::WSAGetLastError() << endl;
	}
	else
	{
		cout << "Client : Send " << sendLen << endl;
	}

	return S_OK;
}
