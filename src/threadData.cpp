#include "comm.h"
#include "threadData.h"
#include "config.h"

pthread_key_t ThreadData::mThreadKey;

ThreadData::ThreadData()
{
	mRedis = NULL;
}
ThreadData::~ThreadData()
{
	CloseRedis();
}

Redis* ThreadData::GetRedis()
{
	if(mRedis != NULL)
	{
		return mRedis;
	}
	else
	{
		mRedis = new Redis();
		Config* pConfig = Config::Singleton();
		if(mRedis->Conn(pConfig->mRedis_Ip, pConfig->mRedis_Port))
		{
			return mRedis;
		}
		else
		{
			DELETE(mRedis);
			return NULL;
		}
	}
}

void ThreadData::CloseRedis()
{
	if(mRedis != NULL)
	{
		DELETE(mRedis);
	}
}


