#ifndef _UDP_H_
#define _UDP_H_

class UdpClient
{
public:
	UdpClient();
	~UdpClient();
public:
	struct sockaddr_in mAddr;
private:
	int mLastActiveTime;
};

//***********************************************************
class UdpMgr
{
private:
	static map<int, UdpClient*> mCharUdpClient;
public:
	static int mUdpSock;
public:
	static void AddUdpClient(int charId, UdpClient* pUdpClient);
	static void DelUdpClient(int charId);

	static void SendToChar(int charId, char* buff, int len);
	static void SendToChars(const vector<int>& charIds, char* buff, int len);
};
#endif//_UDP_H_
