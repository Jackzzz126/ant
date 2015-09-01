#include "comm.h"
#include "udpListen.h"
#include "poll.h"
#include "conn.h"
#include "config.h"
#include "udpClient.h"
#include "msg.h"

#include "benchMark.pb.h"

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

	int recvLen = 0;
	while(true)
	{
		recvLen = recvfrom(mSock, buff, BUFF_UNIT, 0, (sockaddr*)&addr, (socklen_t*)&addrLen);
		if(recvLen == -1 || recvLen == 0)
			break;
		if(recvLen < HEAD_LENGTH)
			continue;

		int packId = *((int*)buff) ^ 0x79669966;
		int packLen = *((int*)(buff) + 1) ^ 0x79669966;
		if((packLen + HEAD_LENGTH) != recvLen)
		{
			continue;
		}
		if(packId == -21)//udp login
		{
			BenchMark::ReqUdpLogin req;
			BenchMark::ResUdpLogin res;

			req.ParseFromArray(buff + HEAD_LENGTH, packLen);
			UdpClient* pUdpClient = new UdpClient(addr, DateTime::GetTimeStamp());
			UdpClientMgr::AddUdpClient(req.charid(), pUdpClient);

			res.set_status(STATUS_SUCCESS);
			int resPackLen = res.ByteSize();
			PackId::WritePackHead(buff, PackId::BENCHMARK_UDPMOVE, resPackLen);
			res.SerializeToArray(buff + HEAD_LENGTH, resPackLen);
			sendto(mSock, buff, resPackLen + HEAD_LENGTH, 0, (const sockaddr*)&addr, addrLen);
		}
		else
		{
			char* data = NewBuff(packLen);
			memcpy(data, buff + HEAD_LENGTH, packLen);
			MsgQueue::Singleton()->PushMsg(mSock, packId, data, packLen);
		}
	}
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
