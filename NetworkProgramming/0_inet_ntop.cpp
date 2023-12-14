#include <stdio.h>
#include <winsock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

void Test_inet_ntop_IPv4()
{
    struct sockaddr_in sa;
    char ipv4AddressAsString[INET_ADDRSTRLEN];

    // 변환할 이진 형식의 IPv4 주소 (예: "192.0.2.1") 255.255.255.255
    inet_pton(AF_INET, "192.0.2.1", &(sa.sin_addr));

    // 이진 형식의 주소를 텍스트 형식으로 변환
    inet_ntop(AF_INET, &(sa.sin_addr), ipv4AddressAsString, INET_ADDRSTRLEN);

    printf("IPv4 Address: %s\n", ipv4AddressAsString);
}

void Test_inet_ntop_IPv6()
{
    struct sockaddr_in6 sa;
    char ipv6AddressAsString[INET6_ADDRSTRLEN];

    // 변환할 이진 형식의 IPv6 주소 (예: "2001:0db8:85a3:0000:0000:8a2e:0370:7334")
    inet_pton(AF_INET6, "2001:0db8:85a3:0000:0000:8a2e:0370:7334", &(sa.sin6_addr));

    // 이진 형식의 주소를 텍스트 형식으로 변환
    inet_ntop(AF_INET6, &(sa.sin6_addr), ipv6AddressAsString, INET6_ADDRSTRLEN);
    printf("IPv6 Address: %s\n", ipv6AddressAsString);
}

int main()
{
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        printf("WSAStartup failed: %d\n", result);
        return 1;
    }

    Test_inet_ntop_IPv4();
    Test_inet_ntop_IPv6();

    WSACleanup();
    return 0;
}
