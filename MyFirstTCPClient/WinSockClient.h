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
    char m_sendBuffer[BUFSIZE] = "������ �ſ�";
    WSANETWORKEVENTS m_networkEvents;

public:
    bool Connect();
    void Update();
    bool Disconnect();

private:
    void Receive();
    void Send();
};
// ���� Ŭ������ ��Ʈ��ũ �۾��� ���� ����
// �����ͱ׷��̳� ��Ʈ���� ������� �� �������ְ� �־�� ��
// ��Ŷ ������ ���� �����ؾ��� ��Ŷ �����̳ʰ� �޶����ϴ�
// ��Ʈ���̶�� ���ۿ� ��� ���� �����͸� �����ϰ�
// �����ͱ׷��̶�� �� ��Ŷ�� �и��Ǿ� ����˴ϴ�.