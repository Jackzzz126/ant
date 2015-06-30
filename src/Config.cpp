#include "comm.h"
#include "Config.h"
#include "Json.h"

Config::Config()
{
	mPort = 0;
	mBacklog = 128;
}

bool Config::Load(char* fileName)
{
	FILE* pFile = fopen(fileName, "r");
	if(pFile == NULL)
	{
		printf("Error when open %s: %s.\n", fileName, strerror(errno));
		return false;
	}

	fseek(pFile, 0, SEEK_END);
	size_t fileLen = ftell(pFile); 
	char buff[1024 * (fileLen / 1024 + 1)];
	fseek(pFile, 0, SEEK_SET);
	size_t readLen = fread(buff, 1, fileLen, pFile);
	if(readLen != fileLen)
	{
		printf("Error when read %s: %s.\n", fileName, strerror(errno));
		return false;
	}
	buff[fileLen] = '\0';
	
	Json json;
	if(!json.Parse(buff))
	{
		printf("Error when parse %s: %s.\n", fileName, strerror(errno));
		return false;
	}
	if( !json.GetValue("server:port", &mPort) ||
		!json.GetValue("server:ip", mIp) ||
		!json.GetValue("server:backlog", &mBacklog) ||
		!json.GetValue("log:logFile", mLogFileName) ||
		!json.GetValue("log:errFile", mErrFileName)
	  )
	{
		printf("Error when parse %s: some value miss.\n", fileName);
		return false;
	}
	mLogFileHanle = fopen(mLogFileName.c_str(), "ab");
	mErrFileHanle = fopen(mErrFileName.c_str(), "ab");
	if(mLogFileHanle == NULL || mErrFileHanle == NULL)
	{
		printf("Error when open log files:%s, %s.\n", mLogFileName.c_str(), mErrFileName.c_str());
		return false;
	}

	if( mPort == 0 ||
		mIp == ""
	  )
	{
		printf( "Error when parse %s: some value invalid.\n", fileName);
		return false;
	}
	return true;
}



