#ifndef _PLAYER_H_
#define _PLAYER_H_

class Player
{
public:
	Player(uv_stream_t *conn);
	~Player(){};

	void OnValidChar(bool success);
public:
	uv_stream_t* mConn;
	string mSession;
	int mCharId;
};

//*****************************************
class PlayerMgr
{
public:
	static map<void*, Player*> mAllPlayers;
public:
	static void AddPlayer(uv_connect_t* conn, Player* player);
	static Player* GetPlayer(uv_connect_t* conn);
	static void ResetPlayer(uv_connect_t* conn, uv_stream_t* stream);
	static Player* GetPlayer(uv_stream_t* stream);
	static void DelPlayer(uv_stream_t* stream);
	static void DelPlayer(uv_connect_t* conn);
public:
	PlayerMgr();
	~PlayerMgr(){};
public:
};
#endif//_PLAYER_H_

