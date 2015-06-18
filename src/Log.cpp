#include "comm.h"
#include "Log.h"

void Log::Out(const char* str, ...)
{
	va_list arg;
	va_start(arg, str);
	printf("[LOG] ");
	vprintf(str, arg);
	va_end(arg);
}
void Log::Error(const char* str, ...)
{
	va_list arg;
	va_start(arg, str);
	printf("[ERR] ");
	vprintf(str, arg);
	va_end(arg);
}
void Log::Debug(const char* str, ...)
{
	va_list arg;
	va_start(arg, str);
	printf("[DBG] ");
	vprintf(str, arg);
	va_end(arg);
}


