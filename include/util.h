#ifndef _UTIL_H_
#define _UTIL_H_

char* NewBuff(int size);
void DelBuff(char** pp);

class Util
{
public:
	static string ToHex(const char* data, int len);
	static string GetIpByName(const char* hostName);
};

#endif//_UTIL_H_