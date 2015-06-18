#ifndef _JSON_H_
#define _JSON_H_
#include "libjson.h"

//all value must have a key, array only with int
/*
for data:
    {"test":{"id":1, "name":"aaaaa", "equip":[1,2,3]}}
use like this:
	Json json;
	if(!json.Parse(string(buff, size)))
	{
		UserMgr::CloseUser(mConn, User::UNKNOWN_DATA);
		return;
	}
	int id = 0;
	string name;
	vector<int> equip;
	if(!json.GetValue("test:id", &id)
		|| !json.GetValue("test:name", name)
		|| !json.GetValue("test:equip", equip)
		)
	{
		UserMgr::CloseUser(mConn, User::UNKNOWN_DATA);
		return;
	}
	else
	{
	}
*/
class Json
{
public:
	Json();
	~Json(){};
public:
	bool Parse(const string& str);
	bool GetValue(const string& key, int* value);
	bool GetValue(const string& key, float* value);
	bool GetValue(const string& key, string& str);
	bool GetValue(const string& key, vector<int>& array);
private:
	bool Parse(const string& key, JSONNODE* node);
public:
	map<string, float> mDataFloat;
	map<string, string> mDataString;
	map<string, vector<int> > mDataArray;
};
#endif//_JSON_H_

