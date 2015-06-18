#ifndef _TIME_H_
#define _TIME_H_
#include <time.h>
#include <sys/time.h>

class Time
{
public:
	/*
		int startTime = Time::GetSysTime();
		int startTime1 = Time::GetProcTime();
		int endTime = Time::GetSysTime();
		int endTime1 = Time::GetProcTime();
		Log::Error( "time: %d\n", endTime - startTime );
		Log::Error( "cpu time: %d\n", endTime1 - startTime1 );
	*/
	//should use int 64
	//static int GetSysTime();//time stamp in millisecond
	static int GetProcTime();//time stamp in millisecond

	static string GetTimeStr();
};
#endif//_TIME_H_
