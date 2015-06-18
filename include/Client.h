#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "Player.h"

class Client
{
public:
	enum DisconnectReason
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
	Client(uv_stream_t *conn, const string& addr );
	~Client();
public:
	void ReceiveData(char* buff, int size);
	void Destroy(Client::DisconnectReason reason);
private:
	void ParseHttpPack();
	void ParseNormalPack();
	void HandleHttpPack(const string& url, char* buff, int size);
	void HandleHttpPack(const string& url);
	void HandleNormalPack(int packId, char* buff, int size);
	
	bool IsHttpPack(char* buff);
public:
	uv_stream_t* mConn;
	string mAddr;
	uv_buf_t mReceiveBuff;
	uv_buf_t mDataBuff;
	int mValidSize;

	Player* mPlayer;
};

//***********************************************************
class ClientMgr
{
public:
	static map<void*, Client*> mAllClients;
public:
	static void CloseClient(uv_stream_t* conn, Client::DisconnectReason reason);
	static void SendPackToAll(uv_buf_t buff);
public:
	ClientMgr();
	~ClientMgr(){};
};
#endif//_CLIENT_H_
