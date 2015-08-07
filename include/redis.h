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

	bool RunCmd(const char* cmd);
	bool RunCmd(int* reply, const char* cmd);
	bool RunCmd(string& reply, const char* cmd);
	bool RunCmd(vector<string>& reply, const char* cmd);
private:
	redisContext* mContext;
};

#endif//_REDIS_H_

