#include "comm.h"
#include "redis.h"

Redis::Redis()
{
	mContext = NULL;
}
Redis::~Redis()
{
}

bool Redis::Conn(const string& ip, int port)
{
	mContext = redisConnect(ip.c_str(), port);
	if (mContext != NULL && mContext->err) {
		Log::Error("Error when connect to redis: %s.\n", mContext->errstr);
		return false;
	}
	else
	{
		return true;
	}
}


