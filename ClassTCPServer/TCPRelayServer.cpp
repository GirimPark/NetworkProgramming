#include "TCPRelayServer.h"
#include "Client.h"
#include "Session.h"
#include <cstdio>

// link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

namespace netfish
{
	class Listener : public AsyncSocket
	{
	public:
		Listener() = default;
		~Listener() = default;

		bool OnAccept(AsyncSocket* pSocket) override
		{
			return (Accept(*pSocket));
		}
	};


	void TCPRelayServer::Start()
	{
		AsyncSocket::InitSock();

		m_pListener = new Listener();

		m_pListener->Create(SOCK_STREAM, FD_ACCEPT);

		m_pListener->Bind(7777, "127.0.0.1");//172.21.1.61

		m_pListener->Listen();
	}

	void TCPRelayServer::Stop()
	{
		// 소멸자에서 소켓을 닫기는 하지만 좋은 방법은 아니다.
		// 소켓 버퍼에 보낼 내용이 남아있음에도 닫을 수 있다. 성능상 문제가 되나?

		delete m_pListener;
		m_pListener = nullptr;

		for(auto& pClient : m_clients)
		{
			delete pClient;
		}

		for(auto& pSession : m_sessions)
		{
			delete pSession.second;
		}

		AsyncSocket::CleanupSock();
	}

	void TCPRelayServer::NetUpdate()
	{
		if (!m_pListener) return;

		std::vector<WSAEVENT> wsaEvents;

		wsaEvents.push_back(m_pListener->GetEvent());

		for(auto& pClient : m_clients)
		{
			if (pClient == nullptr) break;
			wsaEvents.push_back(pClient->GetEvent());
		}

		// 이벤트를 기다리는 시간을 1ms로 하면 호출 스레드는 대기 상태가 된다.
		// 메인(렌더) 스레드에서 딜레이를 주고 싶지 않으면 어떻게 해야 할까?
		// 각 작업을 스레드로 분리하면 TIME_OUT을 무한히 기다리거나, sleep을 걸어도 된다
		// sleep 0: 타임 슬라이스가 끝나지 않더라도 작업을 대기 상태로 전환할 것인지 OS에게 판단을 맡긴다
		int index = ::WSAWaitForMultipleEvents(wsaEvents.size(), &wsaEvents[0], FALSE, 1, FALSE);

		if(index == WSA_WAIT_FAILED)
		{
			printf("WSAWaitForMultipleEvents Error %d ", WSAGetLastError());
			return;
		}

		if (index == WSA_WAIT_TIMEOUT) return;

		index -= WSA_WAIT_EVENT_0;

		AsyncSocket* pSocket = nullptr;

		// 이벤트가 발생한 소켓 핸들을 찾아야 한다.
		if (wsaEvents[index] == m_pListener->GetEvent()) pSocket = m_pListener;
		else pSocket = m_clients[index - 1];

		WSANETWORKEVENTS networkEvents;
		// 이벤트를 계속 발생시키다보면 힙 에러 발생(Critical error detected c0000374)
		if (::WSAEnumNetworkEvents(pSocket->GetHandle(), wsaEvents[index], &networkEvents) == SOCKET_ERROR)
		{
			printf("WSAEnumNetworkEvents Error %d", WSAGetLastError());

			return;
		}

		if (networkEvents.lNetworkEvents & FD_ACCEPT)
		{
			if (networkEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
			{
				onNetError(networkEvents.iErrorCode[FD_ACCEPT_BIT], "Accept");
				return;
			}

			onAccept();
		}

		if (networkEvents.lNetworkEvents & FD_READ)
		{
			if (networkEvents.iErrorCode[FD_READ_BIT] != 0)
			{
				onNetError(networkEvents.iErrorCode[FD_READ_BIT], "Recv", pSocket);
				return;
			}

			onReceive(pSocket);
		}

		if (networkEvents.lNetworkEvents & FD_WRITE)
		{
			if (networkEvents.iErrorCode[FD_WRITE_BIT] != 0)
			{
				onNetError(networkEvents.iErrorCode[FD_WRITE_BIT], "Write", pSocket);
				return;
			}

			onSend(pSocket);
		}

		if (networkEvents.lNetworkEvents & FD_CLOSE)
		{
			if (networkEvents.iErrorCode[FD_CLOSE_BIT] != 0)
			{
				onNetError(networkEvents.iErrorCode[FD_CLOSE_BIT], "Close", pSocket);
			}

			onClose(pSocket);
		}

		//네트워크 이벤트 처리가 모두 끝나면


		// 각 세션 별로 수신 버퍼에 받아온 데이터를 처리하고
		// 송신 버퍼에 있는 데이터를 실제 소켓 송신을 처리해야 합니다.
		for (auto& session : m_sessions)
		{
			session.second->NetUpdate();
		}
	}

	void TCPRelayServer::onAccept()
	{
		printf("onAccept\n");

		Client* pClient = new Client();
		Session* pSession = new Session();

		if (m_pListener->OnAccept(pClient))
		{

			pSession->SetClient(pClient);
			m_clients.push_back(pClient);
			m_sessions[pSession->GetSessionId()] = pSession;

			++m_ClientCount;

		}
		else
		{
			delete pClient;
			delete pSession;

			printf("Accept Error %d\n", WSAGetLastError());
		}

		printf("연결된 클라이언트 수 : %d \n", m_ClientCount);
	}

	void TCPRelayServer::onReceive(AsyncSocket* pSocket)
	{
		printf("onReceive  %s, %d\n", pSocket->GetIP().c_str(), pSocket->GetPort());

		Client* pClient = dynamic_cast<Client*>(pSocket);
		if (pClient == nullptr) return;

		Session* pSession = m_sessions[pClient->GetSessionId()];

		if (pSession == nullptr) return;

		pSession->ReadUpdate();
	}

	void TCPRelayServer::onSend(AsyncSocket* pSocket)
	{
		printf("onSend  %s : %d\n", pSocket->GetIP().c_str(), pSocket->GetPort());
	}

	void TCPRelayServer::onClose(AsyncSocket* pSocket)
	{
		printf("onClose  %s : %d", pSocket->GetIP().c_str(), pSocket->GetPort());

		// 클라이언트 소켓이 닫히면 클라이언트를 삭제합니다. 연결된 세션도 삭제 합니다.
		Client* pClient = dynamic_cast<Client*>(pSocket);
		if (pClient == nullptr) return;

		m_sessions.erase(pClient->GetSessionId());

		for (auto iter = m_clients.begin(); iter != m_clients.end(); ++iter)
		{
			if (*iter == pClient)
			{
				m_clients.erase(iter);
				break;
			}
		}
		//TODO:숙제: 이코드의 문제점은 무엇일까요?

		delete pClient;


		m_ClientCount--;
		printf("연결된 클라이언트 수 : %d \n", m_ClientCount);
	}

	void TCPRelayServer::onNetError(int errorCode, const char* errorMsg, AsyncSocket* pSocket)
	{
		if (errorMsg)
		{
			printf("\n onNetError %s \t", errorMsg);
		}

		if (pSocket)
		{
			printf("\n onNetError  %s : %d", pSocket->GetIP().c_str(), pSocket->GetPort());
		}

		printf("NetErrorCode  %d  \n", errorCode);
	}
}
