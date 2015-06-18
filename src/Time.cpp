#include "comm.h"
#include "Time.h"

//int Time::GetSysTime()
//{
//	timeval time;
//	gettimeofday(&time, NULL);
//	return time.tv_sec * 1000 + time.tv_usec / 1000;
//}

int Time::GetProcTime()
{
	clock_t time = clock( );
	return time * 1000 / CLOCKS_PER_SEC;
}

string Time::GetTimeStr()
{
	time_t rawTime;
	struct tm * timeInfo;
	char timeBuff[128];
	time(&rawTime);
	timeInfo = localtime(&rawTime);
	strftime(timeBuff, 128, "%Y-%m-%d %H:%M:%S", timeInfo);
	return string(timeBuff);
}
