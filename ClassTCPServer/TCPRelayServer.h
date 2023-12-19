#pragma once

#include "Types.h"
#include <unordered_map>
#include <vector>

namespace netfish
{
	class AsyncSocket;
	class Listener;
	class Client;
	class Session;

	class TCPRelayServer
	{
	public:
		TCPRelayServer() = default;
		~TCPRelayServer() = default;

		void Start();

		void Stop();

		void NetUpdate();

	private:

		void onAccept();

		void onReceive(AsyncSocket* pSocket);

		void onSend(AsyncSocket* pSocket);

		void onClose(AsyncSocket* pSocket);

		void onNetError(int errorCode, const char* errorMsg, AsyncSocket* pSocket = nullptr);

		std::vector<Client*> m_clients;
		std::unordered_map<SessionId, Session*> m_sessions;

		Listener* m_pListener = nullptr;

		int m_ClientCount = 0;

		// 이벤트셀렉트가 63개까지만 받을 수 있다.
		static constexpr int MAX_CLIENTS = 63;
	};
}
