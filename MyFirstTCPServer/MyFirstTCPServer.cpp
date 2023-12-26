#include <iostream>
#include "../NetworkLibrary/WinSockServer.h"

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

int main()
{
    WSAData wsaData;

    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return 0;

    WinSockServer server(7777);

    while(true)
    {
        HRESULT rt = server.Accept();
        if (rt == E_FAIL)
            continue;
        else if (rt == E_ABORT)
            return 0;

        rt = server.Update();
        if (rt == E_FAIL)
            continue;
        else if (rt == E_ABORT)
            return 0;
    }

    WSACleanup();
    return 0;
}
