#include <iostream>
#include "WinSockBase.h"

// CAsyncSocket을 참고해서 클래스로 만들기
// 해당 클래스를 사용해서 서버에 연결하는 코드 작성
// eventSelect 예제 참고
// eventSelect 서버에 연결 확인
class WinSockClient : public WinSockBase
{
public:

};

// 소켓 클래스는 네트워크 작업을 위한 래퍼
// 데이터그램이나 스트림에 상관없이 다 제공해주고 있어야 함
// 패킷 종류에 따라 관리해야할 패킷 컨테이너가 달라집니다
// 스트림이라면 버퍼에 계속 읽은 데이터를 누적하고
// 데이터그램이라면 각 패킷은 분리되어 저장됩니다.

int main()
{
    // 윈속 초기화하고

    WinSockClient client; // 연결을 위한 개체를 만들어서


}
