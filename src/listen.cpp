#include "comm.h"
#include "listen.h"
#include "log.h"
#include "poll.h"
#include "conn.h"

Listen::Listen(int sock)
{
	Sock(sock);
}
Listen::~Listen()
{
}

void Listen::OnRead()
{
	sockaddr_in sin;
	int sock = accept(mSock, (struct sockaddr*)&sin, sizeof(sockaddr_in))
	if(sock == -1)
	{
		Log::Error("Error when accept connection: %d.\n", errno);
		return;
	}
	Poll* pPoll = Poll::Singleton();
	
	Conn* pConn = new Conn(sock);
	pPoll->Add(sock, pConn);
	pPoll->SetWrite(sock, pConn);
	//printf("new conn[%s:%d]", inet_ntoa(sin.sin_addr),ntohs(sin.sin_port));
}
void Listen::OnWrite()
{
}
