// 넌블러킹 소켓
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
    int sendBytes = 0; // 받은만큼 돌려 주는 인과응보 에코 서버
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
    /// 소켓, 이벤트 생성
    m_clientSocket = ::socket(AF_INET, SOCK_STREAM, 0);

    if (m_clientSocket == INVALID_SOCKET) return false;

    m_clientEvent = ::WSACreateEvent();

    // 클라이언트 소켓 핸들에 FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE 4가지의 네트워크 신호가 켜지면
    // m_clinetEvnet 이벤트 객체를 통해 알 수 있도록 연결한다.
    // m_clinetSocket을 넌블러킹 모드로 바꿔준다.
    if (::WSAEventSelect(m_clientSocket, m_clientEvent, FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR) 
        return 0;

    /// 클라이언트 소켓을 서버 주소에 연결한다.
    SOCKADDR_IN serverAddr;
    ::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = ::htons(7777);
    inet_pton(serverAddr.sin_family, "127.0.0.1", &(serverAddr.sin_addr)); // 다른 주소로 바꿔서 테스트 해보세요.

    // m_clinetSocket이 블러킹 소켓이라면 접속할 수 있을 때까지 블러킹 된다.
    // 그러나 넌블러킹 소켓이므로 이벤트 객체를 통해 FD_CONNECT 네트워크 이벤트가 발생하여 해당 함수는 블럭되지 않는다.
    if (::connect(m_clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) 
        return false;

    cout << "Client : Connecting" << endl;

    return true;
}

void CNetClient::DoUpdate()
{
    /// 기다리는 이벤트
    int index = ::WSAWaitForMultipleEvents(1, &m_clientEvent, FALSE, 1000, FALSE);

    if (index == WSA_WAIT_FAILED)
        return;

    index -= WSA_WAIT_EVENT_0;

    /// 소켓에 대한 이벤트 검색
    WSANETWORKEVENTS networkEvents;
    if (::WSAEnumNetworkEvents(m_clientSocket, m_clientEvent, &networkEvents) == SOCKET_ERROR)
    {
        cout << "EnumNetworkEvents Error" << WSAGetLastError() << endl;
        return;
    }

    /// 연결에 대한 이벤트, 연결 여부 확인
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
    /// 윈속 초기화
    WSAData wsaData;
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return 0;

    /// 서버 리슨 소켓 만들기
    SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == INVALID_SOCKET)
        return 0;

    /// 리슨 소켓 포트에 바인딩
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

    /// 리슨소켓에 리슨이벤트 등록
	vector<WSAEVENT> wsaEvents;
    vector<Session> sessions;
    sessions.reserve(100);

    WSAEVENT listenEvent = ::WSACreateEvent();
    wsaEvents.push_back(listenEvent);
    sessions.push_back(Session{ listenSocket });

    // 이벤트를 등록하고 해당 이벤트가 발생하면 인덱스를 알아올 수 있다.
    // WSAEventSelect 함수는 listenSocket을 넌블러킹 소켓으로 만든다.
    if (::WSAEventSelect(listenSocket, listenEvent, FD_ACCEPT) == SOCKET_ERROR) return 0;

    /// 클라이언트 생성, 연결
    //CNetClient client;
    //client.Start(true);

    //if (false == client.Connect())
    //{
    //    cout << "Connect Error " << WSAGetLastError() << endl;
    //}

    while(true)
    {
        // 등록된 이벤트에 대해 통보를 받기위한 함수. 이때 인덱스로 어떤 이벤트인지 알 수 있음
    	int index = ::WSAWaitForMultipleEvents(wsaEvents.size(), &wsaEvents[0], FALSE, WSA_INFINITE, FALSE);

        if (index == WSA_WAIT_FAILED)
            continue;

        // 매뉴얼, index에 항상 WSA_WAIT_EVENT_0가 더해져서 온다.
        index -= WSA_WAIT_EVENT_0;

        // WSAEventSelect로 소켓에 등록된 이벤트가 발생하면
        // 어떤 네트워크 이벤트인지 확인해야 한다.
        WSANETWORKEVENTS networkEvents;
        if (::WSAEnumNetworkEvents(sessions[index].socket, wsaEvents[index], &networkEvents) == SOCKET_ERROR)
            return 0;

        if (networkEvents.lNetworkEvents & FD_ACCEPT)
        {
            if (networkEvents.iErrorCode[FD_ACCEPT_BIT] != 0) // FD_ACCEPT에 맞는 에러
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

        // 연결된 클라이언트로 메시지를 하나 보내고 소켓을 닫는다.
        // 받은 메시지를 유저 수신 버퍼에 읽어오고 다시 해당 클라로 전송
        // 수신 버퍼에 돌려 보낼 데이터가 없을 때까지 계속 보내고, 수신 버퍼가 비면 송신 버퍼의 데이터를 읽어온다.

        // 소켓 수신 버퍼(커널 메모리)에 읽어올 데이터가 있다면
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
            // 실제 Receive 버퍼는 링버퍼(혹은 주기적 초기화)를 사용한다. 데이터를 한 번 읽을때마다 버퍼를 비울 수 없기 때문이다.
            if (s.recvBytes > s.sendBytes) {
                // 보내야할 데이터를 모두 보내지 않았다면, 남은 데이터를 다시 보내도록 시도
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
