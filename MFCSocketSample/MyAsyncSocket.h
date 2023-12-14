#pragma once
#include <afxsock.h>
class CMyAsyncSocket :
    public CAsyncSocket
{
public:
    virtual void OnAccept(int nErrorCode);
    virtual void OnClose(int nErrorCode);
    virtual void OnReceive(int nErrorCode);
    virtual void OnSend(int nErrorCode);
};

