 //CAsyncSocket을 참고해서 클래스로 만들기
 //해당 클래스를 사용해서 서버에 연결하는 코드 작성
 //eventSelect 예제 참고
 //eventSelect 서버에 연결 확인
#include <iostream>
#include "WinSockClient.h"

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

int main()
{
    WSAData wsaData;
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return 0;

    WinSockClient client;
    client.Connect();
    while(true)
    {
        client.Update();
    }

    client.Disconnect();
    WSACleanup();
    return 0;
}
