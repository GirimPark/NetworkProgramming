#pragma once
#include "Types.h"
#include <vector>

const int _MIN = 100;

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

		void ReadUpdate();

		void NetUpdate();

		void SetClient(Client* pClient);

		const char* GetRecvMessage() { return m_readBuffer; }

		SessionId GetSessionId() { return m_sessionId; }

	private:

		Client* m_pClient = nullptr;

		static constexpr int BUF_SIZE = 1024;

		char m_readBuffer[BUF_SIZE] = {};
		char m_writeBuffer[BUF_SIZE] = {};

		int m_readBytes = 0;
		int m_writeBytes = 0;

		SessionId m_sessionId = 0;

		static SessionId s_sessionIdCounter;
	};
}
