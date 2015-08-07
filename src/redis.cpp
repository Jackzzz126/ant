#include "comm.h"
#include "redis.h"

Redis::Redis()
{
	mContext = NULL;
}
Redis::~Redis()
{
	redisFree(mContext);
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

bool Redis::RunCmd(const char* cmd)
{
	redisReply* pReply = (redisReply*)redisCommand(mContext, cmd);
	if(pReply == NULL)
		return false;

	if(pReply->type == REDIS_REPLY_STATUS)
	{
		if(string(pReply->str) == "OK")
		{
			freeReplyObject(pReply);
			return true;
		}
		else
		{
			freeReplyObject(pReply);
			return false;
		}
	}
	else
	{
		freeReplyObject(pReply);
		return false;
	}
}

bool Redis::RunCmd(int* reply, const char* cmd)
{
	redisReply* pReply = (redisReply*)redisCommand(mContext, cmd);
	if(pReply == NULL)
		return false;

	if(pReply->type == REDIS_REPLY_INTEGER)
	{
		*reply = (int)(pReply->integer);
		freeReplyObject(pReply);
		return true;
	}
	else
	{
		freeReplyObject(pReply);
		return false;
	}
}

bool Redis::RunCmd(string& reply, const char* cmd)
{
	redisReply* pReply = (redisReply*)redisCommand(mContext, cmd);
	if(pReply == NULL)
		return false;

	if(pReply->type == REDIS_REPLY_STRING)
	{
		reply = string(pReply->str);
		freeReplyObject(pReply);
		return true;
	}
	else if(pReply->type == REDIS_REPLY_NIL)
	{
		reply = "";
		freeReplyObject(pReply);
		return true;
	}
	else
	{
		freeReplyObject(pReply);
		return false;
	}
}

bool Redis::RunCmd(vector<string>& reply, const char* cmd)
{
	redisReply* pReply = (redisReply*)redisCommand(mContext, cmd);
	if(pReply == NULL)
		return false;

	if(pReply->type == REDIS_REPLY_ARRAY)
	{
		reply.clear();
		for(size_t i = 0; i < pReply->elements; i++)
		{
			reply.push_back(string(pReply->element[i]->str));
		}
		freeReplyObject(pReply);
		return true;
	}
	else
	{
		freeReplyObject(pReply);
		return false;
	}
}


