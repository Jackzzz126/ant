#ifndef _UDPCLIENT_H_
#define _UDPCLIENT_H_

class UdpClient
{
public:
	UdpClient(sockaddr_in addr, int timeStamp);
	~UdpClient();
public:
	struct sockaddr_in mAddr;
private:
	int mLastActiveTime;
};

//***********************************************************
class UdpClientMgr
{
private:
	static map<int, UdpClient*> mCharUdpClient;
public:
	static void AddUdpClient(int charId, UdpClient* pUdpClient);
	static void DelUdpClient(int charId);

	static void SendToChar(int charId, char* buff, int len);
	static void SendToChars(const vector<int>& charIds, char* buff, int len);
};
#endif//_UDPCLIENT_H_
