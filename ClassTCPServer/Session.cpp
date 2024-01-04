#include "Session.h"
#include "Client.h"

#include "TCPRelayServer.h"
#include "../Common/MyProtocol.h"

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
		memcpy(m_writeBuffer + m_writeBytes, pData, len);
		m_writeBytes += len;
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
			// ���� �����͸� �״�� �۽� ���ۿ� �����غ���.
			//memcpy(m_writeBuffer + m_writeBytes, m_readBuffer, m_readBytes);
			//m_writeBytes += m_readBytes;
			//memmove(m_readBuffer, m_readBuffer + m_readBytes, BUF_SIZE - m_readBytes);
			//m_readBytes = 0;
			//------------------------------------------------------------------------------------------------

			// ���� ���ۿ� �����Ͱ� ������ ��ȿ�� ��Ŷ���� Ȯ���ϰ�, ��ȿ�� ��Ŷ�̸� �޽����� ó���Ѵ�.
			// '��ȿ�� ��Ŷ���� Ȯ��' -> Read�� �ϰ� m_readBuffer�� �����͸� �м�

			// ���� ������ ��Ŷ ����� 2����Ʈ-������, 2����Ʈ-���̵�� �̷�����ִ�.(MyProtocol.h)
			// 1. ���� ������ ù 2����Ʈ�� �о ����� Ȯ���Ѵ�. �񼺻����̶�� ���� �ʴ´�.
			// 2. ������ ������ 30����Ʈ���, ó������ �ڿ� ������ ������ ũ�⸦ �����ؼ�
			//    ���� ���ۿ� 30����Ʈ �̻� �ִ��� Ȯ���Ѵ�.
			// 3. ������ ������ ����ϴٸ� ��Ŷ ���̵� Ȯ���Ѵ�. ���ǵ� ���̵� �ƴ϶�� ���� �ʴ´�.(�α׸� ����ų�)
			// 4. ��� �����ϴٸ� ��Ŷ id���� ó�� �Լ��� ȣ���ϴ� ����� ���� �����ϴ�. (���Ŀ��� ť�� �־ ó���Ѵ�.)
			// ��Ŷ ť�� �ְ� ��� ������ ��Ŷ ť�� �ϼ��Ǹ� �ܺο��� ó���Ѵ�.

			// ��Ŷ ������ ����
			short size = (m_readBuffer[0] - '0') * 10 + (m_readBuffer[1] - '0');
			if (size < 0 || size > m_readBytes)
				return;
			int packetId = (m_readBuffer[2] - '0') * 10 + (m_readBuffer[3] - '0');
			if (packetId <= EPacketId::PACKETID_START || packetId >= EPacketId::PACKETID_END)
				return;

			// ��Ŷ�� ó���� Server�� ��Ŷ �߰�
			char* packet = new char[size];
			memcpy(packet, m_readBuffer, size);
			TCPRelayServer::GetInstance()->AddProcessPacket(packet);

			// readBuffer ����
			m_readBytes -= size;
			memmove(m_readBuffer, m_readBuffer + size, m_readBytes);
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