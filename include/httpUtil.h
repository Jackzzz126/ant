#ifndef _HTTPUTIL_H_
#define _HTTPUTIL_H_
#include "comm.h"

class HttpUtil
{
public:
	static void ResStr(int sock, const string& str);
};
#endif//_HTTPUTIL_H_
