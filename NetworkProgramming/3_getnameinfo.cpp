#include <stdio.h>
#include <winsock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

int main()
{
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        printf("WSAStartup failed: %d\n", result);
        return 1;
    }

    struct sockaddr_in sa;
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    // sockaddr_in 구조체 설정
    sa.sin_family = AF_INET; // IPv4 주소 체계

    inet_pton(AF_INET, "172.217.26.238", &(sa.sin_addr));

    sa.sin_port = htons(80); // HTTP 포트

    // getnameinfo 호출
    result = getnameinfo((struct sockaddr*)&sa, sizeof(sa),
        host, NI_MAXHOST,
        service, NI_MAXSERV, 0);

    if (result != 0) {
        printf("getnameinfo failed: %d\n", result);
    }
    else {
        printf("Host: %s, Service: %s\n", host, service);
    }

    WSACleanup();
	return 0;
}