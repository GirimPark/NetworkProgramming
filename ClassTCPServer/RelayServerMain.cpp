#include <iostream>
#include "TCPRelayServer.h"

int main()
{
	// ��Ŷ�� ���� Ŭ�� ���� ���ӵ� �ٸ� Ŭ�󿡵� �޽����� ������.
	// ���� ���� Ŭ���̾�Ʈ ������ ������ �� �־�� �Ѵ�.
	netfish::TCPRelayServer server;
	server.Start();

	while(true)
	{
		server.NetUpdate();
	}

	server.Stop();
	return 0;
}