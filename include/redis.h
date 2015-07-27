#ifndef _REDIS_H_
#define _REDIS_H_

#include "hiredis.h"
class Redis
{
public:
	Redis();
	virtual ~Redis();
public:
	bool Conn(const string& ip, int port);
private:
	redisContext* mContext;
};

#endif//_REDIS_H_

