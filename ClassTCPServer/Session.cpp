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
		// m_pClinet->Send를 바로 호출하지 않고 m_writeBuffer에 복사해둔다
	}

	void Session::Read(char* pData, int len)
	{
		// 소켓에 m_readBuffer를 던져서 읽어오게 한다.
		// TODO: 받은 패킷을 어떻게 파싱할 것인가에 따라 구조가 바뀐다.
	}

	// 소켓에 존재하는 buffer는 커널 메모리에 존재한다.
	// 세션에 있는 buffer(유저 메모리) -> 소켓에 있는 buffer(커널 메모리)로 memcpy가 이루어진다
	// 시스템 콜에 대한 부하를 줄이기 위해 NetUpdate에서 패킷을 분석 후 일정한 단위로 Write한다.
	// 커널 메모리는 가상 메모리 기법을 사용하지 않는다. 
	void Session::NetUpdate()
	{
		// 송신 버퍼에 데이터가 있으면 클라이언트에게 전송한다. m_pClient->Send
		// 수신 버퍼에 데이터가 있으면 유효한 패킷인지 확인하고, 유효한 패킷이면 메시지를 처리한다.
		// '유효한 패킷인지 확인' -> Read를 하고 m_readBuffer의 데이터를 분석
	}

	void Session::SetClient(Client* pClient)
	{
		m_sessionId = ++s_sessionIdCounter;

		m_pClient = pClient;

		m_pClient->SetSessionId(m_sessionId);
	}

}