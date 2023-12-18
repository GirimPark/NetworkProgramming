#include "pch.h"
#include "WinSockBase.h"

WinSockBase::~WinSockBase()
{
	Finalize();
}

HRESULT WinSockBase::Update()
{
	HRESULT rt;

	rt = Receive();
	if (rt != S_OK)
		return rt;
	
	rt = Send();
	if (rt != S_OK)
		return rt;

	return S_OK;
}

void WinSockBase::Finalize()
{
	closesocket(m_socket);
}
