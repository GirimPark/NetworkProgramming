#include <winsock2.h>
#include <WS2tcpip.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

namespace net_global
{

    void WnsockCleanup()
    {
	    
    }
}

// Session이 아니라 가볍게 Socket으로 만들자
class WinSocketBase
{
    /// 재사용성
    // 공통 API
    // 소켓 생성 Create
    // 주소 바인드
    // 연결 Connect
    // 리슨
    // 닫기 closeSocket : 디폴트 옵션으로 닫는다.

    // Attach, Dettach가 필요한 이유 : 소켓 풀을 만들때, 소켓 핸들 생성 전이므로 미리 만들어서
    // Accept의 결과가 소켓 핸들을 받고, 소켓 객체 풀에서 하나 꺼내서 바인드한다.
    // 소켓이 닫히면 해당 소켓 핸들은 더이상 사용하지 않으나, 소켓 객체는 다시 사용해야되므로 Detach 후 풀로 돌려준다.

	// 소켓 상태 : WinSocket으로 객체를 여러개 미리 만들어 둘 수 있다. 소켓 객체 풀, 소켓 생성 전

    /// 다형성
    virtual void OnAccept(int nErrorCode) = 0;
    virtual void OnClose(int nErrorCode) = 0;
    virtual void OnReceive(int nErrorCode) = 0;
    virtual void OnSend(int nErrorCode) = 0;
};

class MyWinSocket : public WinSocketBase
{
    virtual void OnAccept(int nErrorCode)
    {
	    // 받은 패킷을 분석해서 처리가 가능한 메시지로 분리하고 큐에 넣는다.
        // 큐에 처리가 가능한 메시지가 완성되었다면 해당 메시지를 처리하는 함수를 호출

        // OnMsgProc(&msg)
    }

    void OnMsgProc(void* pMsg)
    {
	    // 메시지 헤더의 패킷 아이디를 확인하고 각각 맞게 처리하는 함수를 호출한다.
    }

    // 상태, 세션 관리가 가능해진다.
};

int main()
{
    WSAData wsaData;

    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return 0;

    

    WSACleanup();
    return 0;
}
