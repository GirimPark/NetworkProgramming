// �ͺ�ŷ ����
#include <winsock2.h>

#include <WS2tcpip.h>
#include <iostream>
#include <vector>

#include "LoopThread.h"

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

constexpr int BUFSIZE = 1024;

struct Session
{
    SOCKET socket = INVALID_SOCKET;

    char recvBuffer[BUFSIZE] = {};

    int recvBytes = 0;
    int sendBytes = 0; // ������ŭ ���� �ִ� �ΰ����� ���� ����
};

class CNetClient : public CLoopThreadBase
{
public:
    CNetClient() {}
    virtual ~CNetClient() {}

    bool Connect();

private:
    virtual    void    DoUpdate() override;

    SOCKET    m_clientSocket = INVALID_SOCKET;

    WSAEVENT m_clientEvent = WSA_INVALID_EVENT;

    bool m_isConnected = false;

    char m_sendBuffer[BUFSIZE] = "";
};

bool CNetClient::Connect()
{
    /// ����, �̺�Ʈ ����
    m_clientSocket = ::socket(AF_INET, SOCK_STREAM, 0);

    if (m_clientSocket == INVALID_SOCKET) return false;

    m_clientEvent = ::WSACreateEvent();

    // Ŭ���̾�Ʈ ���� �ڵ鿡 FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE 4������ ��Ʈ��ũ ��ȣ�� ������
    // m_clinetEvnet �̺�Ʈ ��ü�� ���� �� �� �ֵ��� �����Ѵ�.
    // m_clinetSocket�� �ͺ�ŷ ���� �ٲ��ش�.
    if (::WSAEventSelect(m_clientSocket, m_clientEvent, FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR) 
        return 0;

    /// Ŭ���̾�Ʈ ������ ���� �ּҿ� �����Ѵ�.
    SOCKADDR_IN serverAddr;
    ::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = ::htons(7777);
    inet_pton(serverAddr.sin_family, "127.0.0.1", &(serverAddr.sin_addr)); // �ٸ� �ּҷ� �ٲ㼭 �׽�Ʈ �غ�����.

    // m_clinetSocket�� ��ŷ �����̶�� ������ �� ���� ������ ��ŷ �ȴ�.
    // �׷��� �ͺ�ŷ �����̹Ƿ� �̺�Ʈ ��ü�� ���� FD_CONNECT ��Ʈ��ũ �̺�Ʈ�� �߻��Ͽ� �ش� �Լ��� ������ �ʴ´�.
    if (::connect(m_clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) 
        return false;

    cout << "Client : Connecting" << endl;

    return true;
}

void CNetClient::DoUpdate()
{
    /// ��ٸ��� �̺�Ʈ
    int index = ::WSAWaitForMultipleEvents(1, &m_clientEvent, FALSE, 1000, FALSE);

    if (index == WSA_WAIT_FAILED)
        return;

    index -= WSA_WAIT_EVENT_0;

    /// ���Ͽ� ���� �̺�Ʈ �˻�
    WSANETWORKEVENTS networkEvents;
    if (::WSAEnumNetworkEvents(m_clientSocket, m_clientEvent, &networkEvents) == SOCKET_ERROR)
    {
        cout << "EnumNetworkEvents Error" << WSAGetLastError() << endl;
        return;
    }

    /// ���ῡ ���� �̺�Ʈ, ���� ���� Ȯ��
    if (networkEvents.lNetworkEvents & FD_CONNECT)
    {
        if (networkEvents.iErrorCode[FD_CONNECT_BIT] != 0)
        {
            cout << "Connect Error " << networkEvents.iErrorCode[FD_CONNECT_BIT] << endl;
            return;
        }

        cout << "Client : Connected" << endl;

        m_isConnected = true;
    }

    /// RECIVE
    if (networkEvents.lNetworkEvents & FD_READ)
    {
        if (networkEvents.iErrorCode[FD_READ_BIT] != 0)
        {
            cout << "Read Error" << endl;
            return;
        }

        char buffer[BUFSIZE] = {};
        int recvBytes = ::recv(m_clientSocket, buffer, BUFSIZE, 0);
        if (recvBytes == SOCKET_ERROR)
        {
            cout << "Read Error" << endl;
            return;
        }

        if (recvBytes == 0)
        {
            cout << "Disconnected" << endl;
            return;
        }

        cout << "Client : Recv " << buffer << endl;
    }

    /// Send
    if (m_isConnected)
    {
        Sleep(1000);

        int sendLen = ::send(m_clientSocket, m_sendBuffer, strlen(m_sendBuffer) + 1, 0);
        if (sendLen == SOCKET_ERROR && ::WSAGetLastError() != WSAEWOULDBLOCK) {
            cout << "send Error " << ::WSAGetLastError() << endl;
        }
        else
        {
            cout << "Client : Send " << sendLen << endl;
        }
    }
}


int main()
{
    /// ���� �ʱ�ȭ
    WSAData wsaData;
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return 0;

    /// ���� ���� ���� �����
    SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == INVALID_SOCKET)
        return 0;

    /// ���� ���� ��Ʈ�� ���ε�
    SOCKADDR_IN serverAddr;
    ::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
    serverAddr.sin_port = ::htons(7777);

    if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
        return 0;

    if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
        return 0;

    cout << "Listen" << endl;

    /// �������Ͽ� �����̺�Ʈ ���
	vector<WSAEVENT> wsaEvents;
    vector<Session> sessions;
    sessions.reserve(100);

    WSAEVENT listenEvent = ::WSACreateEvent();
    wsaEvents.push_back(listenEvent);
    sessions.push_back(Session{ listenSocket });

    // �̺�Ʈ�� ����ϰ� �ش� �̺�Ʈ�� �߻��ϸ� �ε����� �˾ƿ� �� �ִ�.
    // WSAEventSelect �Լ��� listenSocket�� �ͺ�ŷ �������� �����.
    if (::WSAEventSelect(listenSocket, listenEvent, FD_ACCEPT) == SOCKET_ERROR) return 0;

    /// Ŭ���̾�Ʈ ����, ����
    //CNetClient client;
    //client.Start(true);

    //if (false == client.Connect())
    //{
    //    cout << "Connect Error " << WSAGetLastError() << endl;
    //}

    while(true)
    {
        // ��ϵ� �̺�Ʈ�� ���� �뺸�� �ޱ����� �Լ�. �̶� �ε����� � �̺�Ʈ���� �� �� ����
    	int index = ::WSAWaitForMultipleEvents(wsaEvents.size(), &wsaEvents[0], FALSE, WSA_INFINITE, FALSE);

        if (index == WSA_WAIT_FAILED)
            continue;

        // �Ŵ���, index�� �׻� WSA_WAIT_EVENT_0�� �������� �´�.
        index -= WSA_WAIT_EVENT_0;

        // WSAEventSelect�� ���Ͽ� ��ϵ� �̺�Ʈ�� �߻��ϸ�
        // � ��Ʈ��ũ �̺�Ʈ���� Ȯ���ؾ� �Ѵ�.
        WSANETWORKEVENTS networkEvents;
        if (::WSAEnumNetworkEvents(sessions[index].socket, wsaEvents[index], &networkEvents) == SOCKET_ERROR)
            return 0;

        if (networkEvents.lNetworkEvents & FD_ACCEPT)
        {
            if (networkEvents.iErrorCode[FD_ACCEPT_BIT] != 0) // FD_ACCEPT�� �´� ����
                continue;

            SOCKADDR_IN clientAddr;
            int addrLen = sizeof(clientAddr);
            SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);

            if (clientSocket != INVALID_SOCKET)
            {
                cout << "Client Connected" << endl;


                WSAEVENT clientEvent = ::WSACreateEvent();
                wsaEvents.push_back(clientEvent);
                sessions.push_back(Session{ clientSocket });


                if (::WSAEventSelect(clientSocket, clientEvent, FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR)
                    return 0;
            }
        }

        // ����� Ŭ���̾�Ʈ�� �޽����� �ϳ� ������ ������ �ݴ´�.
        // ���� �޽����� ���� ���� ���ۿ� �о���� �ٽ� �ش� Ŭ��� ����
        // ���� ���ۿ� ���� ���� �����Ͱ� ���� ������ ��� ������, ���� ���۰� ��� �۽� ������ �����͸� �о�´�.

        // ���� ���� ����(Ŀ�� �޸�)�� �о�� �����Ͱ� �ִٸ�
        if (networkEvents.lNetworkEvents & FD_READ || networkEvents.lNetworkEvents & FD_WRITE)
        {
            if ((networkEvents.lNetworkEvents & FD_READ) && networkEvents.iErrorCode[FD_READ_BIT] != 0)
                continue;


            if ((networkEvents.lNetworkEvents & FD_WRITE) && networkEvents.iErrorCode[FD_WRITE_BIT] != 0)
                continue;


            Session& s = sessions[index];

            // Receive
            if (s.recvBytes == 0)
            {
                int recvLen = ::recv(s.socket, s.recvBuffer, BUFSIZE, 0);
                if (recvLen == SOCKET_ERROR && ::WSAGetLastError() != WSAEWOULDBLOCK) {
                    continue;
                }

                if(recvLen > 0)
                {
                    s.recvBytes = recvLen;
                }
                cout << "Server : Recv Data= " << recvLen << " : " <<s.recvBuffer << endl;
            }

            // Send
            // ���� Receive ���۴� ������(Ȥ�� �ֱ��� �ʱ�ȭ)�� ����Ѵ�. �����͸� �� �� ���������� ���۸� ��� �� ���� �����̴�.
            if (s.recvBytes > s.sendBytes) {
                // �������� �����͸� ��� ������ �ʾҴٸ�, ���� �����͸� �ٽ� �������� �õ�
                int sendLen = ::send(s.socket, &s.recvBuffer[s.sendBytes], s.recvBytes - s.sendBytes, 0);
                if (sendLen == SOCKET_ERROR && ::WSAGetLastError() != WSAEWOULDBLOCK) {
                    continue;
                }

                s.sendBytes += sendLen;
                if (s.recvBytes == s.sendBytes) {
                    s.recvBytes = 0;
                    s.sendBytes = 0;
                }

                cout << "Server : Send Data= " << sendLen << endl;
            }
        }
    }

    //client.Stop();
    WSACleanup();
    return 0;
}
