#ifndef _CONN_H_
#define _CONN_H_

#include "packId.h"
#include "sock.h"

class Conn : public Sock
{
	struct SendBuffNode
	{
		RefBuff* mRefBuff;
		int mOffset;
		SendBuffNode* mNext;
		SendBuffNode(RefBuff* refBuff)
		{
			mRefBuff = refBuff;
			mOffset = 0;
			mNext = NULL;
		};
	};
public:
	Conn(int sock);
	~Conn();
public:
	void OnRead(int timeStamp);
	void OnWrite(int timeStamp);
	void Write(RefBuff* refBuff);
	void Close(bool logErr);
private:
	void ParseHttpPack();
	void ParseNormalPack();

	void HandleNormalPack(int packId, char* buff, int size);
	void HandleHttpPack(const string& url, char* buff, int size);

	bool IsHttpPack(char* buff);
	void ExpandRecvBuff();
	void ShrinkRecvBuff();
private:
	char* mRecvBuff;//buff to recv data
	int mRecvLen;
	int mValidSize;
	SendBuffNode* mSendBuffHead;
	SendBuffNode* mSendBuffTail;

	pthread_mutex_t mMutex;
};

//***********************************************************
class ConnMgr
{
public:
	static map<int, Conn*> mAllConns;
public:
	static void CloseConn(int sock, bool logErr);
	static void SendToAll(RefBuff* pRefBuff);
	static void SendToOne(int sock, RefBuff* pRefBuff);
	static void SendToMulti(const vector<int>& socks, RefBuff* pRefBuff);
public:
	ConnMgr();
	~ConnMgr(){};
};
#endif//_CONN_H_
