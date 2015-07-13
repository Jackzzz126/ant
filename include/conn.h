#ifndef _CONN_H_
#define _CONN_H_

#include "packId.h"
#include "sock.h"

struct RefBuff
{
	char* mBuff;
	int mLen;
	int mRef;
	RefBuff(int len, int ref)
	{
		mBuff = NewBuff(len);
		mLen = len;
		mRef = ref;
	};
	~RefBuff()
	{
		DelBuff(&mBuff);
	};
};
class Conn : Sock
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
	void OnRead();
	void OnWrite();
	void Write(RefBuff* refBuff);
	void Destroy(bool logErr);
private:
	void ParseHttpPack();
	void ParseNormalPack();
	void HandleHttpPost(const string& url, char* buff, int size);
	void HandleHttpGet(const string& url);
	void HandleNormalPack(int packId, char* buff, int size);
	
	bool IsHttpPack(char* buff);
	void ExpandRecvBuff();
	void ShrinkRecvBuff();
public:
	char* mRecvBuff;//buff to recv data
	int mRecvLen;
	int mValidSize;
	SendBuffNode* mSendBuffHead;
	SendBuffNode* mSendBuffTail;
};

//***********************************************************
class ConnMgr
{
public:
	static map<int, Conn*> mAllConns;
public:
	static void CloseConn(void* conn, bool logErr);
	static void SendToAll(PackId::PackIdType packId, RefBuff* pRefBuff);
	static void SendToOne(void* conn, PackId::PackIdType packId, RefBuff* pRefBuff);
	static void SendToMulti(const vector<void*>& conns, PackId::PackIdType packId, RefBuff* pRefBuff);
public:
	ConnMgr();
	~ConnMgr(){};
};
#endif//_CONN_H_
