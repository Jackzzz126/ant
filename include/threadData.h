#ifndef _THREADDATA_H_
#define _THREADDATA_H_

#include "redis.h"

class ThreadData
{
public:
	static pthread_key_t mThreadKey;
public:
	ThreadData();
	virtual ~ThreadData();
public:
	Redis* GetRedis();
private:
	Redis* mRedis;
};

#endif//_THREADDATA_H_

