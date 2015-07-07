#ifndef _GAME_H_
#define _GAME_H_

#include "httpReq.h"
#include "player.h"

class Game : public HttpReq
{
public:
	static void ValidChar(const string& session, int charId, const string& gameIp, int gamePort, Player* player);
	static void OnValidCharConn(uv_connect_t* req, int status);
	static void OnValidCharReturn(uv_stream_t *conn, ssize_t nread, uv_buf_t recvBuff);
public:
	Game();
	~Game(){};
public:
};
#endif//_GAME_H_

