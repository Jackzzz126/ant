#include "comm.h"
#include "udpListen.h"
#include "poll.h"
#include "conn.h"
#include "config.h"

UdpListen::UdpListen(int sock) : Sock(sock)
{
}
UdpListen::~UdpListen()
{
}

void UdpListen::OnRead(int timeStamp)
{
	struct sockaddr_in addr;
	int addrLen = sizeof(addr);
	char buff[BUFF_UNIT];
	int len = recvfrom(mSock, buff, BUFF_UNIT, 0, (sockaddr*)&addr, (socklen_t*)&addrLen);
	sendto(mSock, buff, len, 0, (const sockaddr*)&addr, addrLen);
}
void UdpListen::OnWrite(int timeStamp)
{
}
int UdpListen::Init()
{
	Config* pConfig = Config::Singleton();

	sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(pConfig->mIp.c_str());
	sin.sin_port = htons(pConfig->mUdpPort);
	return bind(mSock, (const sockaddr*)&sin, sizeof(sin));
}
