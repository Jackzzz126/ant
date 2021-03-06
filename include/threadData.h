#ifndef _THREADDATA_H_
#define _THREADDATA_H_

#include "redis.h"
#include "luaState.h"

class ThreadData
{
public:
	static pthread_key_t mThreadKey;
public:
	ThreadData();
	virtual ~ThreadData();
public:
	Redis* GetRedis();
	LuaState* GetLuaState();
private:
	void CloseRedis();
	void CloseLuaState();
private:
	Redis* mRedis;
	LuaState* mLua;
};

#endif//_THREADDATA_H_

