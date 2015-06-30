#ifndef _LOG_H_
#define _LOG_H_
#include <stdarg.h>

class Log
{
public:
	static void Out(const char* str, ...);
	//static void Debug(const char* str, ...);
	static void Error(const char* str, ...);
};
#endif//_LOG_H_
