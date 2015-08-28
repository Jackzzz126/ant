#ifndef _CONFIG_H_
#define _CONFIG_H_

class Config
{
public:
	static Config* Singleton();
private:
	static Config* mConfigSingleton;
private:
	Config();
	~Config(){};

public:
	bool Load(char* file);
	void Release();
public:
	//server
	string mIp;
	int mPort;
	int mUdpPort;
	int mBacklog;
	bool mDaemon;
	int mSockTimeout;
	int mWorkerThreads;
	int mDbThreads;
	//log
	FILE* mLogFileHandle;
	FILE* mErrFileHandle;
	//redis
	string mRedis_Ip;
	int mRedis_Port;
	string mRedis_Char;

private:
	//log
	string mLogFileName;
	string mErrFileName;
};
#endif//_CONFIG_H_
