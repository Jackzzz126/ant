#include "comm.h"
#include "Room.h"

#include "Player.h"

Room::Room(int id, Player* player1, Player* player2)
{
}

void Room::BroadCastGotMatch()
{
	//vector<Player*> mPlayers;
}


//*********************************************************
vector<Player*> RoomMgr::mPlayersWaiting;
map<int, Room*> RoomMgr::mAllRooms;
int RoomMgr::mRoomIdGenerator = 1;

RoomMgr::RoomMgr()
{
}

void RoomMgr::JoinRoom(Player* player)
{
	RoomMgr::mPlayersWaiting.push_back(player);
	if(RoomMgr::mPlayersWaiting.size() > 2)
	{
		int roomId = RoomMgr::GenerateNewRoomId();
		Room* room = new Room(roomId, RoomMgr::mPlayersWaiting[0], RoomMgr::mPlayersWaiting[1]);
		room->BroadCastGotMatch();
		RoomMgr::mAllRooms[roomId] = room;
		vector<Player*>::iterator iter = RoomMgr::mPlayersWaiting.begin();
		RoomMgr::mPlayersWaiting.erase(iter, iter + 2);
	}

}

int RoomMgr::GenerateNewRoomId()
{
	if(mRoomIdGenerator == 0x7fffffff)
		mRoomIdGenerator = 1;
	return mRoomIdGenerator++;
}
