#ifndef _TIME_H_
#define _TIME_H_
#include "comm.h"
#include <time.h>
#include <sys/time.h>

class DateTime
{
public:
	static int GetTimeStamp();//time stamp in seconds
	static int GetProcTime();//time stamp in milliseconds

	static string GetNowStr();
};
#endif//_TIME_H_
