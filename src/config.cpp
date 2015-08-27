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


	FILE* pFile = fopen(fileName, "r");
	if(pFile == NULL)
	{
		Log::Error("Error when open %s: %s.\n", fileName, strerror(errno));
		return false;
	}

	fseek(pFile, 0, SEEK_END);
	size_t fileLen = ftell(pFile); 
	char buff[1024 * (fileLen / 1024 + 1)];
	fseek(pFile, 0, SEEK_SET);
	size_t readLen = fread(buff, 1, fileLen, pFile);
	if(readLen != fileLen)
	{
		Log::Error("Error when read %s: %s.\n", fileName, strerror(errno));
		return false;
	}
	buff[fileLen] = '\0';
	fclose(pFile);
	
	Json json;
	if(!json.Parse(buff))
	{
		Log::Error("Error when parse %s: %s.\n", fileName, strerror(errno));
		return false;
	}
	if( !json.GetValue("server:port", &mPort)
		|| !json.GetValue("server:ip", mIp)
		|| !json.GetValue("server:backlog", &mBacklog)
		|| !json.GetValue("server:daemon", &mDaemon)
		|| !json.GetValue("server:sockTimeout", &mSockTimeout)
		|| !json.GetValue("server:workerThreads", &mWorkerThreads)
		|| !json.GetValue("server:dbThreads", &mDbThreads)
		|| !json.GetValue("log:logFile", mLogFileName)
		|| !json.GetValue("log:errFile", mErrFileName)
		|| !json.GetValue("redis:ip", mRedis_Ip)
		|| !json.GetValue("redis:port", &mRedis_Port)
		|| !json.GetValue("redis:charIdSeed", mRedis_CharIdSeed)
		|| !json.GetValue("redis:char", mRedis_Char)
	  )
	{
		Log::Error("Error when parse %s: some value miss.\n", fileName);
		return false;
	}

	//check
	if(mPort == 0 || mIp == "")
	{
		Log::Error( "Error when parse %s: ip, port invalid.\n", fileName);
		return false;
	}
	if((mLogFileName == "" || mErrFileName == "") && mDaemon)
	{
		Log::Error( "Error when parse %s: must set log file in daemon mode.\n", fileName);
		return false;
	}
	if(mWorkerThreads < 1)
	{
		Log::Error( "Error when parse %s: worker threads must > 1.\n", fileName);
		return false;
	}
	if(mRedis_Port == 0 || mRedis_Ip == "" || mRedis_CharIdSeed == ""
			|| mRedis_Char == "")
	{
		Log::Error( "Error when parse %s: reids ip, port or keys invalid.\n", fileName);
		return false;
	}

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



