#include "comm.h"
#include "config.h"
#include "json.h"
#include "luaState.h"

Config* Config::mConfigSingleton = NULL;
Config* Config::Singleton()
{
	if(mConfigSingleton == NULL)
	{
		mConfigSingleton = new Config();
	}
	return mConfigSingleton;
}
Config::Config()
{
	mPort = 0;
	mUdpPort = 0;
	mBacklog = 128;
	mDaemon = false;
	mWorkerThreads = 0;
	mDbThreads = 0;
	mSockTimeout = 60 * 5;

	mRedis_Port = 0;

	mLogFileHandle = stdout;
	mErrFileHandle = stderr;
}

bool Config::Load(char* fileName)
{
	LuaState* pLua = new LuaState();
	int ret = 0;
	char* errStr = NULL;
	pLua->Call("lua/config.lua", "check", "|is", &ret, &errStr);
	if(!ret)
	{
		vector<string> errStrs;
		StrUtil::Split(errStr, "\n", errStrs);
		vector<string>::iterator iter = errStrs.begin();
		for(; iter != errStrs.end(); iter++)
		{
			if(*iter != "")
			{
				Log::Error("%s\n", iter->c_str());
			}
		}
		return false;
	}

	if(!pLua->GetValue("lua/config.lua", "server", "ip", mIp)
		|| !pLua->GetValue("lua/config.lua", "server", "port", &mPort)
		|| !pLua->GetValue("lua/config.lua", "server", "udpPort", &mUdpPort)
		|| !pLua->GetValue("lua/config.lua", "server", "backlog", &mBacklog)
		|| !pLua->GetValue("lua/config.lua", "server", "daemon", &mDaemon)
		|| !pLua->GetValue("lua/config.lua", "server", "sockTimeout", &mSockTimeout)
		|| !pLua->GetValue("lua/config.lua", "server", "workerThreads", &mWorkerThreads)
		|| !pLua->GetValue("lua/config.lua", "server", "dbThreads", &mDbThreads)
		|| !pLua->GetValue("lua/config.lua", "log", "logFile", mLogFileName)
		|| !pLua->GetValue("lua/config.lua", "log", "errFile", mErrFileName)
		|| !pLua->GetValue("lua/config.lua", "redis", "ip", mRedis_Ip)
		|| !pLua->GetValue("lua/config.lua", "redis", "port", &mRedis_Port)
		|| !pLua->GetValue("lua/config.lua", "redis", "char", mRedis_Char)
	  )
	{
		Log::Error("Error when parse config: some value miss.\n");
		return false;
	}

	//FILE* pFile = fopen(fileName, "r");
	//if(pFile == NULL)
	//{
	//	Log::Error("Error when open %s: %s.\n", fileName, strerror(errno));
	//	return false;
	//}

	//fseek(pFile, 0, SEEK_END);
	//size_t fileLen = ftell(pFile); 
	//char buff[1024 * (fileLen / 1024 + 1)];
	//fseek(pFile, 0, SEEK_SET);
	//size_t readLen = fread(buff, 1, fileLen, pFile);
	//if(readLen != fileLen)
	//{
	//	Log::Error("Error when read %s: %s.\n", fileName, strerror(errno));
	//	return false;
	//}
	//buff[fileLen] = '\0';
	//fclose(pFile);
	//
	//Json json;
	//if(!json.Parse(buff))
	//{
	//	Log::Error("Error when parse %s: %s.\n", fileName, strerror(errno));
	//	return false;
	//}
	//init
	if(mLogFileName == "" || mErrFileName == "")
	{
		mLogFileHandle = stdout;
		mErrFileHandle = stderr;
	}
	else
	{
		mLogFileHandle = fopen(mLogFileName.c_str(), "ab");
		mErrFileHandle = fopen(mErrFileName.c_str(), "ab");
		if(mLogFileHandle == NULL || mErrFileHandle == NULL)
		{
			Log::Error("Error when open log files:%s, %s.\n",
					mLogFileName.c_str(), mErrFileName.c_str());
			return false;
		}
	}

	return true;
}

void Config::Release()
{
	if(mLogFileHandle != stdout)
	{
		fclose(mLogFileHandle);
	}
	if(mErrFileHandle != stderr)
	{
		fclose(mErrFileHandle);
	}
}



