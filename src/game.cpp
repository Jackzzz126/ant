#include "comm.h"
#include "game.h"
#include "log.h"
#include "player.h"
#include "conn.h"

//#include "ReqValidCharacter.h"
//#include "ResValidCharacter.h"

//extern uv_loop_t *loop;
//extern vector<HttpConn> gHttpConns;
//
//Game::Game()
//{
//}
//
//void Game::ValidChar(const string& session, int charId, const string& gameIp, int gamePort, Player* player)
//{
//	//HttpConn conn;
//	//conn.mSocket = new uv_tcp_t;
//	//uv_tcp_init(loop, conn.mSocket);
//
//	//conn.mConn = new uv_connect_t;
//	//struct sockaddr_in server_addr;
//	//uv_ip4_addr(gameIp.c_str(), gamePort, &server_addr);
//	//uv_tcp_connect(conn.mConn, conn.mSocket, (sockaddr*)&server_addr, OnValidCharConn);
//	//gHttpConns.push_back(conn);
//	//PlayerMgr::AddPlayer(conn.mConn,player);
//}
//
//void Game::OnValidCharConn(uv_connect_t* conn, int status)
//{
//	//if(status == -1)
//	//{
//	//	Log::Out("connect to game server faile.\n");
//	//	ClearConn(conn->handle);
//	//	PlayerMgr::DelPlayer(conn);
//	//}
//	//else
//	//{
//	//	PlayerMgr::ResetPlayer(conn, conn->handle);
//	//	uv_read_start(conn->handle, OnAllocBuffer, OnValidCharReturn);
//
//	//	ReqValidCharacter req;
//	//	req.m_session = PlayerMgr::GetPlayer(conn->handle)->mSession;
//	//	req.m_charId = PlayerMgr::GetPlayer(conn->handle)->mCharId;
//
//	//	uv_buf_t sendBuff;
//	//	req.Write(sendBuff);
//
//	//	SendHttpPack(conn, "/character/valid", sendBuff);
//	//}
//}
//
//void Game::OnValidCharReturn(uv_stream_t *handle, ssize_t nread, uv_buf_t recvBuff)
//{
//	//if (nread == -1)
//	//{
//	//	if(uv_last_error(loop).code != UV_EOF)
//	//	{
//	//		Log::Error("error when valid char:%s.\n", uv_err_name(uv_last_error(loop)));
//	//		uv_close((uv_handle_t*)handle, NULL);
//	//		ClearConn(handle);
//	//		PlayerMgr::DelPlayer(handle);
//	//		ClientMgr::CloseClient(PlayerMgr::GetPlayer(handle)->mConn, Client::SOCK_ERROR);
//	//	}
//	//	else
//	//	{
//	//		Log::Error("Error when reg combat server:lost conn.\n");
//	//		ClearConn(handle);
//	//		PlayerMgr::DelPlayer(handle);
//	//		ClientMgr::CloseClient(PlayerMgr::GetPlayer(handle)->mConn, Client::SOCK_ERROR);
//	//	}
//	//	return;
//	//}
//
//	//string httpHead = string(recvBuff.base);
//	//uint httpHeadPos = httpHead.find("\r\n\r\n") + 4;
//	//httpHead = httpHead.substr(0, httpHeadPos);
//	//if(httpHead.substr(0, 12) != "HTTP/1.1 200")
//	//{
//	//	Log::Out("error when valid char:500.\n");
//	//	DelBuff(&recvBuff.base);
//	//	PlayerMgr::GetPlayer(handle)->OnValidChar(false);
//	//	PlayerMgr::DelPlayer(handle);
//	//}
//	//else
//	//{
//	//	ResValidCharacter res;
//	//	char* data = (char*)recvBuff.base + httpHeadPos;
//	//	res.InitBuff(data, *((int*)data + 1) ^ 0x79669966);
//	//	if(!res.Parse())
//	//	{
//	//		Log::Out("unkown return when valid char.\n");
//	//		PlayerMgr::GetPlayer(handle)->OnValidChar(false);
//	//		PlayerMgr::DelPlayer(handle);
//	//	}
//	//	else
//	//	{
//	//		if(res.m_status < 0)
//	//		{
//	//			Log::Out("error when valid char:%d\n");
//	//			PlayerMgr::GetPlayer(handle)->OnValidChar(false);
//	//			PlayerMgr::DelPlayer(handle);
//	//		}
//	//		else
//	//		{
//	//			PlayerMgr::GetPlayer(handle)->OnValidChar(true);
//	//			PlayerMgr::DelPlayer(handle);
//	//		}
//	//	}
//	//	DelBuff(&recvBuff.base);
//	//}
//}


