#include "comm.h"
#include "dateTime.h"

int DateTime::GetTimeStamp()
{
	timeval time;
	gettimeofday(&time, NULL);
	return time.tv_sec;
}

int DateTime::GetProcTime()
{
	clock_t time = clock( );
	return time * 1000 / CLOCKS_PER_SEC;
}

string DateTime::GetNowStr()
{
	time_t rawTime;
	struct tm * timeInfo;
	char timeBuff[128];
	time(&rawTime);
	timeInfo = localtime(&rawTime);
	strftime(timeBuff, 128, "%Y-%m-%d %H:%M:%S", timeInfo);
	return string(timeBuff);
}
