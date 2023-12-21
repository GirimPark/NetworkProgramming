#include <iostream>
#include "TCPRelayServer.h"

int main()
{
	// 패킷을 보낸 클라 외의 접속된 다른 클라에도 메시지를 보낸다.
	// 여러 개의 클라이언트 연결을 관리할 수 있어야 한다.
	netfish::TCPRelayServer server;
	server.Start();

	while(true)
	{
		server.NetUpdate();
	}

	server.Stop();
	return 0;
}