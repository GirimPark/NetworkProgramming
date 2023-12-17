#include "WinSockClient.h"

#include <iostream>
#include <ws2tcpip.h>

using namespace std;

bool WinSockClient::Connect()
{
	/// 소켓, 이벤트 생성
	m_clientSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (m_clientSocket == INVALID_SOCKET)
		return false;
	m_clientEvent = ::WSACreateEvent();

	/// 클라이언트 소켓에 4가지 이벤트 연결
	///	소켓을 넌블러킹 모드로 전환한다.
	if (::WSAEventSelect(m_clientSocket, m_clientEvent, FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR)
		return false;

	/// 클라이언트 소켓을 서버 주소에 연결한다.
	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = ::htons(m_serverPort);
	inet_pton(serverAddr.sin_family, m_serverIP, &(serverAddr.sin_addr));

	if (::connect(m_clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		return false;

	cout << "Clinet_Connecting" << endl;

	return true;
}

void WinSockClient::Update()
{
	/// 기다리는 이벤트
	int index = ::WSAWaitForMultipleEvents(1, &m_clientEvent, FALSE, 1000, FALSE);
	if (index == WSA_WAIT_FAILED)
		return;

	/// 소켓에 대한 이벤트 검색
	if (::WSAEnumNetworkEvents(m_clientSocket, m_clientEvent, &m_networkEvents) == SOCKET_ERROR)
	{
		cout << "EnumNetworkEvents Error" << WSAGetLastError() << endl;
		return;
	}

	/// 연결에 대한 이벤트, 연결 여부 확인
	if (m_networkEvents.lNetworkEvents & FD_CONNECT)
	{
		if (m_networkEvents.iErrorCode[FD_CONNECT_BIT] != 0)
		{
			cout << "Connect Error " << m_networkEvents.iErrorCode[FD_CONNECT_BIT] << endl;
			return;
		}

		cout << "Client : Connected" << endl;

		m_bConnected = true;
	}

	Receive();
	Send();
}

void WinSockClient::Receive()
{
	if (m_networkEvents.lNetworkEvents & FD_READ)
	{
		if (m_networkEvents.iErrorCode[FD_READ_BIT] != 0)
		{
			cout << "Read Error" << endl;
			return;
		}

		char buffer[BUFSIZE] = {};
		int recvBytes = ::recv(m_clientSocket, buffer, BUFSIZE, 0);
		if (recvBytes == SOCKET_ERROR)
		{
			cout << "Read Error" << endl;
			return;
		}

		if (recvBytes == 0)
		{
			cout << "Disconnected" << endl;
			return;
		}

		cout << "Client : Recv " << buffer << endl;
	}
}

void WinSockClient::Send()
{
	if (!m_bConnected)
		return;

	Sleep(1000);

	int sendLen = ::send(m_clientSocket, m_sendBuffer, strlen(m_sendBuffer) + 1, 0);
	if (sendLen == SOCKET_ERROR && ::WSAGetLastError() != WSAEWOULDBLOCK) {
		cout << "send Error " << ::WSAGetLastError() << endl;
	}
	else
	{
		cout << "Client : Send " << sendLen << endl;
	}
}

bool WinSockClient::Disconnect()
{
	closesocket(m_clientSocket);
	return true;
}
