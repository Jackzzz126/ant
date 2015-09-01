#ifndef _UTIL_H_
#define _UTIL_H_

char* NewBuff(int size);
void DelBuff(char** pp);

class Util
{
public:
	static string ToHex(const char* data, int len);
	static string GetIpByName(const char* hostName);

	static string GetIpByAddr(sockaddr_in addr);
	static int GetPortByAddr(sockaddr_in addr);
};

#endif//_UTIL_H_
