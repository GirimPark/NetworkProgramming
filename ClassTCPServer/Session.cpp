#include "Session.h"
#include "Client.h"
#include <assert.h>

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

	void Session::ReadUpdate()
	{
		// readBuffer�� �����͸� �״� �뵵
		// ���Ͽ� m_readBuffer�� ������ �о���� �Ѵ�.
		// TODO: ���� ��Ŷ�� ��� �Ľ��� ���ΰ��� ���� ������ �ٲ��.

		assert(m_pClient);

		int bufLen = BUF_SIZE - m_readBytes;

		int nRead = m_pClient->Recv(m_readBuffer + m_readBytes, bufLen);
		m_readBytes += nRead;
	}

	// ���Ͽ� �����ϴ� buffer�� Ŀ�� �޸𸮿� �����Ѵ�.
	// ���ǿ� �ִ� buffer(���� �޸�) -> ���Ͽ� �ִ� buffer(Ŀ�� �޸�)�� memcpy�� �̷������
	// �ý��� �ݿ� ���� ���ϸ� ���̱� ���� NetUpdate���� ��Ŷ�� �м� �� ������ ������ Write�Ѵ�.
	// Ŀ�� �޸𸮴� ���� �޸� ����� ������� �ʴ´�. 
	void Session::NetUpdate()
	{
		if (m_readBytes)
		{
			// ���� ���ۿ� �����Ͱ� ������ ��ȿ�� ��Ŷ���� Ȯ���ϰ�, ��ȿ�� ��Ŷ�̸� �޽����� ó���Ѵ�.
			// '��ȿ�� ��Ŷ���� Ȯ��' -> Read�� �ϰ� m_readBuffer�� �����͸� �м�

			// ���� ������ ��Ŷ ����� 2����Ʈ-������, 2����Ʈ-���̵�� �̷�����ִ�.(MyProtocol.h)
			// 1. ���� ������ ù 2����Ʈ�� �о ����� Ȯ���Ѵ�. �񼺻����̶�� ���� �ʴ´�.
			// 2. ������ ������ 30����Ʈ���, ó������ �ڿ� ������ ������ ũ�⸦ �����ؼ�
			//    ���� ���ۿ� 30����Ʈ �̻� �ִ��� Ȯ���Ѵ�.
			// 3. ������ ������ ����ϴٸ� ��Ŷ ���̵� Ȯ���Ѵ�. ���ǵ� ���̵� �ƴ϶�� ���� �ʴ´�.(�α׸� ����ų�)
			// 4. ��� �����ϴٸ� ��Ŷ id���� ó�� �Լ��� ȣ���ϴ� ����� ���� �����ϴ�. (���Ŀ��� ť�� �־ ó���Ѵ�.)

			// �켱, ���� �����͸� �״�� �۽� ���ۿ� �����غ���.

			memcpy(m_writeBuffer + m_writeBytes, m_readBuffer, m_readBytes);
			m_writeBytes += m_readBytes;
			memmove(m_readBuffer, m_readBuffer + m_readBytes, BUF_SIZE - m_readBytes);
			m_readBytes = 0;
		}

		// �۽� ���ۿ� �����Ͱ� ������ Ŭ���̾�Ʈ���� �����Ѵ�. m_pClient->Send
		if(m_writeBytes)
		{
			int nSent = m_pClient->Send(m_writeBuffer, m_writeBytes);

			if(nSent > 0)
			{
				m_writeBytes -= nSent;

				if(m_writeBytes > 0)
				{
					memmove(m_writeBuffer, m_writeBuffer + nSent, m_writeBytes - nSent);
				}
			}
			else if(nSent == 0)
			{
				// ���� ���۰� ���� ���� ������ �Ұ����� ���

			}
			else
			{
				// ���� ����

			}
		}
	}

	void Session::SetClient(Client* pClient)
	{
		m_sessionId = ++s_sessionIdCounter;

		m_pClient = pClient;

		m_pClient->SetSessionId(m_sessionId);
	}

}