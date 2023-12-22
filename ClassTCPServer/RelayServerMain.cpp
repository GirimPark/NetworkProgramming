#include <iostream>
#include "TCPRelayServer.h"

// TODO
// 1. 윈속 클래스를 만들고, 그 클래스를 이용해서 클라이언트를 만든다.
// 2. 주어진 샘플 서버에서 TCPRelayServer::onClose 부분을 수정한다
// 3. 주어진 샘플 서버에서 Session::NetUpdate() 등을 수정해서
//    클라이언트가 보내는 데이터를 그대로 돌려주는 코드를 작성한다.

// 여기까지 되면 다른 머신에서 클라, 서버를 띄우고
// 패킷을 보낸 크기와 받은 크기를 각각 확인한다.
// 그리고 강제로 브레이크를 걸거나 슬립을 걸어서 서버가 패킷을 받는 속도를 지연하고
// 그때, 클라이언트에서 어떤 현상이 나타나는지 확인한다.

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