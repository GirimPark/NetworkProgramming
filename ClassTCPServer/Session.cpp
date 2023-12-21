#include "Session.h"
#include "Client.h"

namespace netfish
{
	SessionId Session::s_sessionIdCounter;

	Session::Session()
	{

	}

	Session::~Session()
	{
		m_pClient->Close();
	}

	void Session::Write(const char* pData, int len)
	{
		// m_pClinet->Send�� �ٷ� ȣ������ �ʰ� m_writeBuffer�� �����صд�
	}

	void Session::Read(char* pData, int len)
	{
		// ���Ͽ� m_readBuffer�� ������ �о���� �Ѵ�.
		// TODO: ���� ��Ŷ�� ��� �Ľ��� ���ΰ��� ���� ������ �ٲ��.
	}

	// ���Ͽ� �����ϴ� buffer�� Ŀ�� �޸𸮿� �����Ѵ�.
	// ���ǿ� �ִ� buffer(���� �޸�) -> ���Ͽ� �ִ� buffer(Ŀ�� �޸�)�� memcpy�� �̷������
	// �ý��� �ݿ� ���� ���ϸ� ���̱� ���� NetUpdate���� ��Ŷ�� �м� �� ������ ������ Write�Ѵ�.
	// Ŀ�� �޸𸮴� ���� �޸� ����� ������� �ʴ´�. 
	void Session::NetUpdate()
	{
		// �۽� ���ۿ� �����Ͱ� ������ Ŭ���̾�Ʈ���� �����Ѵ�. m_pClient->Send
		// ���� ���ۿ� �����Ͱ� ������ ��ȿ�� ��Ŷ���� Ȯ���ϰ�, ��ȿ�� ��Ŷ�̸� �޽����� ó���Ѵ�.
		// '��ȿ�� ��Ŷ���� Ȯ��' -> Read�� �ϰ� m_readBuffer�� �����͸� �м�
	}

	void Session::SetClient(Client* pClient)
	{
		m_sessionId = ++s_sessionIdCounter;

		m_pClient = pClient;

		m_pClient->SetSessionId(m_sessionId);
	}

}