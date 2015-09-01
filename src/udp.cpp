#include "comm.h"
#include "udp.h"

UdpClient::UdpClient()
{
}
UdpClient::~UdpClient()
{
}

//************************************************
map<int, UdpClient*> UdpClientMgr::mCharUdpClient;
int UdpClientMgr::mUdpSock = 0;

void UdpClientMgr::AddUdpClient(int charId, UdpClient* pUdpClient)
{
	mCharUdpClient[charId] = pUdpClient;
}

void UdpClientMgr::DelUdpClient(int charId)
{
	mCharUdpClient.erase(charId);
}

void UdpClientMgr::SendToChar(int charId, char* buff, int len)
{
	map<int, UdpClient*>::iterator iter = mCharUdpClient.find(charId);
	if(iter != mCharUdpClient.end())
	{
		sendto(mUdpSock, buff, len, 0, (const sockaddr*)&iter->second->mAddr,
			sizeof(sockaddr_in));
	}
	DelBuff(&buff);
}
void UdpClientMgr::SendToChars(const vector<int>& charIds, char* buff, int len)
{
	vector<int>::const_iterator iter = charIds.begin();
	for(; iter != charIds.end(); iter++)
	{
		map<int, UdpClient*>::iterator udpIter = mCharUdpClient.find(*iter);
		if(udpIter != mCharUdpClient.end())
		{
			sendto(mUdpSock, buff, len, 0, (const sockaddr*)&udpIter->second->mAddr,
				sizeof(sockaddr_in));
		}
	}
	DelBuff(&buff);
}


