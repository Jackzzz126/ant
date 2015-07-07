#ifndef _STRING_H_
#define _STRING_H_
#include "comm.h"

class StrUtil
{
public:
	static void Trim(string& str);
	static string Format(const char* str, ...);
};
#endif//_STRING_H_
