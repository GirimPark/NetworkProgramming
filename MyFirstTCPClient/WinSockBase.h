#pragma once
class WinSockBase
{
    /// ���뼺
// ���� API
// ���� ���� Create
// �ּ� ���ε�
// ���� Connect
// ����
// �ݱ� closeSocket : ����Ʈ �ɼ����� �ݴ´�.

// Attach, Dettach�� �ʿ��� ���� : ���� Ǯ�� ���鶧, ���� �ڵ� ���� ���̹Ƿ� �̸� ����
// Accept�� ����� ���� �ڵ��� �ް�, ���� ��ü Ǯ���� �ϳ� ������ ���ε��Ѵ�.
// ������ ������ �ش� ���� �ڵ��� ���̻� ������� ������, ���� ��ü�� �ٽ� ����ؾߵǹǷ� Detach �� Ǯ�� �����ش�.

// ���� ���� : WinSocket���� ��ü�� ������ �̸� ����� �� �� �ִ�. ���� ��ü Ǯ, ���� ���� ��

/// ������
    virtual void OnAccept(int nErrorCode) = 0;
    virtual void OnClose(int nErrorCode) = 0;
    virtual void OnReceive(int nErrorCode) = 0;
    virtual void OnSend(int nErrorCode) = 0;
};

bool WinsockInit();
void WinsockCleanup();
