#pragma once

#include <winsock2.h>
#include <string>

namespace netfish
{
	// 비동기 소켓 클래스
	class AsyncSocket
	{
	public:
		AsyncSocket();
		virtual ~AsyncSocket();

		// optional
		virtual bool OnAccept(AsyncSocket* pSocket) { return false; }
		virtual bool OnConnect() { return false; }
		virtual bool OnClose() { return false; }
		virtual bool OnSend() { return false; }
		virtual bool OnReceive() { return false; }


		bool Create(
			int nSocketType = SOCK_STREAM,
			long lEvent = FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT | FD_CONNECT | FD_CLOSE
		);

		bool Bind(int port, const char* lpszSocketAddress = nullptr);

		bool Listen(int backlog = SOMAXCONN);

		bool Accept(AsyncSocket& acceptSocket);

		bool Connect(const char* ip, int port);

		void Close();

		int Send(const char* buf, int len);

		int Recv(char* buf, int len);

		int SendTo(const char* buf, int len, const char* ip, int port);

		int RecvFrom(char* buf, int len, char* ip, int* port);

		SOCKET GetHandle() { return m_hSocket; }
		WSAEVENT GetEvent() { return m_hEvent; }

		void SetEvent(WSAEVENT hEvent) { m_hEvent = hEvent; }

		void Attach(SOCKET hSocket) { m_hSocket = hSocket; }
		void Detach() { m_hSocket = INVALID_SOCKET; }

		const std::string& GetIP() { return m_ip; }
		int GetPort() { return m_port; }

		int GetLastError() { return m_error; }

		static bool InitSock();

		static void CleanupSock();

	private:
		SOCKET		m_hSocket = INVALID_SOCKET;
		WSAEVENT	m_hEvent = WSA_INVALID_EVENT;

		std::string m_ip;
		int			m_port = 0;

		int m_error;
	};


}
