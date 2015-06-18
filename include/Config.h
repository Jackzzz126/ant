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
	string mIp;
	int mPort;
	int mBacklog;
};
#endif//_CONFIG_H_
