#include "comm.h"
#include "player.h"
#include "conn.h"

//Player::Player(uv_stream_t *conn)
//{
//	mConn = conn;
//}
//void Player::OnValidChar(bool success)
//{
//	if(success)
//	{
//	}
//	else
//	{
//		//ConnMgr::CloseConn(mConn, Conn::VALID_FAIL);
//	}
//}
//
////***************************************************
//map<void*, Player*> PlayerMgr::mAllPlayers;
//
//PlayerMgr::PlayerMgr()
//{
//}
//void PlayerMgr::AddPlayer(uv_connect_t* conn, Player* player)
//{
//	PlayerMgr::mAllPlayers[conn] = player;
//}
//Player* PlayerMgr::GetPlayer(uv_connect_t* conn)
//{
//	map<void*, Player*>::iterator iter = PlayerMgr::mAllPlayers.find(conn);
//	if(iter == PlayerMgr::mAllPlayers.end())
//	{
//		return NULL;
//	}
//	else
//	{
//		return iter->second;
//	}
//}
//void PlayerMgr::ResetPlayer(uv_connect_t* conn, uv_stream_t* stream)
//{
//	PlayerMgr::mAllPlayers[stream] = PlayerMgr::mAllPlayers[conn];
//	PlayerMgr::mAllPlayers.erase(conn);
//}
//Player* PlayerMgr::GetPlayer(uv_stream_t* stream)
//{
//	map<void*, Player*>::iterator iter = PlayerMgr::mAllPlayers.find(stream);
//	if(iter == PlayerMgr::mAllPlayers.end())
//	{
//		return NULL;
//	}
//	else
//	{
//		return iter->second;
//	}
//}
//void PlayerMgr::DelPlayer(uv_stream_t* stream)
//{
//	PlayerMgr::mAllPlayers.erase(stream);
//}
//void PlayerMgr::DelPlayer(uv_connect_t* conn)
//{
//	PlayerMgr::mAllPlayers.erase(conn);
//}

