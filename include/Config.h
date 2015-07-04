#ifndef _CONFIG_H_
#define _CONFIG_H_

class Config
{
public:
	Config();
	~Config(){};

	bool Load(char* file);
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
	//modules
	vector<string> mModuleNames;

private:
	//log
	string mLogFileName;
	string mErrFileName;

	static Config* mGlobalConfig;
};
#endif//_CONFIG_H_
