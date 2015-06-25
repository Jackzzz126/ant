#ifndef _CONFIG_H_
#define _CONFIG_H_

class Config
{
public:
	Config();
	~Config(){};

	bool Load(char* file);
	//bool Parse(const string& str, string& key, string& value);

public:
	//server
	string mIp;
	int mPort;
	int mBacklog;

	//log
	string mLogFile;
	string mErrFile;
};
#endif//_CONFIG_H_
