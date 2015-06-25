#include "comm.h"
#include "Log.h"
#include "Time.h"
#include "Config.h"

void Log::Out(const char* str, ...)
{
	string nowStr = Time::GetNowStr();
	va_list arg;
	va_start(arg, str);
	printf("[LOG %s] ", nowStr.c_str());
	vprintf(str, arg);
	va_end(arg);
}
void Log::Error(const char* str, ...)
{
	string nowStr = Time::GetNowStr();
	va_list arg;
	va_start(arg, str);
	printf("[ERR %s] ", nowStr.c_str());
	vprintf(str, arg);
	va_end(arg);
}
void Log::Debug(const char* str, ...)
{
	string nowStr = Time::GetNowStr();
	va_list arg;
	va_start(arg, str);
	printf("[DBG %s] ", nowStr.c_str());
	vprintf(str, arg);
	va_end(arg);
}


