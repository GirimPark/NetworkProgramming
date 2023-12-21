#pragma once
#include "Types.h"
#include <string>

namespace netfish
{
	class Client;

	// 클라이언트가 직접 send하지 않고, 세션을 통해 요청한다.
	// 실제 처리하는 주체는 클라이언트가 아닌 세션

	// ex) 로그인을 하는 중간 단계
	// 같은 id 접속 시도가 들어오면, 패스워드는 db단까지 가야 알 수 있다.
	// 두 id로 들어온 접속을 별개로 처리해야 한다. 접속할 때마다 세션은 id를 새로 발급해야 한다.
	class Session
	{
	public:
		Session();

		~Session();

		void Write(const char* pData, int len);

		void NetUpdate();

		void SetClient(Client* pClient);

		const char* GetRecvMessage() { return m_readBuffer; }

		SessionId GetSessionId() { return m_sessionId; }

	private:
		void Read(char* pData, int len);

	private:

		Client* m_pClient = nullptr;

		static constexpr int BUF_SIZE = 1024;

		char m_readBuffer[BUF_SIZE] = {};
		char m_writeBuffer[BUF_SIZE] = {};

		SessionId m_sessionId = 0;

		static SessionId s_sessionIdCounter;
	};
}
