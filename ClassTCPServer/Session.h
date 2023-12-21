#pragma once
#include "Types.h"
#include <string>

namespace netfish
{
	class Client;

	// Ŭ���̾�Ʈ�� ���� send���� �ʰ�, ������ ���� ��û�Ѵ�.
	// ���� ó���ϴ� ��ü�� Ŭ���̾�Ʈ�� �ƴ� ����

	// ex) �α����� �ϴ� �߰� �ܰ�
	// ���� id ���� �õ��� ������, �н������ db�ܱ��� ���� �� �� �ִ�.
	// �� id�� ���� ������ ������ ó���ؾ� �Ѵ�. ������ ������ ������ id�� ���� �߱��ؾ� �Ѵ�.
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
