#ifndef _STRING_H_
#define _STRING_H_

class String
{
public:
	static void Trim(string& str);
	static string Format(const char* str, ...);
	static char mBuff[1024];
};
#endif//_STRING_H_
