#pragma once

#include "AsyncSocket.h"
#include "Types.h"

namespace netfish
{
	// Peer Clinet와 1-1 대응되는 소켓 클래스 : 네트워크 연결 상태 등만 관리
	class Client : public AsyncSocket
	{
	public:
		Client() = default;
		virtual ~Client() = default;

		void SetSessionId(SessionId sessionId) { m_sessionId = sessionId; }
		SessionId GetSessionId() { return m_sessionId; }

	private:
		// 세션 객체와 클라이언트(소켓) 객체와 1-1 대응을 해줌
		// 네트워크 연결이 끊겼다 하더라도 세션 객체는 살아있어야 하는 경우가 있다.
		// 세션은 다른 객체와 연동해서 처리가 완료되면 해제해야 함
		// 로그인 세션의 경우, 네트워크 연결 후 로그인을 하지 않아 객체는 생성되지 않을 수 있다.
		// 클라이언트 객체를 만든다음, 타임아웃을 준다.
		// 예제에서는 세션 객체를 만들어서 바로 관계를 맺어주지만 현실에서는 피어로부터 인증 코드를 받지 않으면
		// 일정 시간이 지난 후 서버에서 연결을 끊는 방식으로 전개된다.
		SessionId m_sessionId = 0;
	};
}