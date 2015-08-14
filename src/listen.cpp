#include "comm.h"
#include "listen.h"
#include "poll.h"
#include "conn.h"
#include "config.h"

Listen::Listen(int sock) : Sock(sock)
{
}
Listen::~Listen()
{
}

void Listen::OnRead(int timeStamp)
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
		ConnMgr::AddConn(sock, pConn);
		if(!pPoll->Add(sock, pConn))
		{
			ConnMgr::CloseConn(sock, false);
			continue;
		}
		pPoll->SetWrite(sock, pConn);
	}
}
void Listen::OnWrite(int timeStamp)
{
}
int Listen::Init()
{
	Config* pConfig = Config::Singleton();

	sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(pConfig->mIp.c_str());
	sin.sin_port = htons(pConfig->mPort);
	bind(mSock, (const sockaddr*)&sin, sizeof(sin));
	return listen(mSock, pConfig->mBacklog);
}
