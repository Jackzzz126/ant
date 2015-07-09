#ifndef _CONN_H_
#define _CONN_H_

#include "packId.h"

class Conn
{
public:
	Conn(void *conn);
	~Conn();
public:
	void RecvData(char* buff, int size);
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
	void* mConn;
	uv_buf_t mRecvBuff;//buff to recv data
	int mValidSize;
};

//***********************************************************
class ConnMgr
{
public:
	static map<void*, Conn*> mAllConns;
public:
	static void CloseConn(void* conn, bool logErr);
	static void SendToAll(PackId::PackIdType packId, uv_buf_t buff);
	static void SendToOne(void* conn, PackId::PackIdType packId, uv_buf_t buff);
	static void SendToMulti(const vector<void*>& conns, PackId::PackIdType packId, uv_buf_t buff);
public:
	ConnMgr();
	~ConnMgr(){};
};
#endif//_CONN_H_
