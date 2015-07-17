#include "comm.h"
#include "scheduleTasks.h"
#include "conn.h"
#include "config.h"

namespace ScheduleTasks
{

void CloseDeadSock()
{
	vector<int> socksToDel;
	int timeStampNow = DateTime::GetTimeStamp();
	Config* pConfig = Config::Singleton();

	map<int, Conn*>::iterator iter = ConnMgr::mAllConns.begin();
	for(; iter != ConnMgr::mAllConns.end(); iter++)
	{
		Conn* pConn = iter->second;
		if((timeStampNow - pConn->mLastActiveTime) > pConfig->mSockTimeout)
		{
			socksToDel.push_back(pConn->mSock);
			pConn->Close(false);
		}
	}

	vector<int>::iterator sockIter = socksToDel.begin();
	for(; sockIter != socksToDel.end(); sockIter++)
	{
		ConnMgr::mAllConns.erase(*sockIter);
	}
	if(socksToDel.size() > 0)
	{
		Log::Error("Find %d timeout socks.\n", socksToDel.size());
	}
}

}
