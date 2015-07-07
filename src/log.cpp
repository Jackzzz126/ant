#include "comm.h"
#include "log.h"
#include "dateTime.h"
#include "config.h"

void Log::Out(const char* str, ...)
{
	string nowStr = DateTime::GetNowStr();

	va_list arg;
	va_start(arg, str);

	FILE* logFileHandle = Config::Singleton()->mLogFileHandle;
	fprintf(logFileHandle, "[LOG %s] ", nowStr.c_str());
	vfprintf(logFileHandle, str, arg);
	fflush(logFileHandle);

	va_end(arg);
}
void Log::Error(const char* str, ...)
{
	string nowStr = DateTime::GetNowStr();

	va_list arg;
	va_start(arg, str);

	FILE* errFileHandle = Config::Singleton()->mErrFileHandle;
	fprintf(errFileHandle, "[ERR %s] ", nowStr.c_str());
	vfprintf(errFileHandle, str, arg);
	fflush(errFileHandle);

	va_end(arg);
}
//void Log::Debug(const char* str, ...)
//{
//	string nowStr = DateTime::GetNowStr();
//	va_list arg;
//	va_start(arg, str);
//	fprintf("[DBG %s] ", nowStr.c_str());
//	fvprintf(str, arg);
//	va_end(arg);
//}


