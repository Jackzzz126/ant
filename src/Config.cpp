#include "comm.h"
#include "Log.h"
#include "Config.h"

Config::Config()
{
	mPort = 0;
	mBackLog = 128;

	mAuthPort = 0;
	mGatePort = 0;

	mRegInterval = 0;
}

bool Config::Load(char* fileName)
{
	FILE* filePtr = fopen(fileName, "r");
	if( filePtr == NULL )
	{
		Log::Error( "Error when parse %s: %s.\n", fileName, strerror(errno) );
		return false;
	}

	char buff[128];
	while(!feof(filePtr) && fgets( buff, 128, filePtr ) != NULL)
	{
		string key;
		string value;
		if(!Parse(string(buff), key, value))
		{
			Log::Error( "Error when parsing: %s.\n", buff );
			return false;
		}
		if(key == "ip")
		{
			mIp = value;
		}
		else if(key == "port")
		{
			mPort = atoi(value.c_str());
		}
		else if(key == "backlog")
		{
			mBackLog = atoi(value.c_str());
		}
		else if(key == "authIp")
		{
			mAuthIp = value;
		}
		else if(key == "authPort")
		{
			mAuthPort = atoi(value.c_str());
		}
		else if(key == "gateIp")
		{
			mGateIp = value;
		}
		else if(key == "gatePort")
		{
			mGatePort = atoi(value.c_str());
		}
		else if(key == "regInterval")
		{
			mRegInterval = atoi(value.c_str());
		}
	}

	if( mPort == 0 ||
		mIp == "" ||

		mAuthIp == "" ||
		mAuthPort == 0 ||
		mGateIp == "" ||
		mGatePort == 0 ||

		mRegInterval == 0
		)
	{
		return false;
	}

	return true;
}

bool Config::Parse(const string& str, string& key, string& value)
{
	int equalPos = str.find("=");
	key = str.substr(0, equalPos);
	value = str.substr(equalPos + 1, str.length() - equalPos - 2);

	String::Trim(key);
	String::Trim(value);

	return true;
}
