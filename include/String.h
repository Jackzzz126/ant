#ifndef _STRING_H_
#define _STRING_H_

class String
{
public:
	static void Trim(string& str);
	static string Format(const char* str, ...);
};
#endif//_STRING_H_
