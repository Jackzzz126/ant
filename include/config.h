#ifndef _CONFIG_H_
#define _CONFIG_H_

class Config
{
private:
	Config();
	~Config(){};

public:
	bool Load(char* file);
	void Release();

	static Config* Singleton();
public:
	//server
	string mIp;
	int mPort;
	int mBacklog;
	//log
	FILE* mLogFileHandle;
	FILE* mErrFileHandle;
	bool mDaemon;
	int mSockTimeout;
	int mWorkerThreads;

private:
	//log
	string mLogFileName;
	string mErrFileName;

	static Config* mGlobalConfig;
};
#endif//_CONFIG_H_
