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
		// m_pClinet->Send를 바로 호출하지 않고 m_writeBuffer에 복사해둔다
	}

	void Session::ReadUpdate()
	{
		// readBuffer에 데이터를 쌓는 용도
		// 소켓에 m_readBuffer를 던져서 읽어오게 한다.
		// TODO: 받은 패킷을 어떻게 파싱할 것인가에 따라 구조가 바뀐다.

		assert(m_pClient);

		int bufLen = BUF_SIZE - m_readBytes;

		int nRead = m_pClient->Recv(m_readBuffer + m_readBytes, bufLen);
		m_readBytes += nRead;
	}

	// 소켓에 존재하는 buffer는 커널 메모리에 존재한다.
	// 세션에 있는 buffer(유저 메모리) -> 소켓에 있는 buffer(커널 메모리)로 memcpy가 이루어진다
	// 시스템 콜에 대한 부하를 줄이기 위해 NetUpdate에서 패킷을 분석 후 일정한 단위로 Write한다.
	// 커널 메모리는 가상 메모리 기법을 사용하지 않는다. 
	void Session::NetUpdate()
	{
		if (m_readBytes)
		{
			// 수신 버퍼에 데이터가 있으면 유효한 패킷인지 확인하고, 유효한 패킷이면 메시지를 처리한다.
			// '유효한 패킷인지 확인' -> Read를 하고 m_readBuffer의 데이터를 분석

			// 현재 정의한 패킷 헤더는 2바이트-사이즈, 2바이트-아이디로 이루어져있다.(MyProtocol.h)
			// 1. 수신 버퍼의 첫 2바이트를 읽어서 사이즈를 확인한다. 비성상적이라면 받지 않는다.
			// 2. 사이즈 정보가 30바이트라면, 처음부터 뒤에 수신한 데이터 크기를 포함해서
			//    수신 버퍼에 30바이트 이상 있는지 확인한다.
			// 3. 사이즈 정보가 충분하다면 패킷 아이디를 확인한다. 정의된 아이디가 아니라면 받지 않는다.(로그를 남기거나)
			// 4. 모두 부합하다면 패킷 id별로 처리 함수를 호출하는 방법이 가장 간단하다. (이후에는 큐에 넣어서 처리한다.)

			// 우선, 받은 데이터를 그대로 송신 버퍼에 복사해본다.

			memcpy(m_writeBuffer + m_writeBytes, m_readBuffer, m_readBytes);
			m_writeBytes += m_readBytes;
			memmove(m_readBuffer, m_readBuffer + m_readBytes, BUF_SIZE - m_readBytes);
			m_readBytes = 0;
		}

		// 송신 버퍼에 데이터가 있으면 클라이언트에게 전송한다. m_pClient->Send
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
				// 소켓 버퍼가 가득 차서 전송이 불가능한 경우

			}
			else
			{
				// 소켓 에러

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