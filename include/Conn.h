#ifndef _CONN_H_
#define _CONN_H_

class BuffList
{
	char* buff;
	int len;
};
class Conn
{
public:
	enum DisconnReason
	{
		UNKNOWN_DATA,
		CLIENT_CLOSE,//close coon by client

		SERVER_CLOSE,//close conn by server
		SERVER_DOWN,//shutdown server

		SOCK_ERROR,
		VALID_FAIL,
	};
public:
	static int BUFF_UNIT;
	static int HEAD_LENGTH;
public:
	Conn(uv_stream_t *conn, const string& addr );
	~Conn();
public:
	void RecvData(char* buff, int size);
	void Destroy(Conn::DisconnReason reason);
private:
	void ParseHttpPack();
	void ParseNormalPack();
	void HandleHttpPack(const string& url, char* buff, int size);
	void HandleHttpPack(const string& url);
	void HandleNormalPack(int packId, char* buff, int size);
	
	bool IsHttpPack(char* buff);
	void ExpandRecvBuff();
	void ShrinkRecvBuff();
public:
	uv_stream_t* mConn;
	string mAddr;
	uv_buf_t mRecvBuff;//buff to recv data
	int mValidSize;
};

//***********************************************************
class ConnMgr
{
public:
	static map<void*, Conn*> mAllConns;
public:
	static void CloseConn(uv_stream_t* conn, Conn::DisconnReason reason);
	static void SendPackToAll(uv_buf_t buff);
public:
	ConnMgr();
	~ConnMgr(){};
};
#endif//_CONN_H_
