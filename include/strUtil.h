#ifndef _STRING_H_
#define _STRING_H_
#include "comm.h"

class StrUtil
{
public:
	static void Trim(string& str);
	static void Format(char* buff, int len, const char* str, ...);
	static void Split(const string& str, const string& spliter, vector<string>& results);
};
#endif//_STRING_H_
