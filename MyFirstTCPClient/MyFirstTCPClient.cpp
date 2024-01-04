 //CAsyncSocket을 참고해서 클래스로 만들기
 //해당 클래스를 사용해서 서버에 연결하는 코드 작성
 //eventSelect 예제 참고
 //eventSelect 서버에 연결 확인
#include <iostream>
#include "../NetworkLibrary/WinSockClient.h"
#include "../Common/MyProtocol.h"

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

int main()
{
    WSAData wsaData;
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return 0;

    WinSockClient client("127.0.0.1", 7777); //172.21.1.61
    //client.SetMessage("구다", 5);
    client.Connect();

    bool isConnected = true;

    // 클라이언트에서 키 입력으로 보낸 메시지를
    // 서버에서 수신 버퍼에 받고, 송신 버퍼로 복사한다.
    // 클라이언트에서는 다시 서버에서 받은 메시지를 출력한다.
    // 서버의 수신 버퍼 메모리 위치 관리(memcpy)
    while(true)
    {
        client.Update();

        if(GetAsyncKeyState(VK_UP) & 0x8001)
        {
            isConnected = false;

            client.DisConnect();
            break;
        }
        else if(GetAsyncKeyState('A') & 0x8001)
        {
            client.SendMessageMy("1003hello", 10);

            //PacketC2S_Hello c2sMsg;
            //c2sMsg.size = ;
            //c2sMsg.id = ';
            //client.SendMessageMy
        	
        }
    }

    if(!isConnected)
    {
        Sleep(5000);
    }

    WSACleanup();
    return 0;
}
