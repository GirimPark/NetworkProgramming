#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

int main()
{
    WSADATA wsaData;
    SOCKET listeningSocket, clientSocket;
    struct sockaddr_in server, client;

    const char* message = "como estas";

    // 1. 윈속을 초기화한다.
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Failed to initialize Winsock. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    // 2. 소켓을 생성한다.
    if ((listeningSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(7777);

    // 3. 소켓을 bind한다.
    if (bind(listeningSocket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    // 4. 소켓을 listen 상태로 만든다.
    listen(listeningSocket, 3);


    fd_set readfds;
    int clientLen = sizeof(struct sockaddr_in);

    while (1)
    {
        printf("Waiting for connections...\n");

        FD_ZERO(&readfds);

        FD_SET(listeningSocket, &readfds);

        // 6. 소켓에 연결된 클라이언트가 있는지 확인한다.
        if (select(listeningSocket + 1, &readfds, NULL, NULL, NULL) == SOCKET_ERROR) {
            printf("select function failed. Error Code: %d\n", WSAGetLastError());
            return 1;
        }


        if (FD_ISSET(listeningSocket, &readfds)) {
            if ((clientSocket = accept(listeningSocket, (struct sockaddr*)&client, &clientLen)) == INVALID_SOCKET) {
                printf("accept failed. Error Code: %d\n", WSAGetLastError());
                return 1;
            }

            printf("Connection accepted\n");

            // 접속한 클라이언트에 메시지를 송신하고 소켓을 닫는다.
            send(clientSocket, message, strlen(message), 0);
            closesocket(clientSocket);
        }
    }

    // 7. listen 소켓을 닫는다.
    closesocket(listeningSocket);

    // 8. 윈속을 종료한다.
    WSACleanup();

    return 0;
}