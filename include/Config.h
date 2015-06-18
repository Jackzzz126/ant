#ifndef _CONFIG_H_
#define _CONFIG_H_

class Config
{
public:
	Config();
	~Config(){};

	bool Load(char* file);
private:
	bool Parse(const string& str, string& key, string& value);

public:
	string mIp;
	int mPort;
	int mBackLog;

	string mAuthIp;
	int mAuthPort;
	string mGateIp;
	int mGatePort;

	int mRegInterval;
};
#endif//_CONFIG_H_
