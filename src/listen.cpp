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
	while(true)
	{
		int sock = accept(mSock, (sockaddr*)&sin, &len);
		if(sock <= 0)
		{
			break;
		}

		Poll* pPoll = Poll::Singleton();
		Conn* pConn = new Conn(sock);
		ConnMgr::mAllConns[sock] = pConn;
		if(!pPoll->Add(sock, pConn))
		{
			ConnMgr::mAllConns.erase(sock);;
			DELETE(pConn);
			close(sock);
			continue;
		}
		pPoll->SetWrite(sock, pConn);
	}
}
void Listen::OnWrite()
{
}
