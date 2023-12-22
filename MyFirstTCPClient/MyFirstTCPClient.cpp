 //CAsyncSocket을 참고해서 클래스로 만들기
 //해당 클래스를 사용해서 서버에 연결하는 코드 작성
 //eventSelect 예제 참고
 //eventSelect 서버에 연결 확인
#include <iostream>
#include "../NetworkLibrary/WinSockClient.h"

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

int main()
{
    WSAData wsaData;
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return 0;

    WinSockClient client("172.21.1.61", 7777);
    client.SetMessage("구다");
    client.Connect();

    bool isConnected = true;

    while(true)
    {
        client.Update();

        if(GetAsyncKeyState(VK_UP) & 0x8001)
        {
            isConnected = false;

            client.DisConnect();
            break;
        }
    }

    if(!isConnected)
    {
        Sleep(5000);
    }

    WSACleanup();
    return 0;
}
