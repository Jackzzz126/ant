#ifndef _CONFIG_H_
#define _CONFIG_H_

class Config
{
public:
	Config();
	~Config(){};

	bool Load(char* file);
public:
	//server
	string mIp;
	int mPort;
	int mBacklog;
	//log
	FILE* mLogFileHanle;
	FILE* mErrFileHanle;

private:
	//log
	string mLogFileName;
	string mErrFileName;
};
#endif//_CONFIG_H_
