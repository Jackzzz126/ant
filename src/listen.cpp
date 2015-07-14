#include "comm.h"
#include "listen.h"
#include "log.h"
#include "poll.h"
#include "conn.h"

Listen::Listen(int sock) : Sock(sock)
{
}
Listen::~Listen()
{
}

void Listen::OnRead()
{
	sockaddr_in sin;
	socklen_t len = sizeof(sockaddr_in);
	int sock = accept(mSock, (struct sockaddr*)&sin, &len);
	if(sock == -1)
	{
		Log::Error("Error when accept connection: %d.\n", errno);
		return;
	}

	Poll* pPoll = Poll::Singleton();
	Conn* pConn = new Conn(sock);
	pPoll->Add(sock, pConn);
	pPoll->SetWrite(sock, pConn);

	ConnMgr::mAllConns[sock] = pConn;
}
void Listen::OnWrite()
{
}
