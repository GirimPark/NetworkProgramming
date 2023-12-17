#pragma once
#include <winsock2.h>

constexpr int BUFSIZE = 1024;

class WinSockClient
{
private:
    const char* m_serverIP = "127.0.0.1";
    short m_serverPort = 7777;

    bool m_bConnected = false;
    SOCKET m_clientSocket;
    WSAEVENT m_clientEvent;
    char m_sendBuffer[BUFSIZE] = "이히히 돼요";
    WSANETWORKEVENTS m_networkEvents;

public:
    bool Connect();
    void Update();
    bool Disconnect();

private:
    void Receive();
    void Send();
};
// 소켓 클래스는 네트워크 작업을 위한 래퍼
// 데이터그램이나 스트림에 상관없이 다 제공해주고 있어야 함
// 패킷 종류에 따라 관리해야할 패킷 컨테이너가 달라집니다
// 스트림이라면 버퍼에 계속 읽은 데이터를 누적하고
// 데이터그램이라면 각 패킷은 분리되어 저장됩니다.