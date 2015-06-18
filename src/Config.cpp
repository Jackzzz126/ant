#include "comm.h"
#include "Log.h"
#include "Config.h"
#include "Json.h"

Config::Config()
{
	mPort = 0;
	mBacklog = 128;
}

bool Config::Load(char* fileName)
{
	FILE* pFile = fopen(fileName, "r");
	if(pFile == NULL)
	{
		Log::Error( "Error when open %s: %s.\n", fileName, strerror(errno) );
		return false;
	}

	fseek(pFile, 0, SEEK_END);
	size_t fileLen = ftell(pFile); 
	char buff[1024 * (fileLen / 1024 + 1)];
	fseek(pFile, 0, SEEK_SET);
	size_t readLen = fread(buff, 1, fileLen, pFile);
	if(readLen != fileLen)
	{
		Log::Error( "Error when read %s: %s.\n", fileName, strerror(errno) );
		return false;
	}
	buff[fileLen] = '\0';
	
	Json json;
	if(!json.Parse(buff))
	{
		Log::Error( "Error when parse %s: %s.\n", fileName, strerror(errno) );
		return false;
	}
	if( !json.GetValue("server:port", &mPort) ||
		!json.GetValue("server:ip", mIp) ||
		!json.GetValue("server:backlog", &mBacklog)
	  )
	{
		Log::Error( "Error when parse %s: %s.\n", fileName, strerror(errno) );
		return false;
	}
	//read txt file code, save for reference.
	//while(!feof(filePtr) && fgets( buff, 128, filePtr ) != NULL)
	//{
	//	string key;
	//	string value;
	//	if(!Parse(string(buff), key, value))
	//	{
	//		Log::Error( "Error when parsing: %s.\n", buff );
	//		return false;
	//	}
	//	if(key == "ip")
	//	{
	//		mIp = value;
	//	}
	//	else if(key == "port")
	//	{
	//		mPort = atoi(value.c_str());
	//	}
	//}

	if( mPort == 0 ||
		mIp == ""
	  )
	{
		return false;
	}

	return true;
}

//bool Config::Parse(const string& str, string& key, string& value)
//{
//	int equalPos = str.find("=");
//	key = str.substr(0, equalPos);
//	value = str.substr(equalPos + 1, str.length() - equalPos - 2);
//
//	String::Trim(key);
//	String::Trim(value);
//
//	return true;
//}
