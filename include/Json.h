#ifndef _JSON_H_
#define _JSON_H_
#ifndef NDEBUG
#define NDEBUG
#include "libjson.h"
#undef NDEBUG
#else
#include "libjson.h"
#endif


//all value must have a key, array only with int
/*
for data:
    {"test":{"id":1, "name":"aaaaa", "equip":[1,2,3]}}
use like this:
	Json json;
	if(!json.Parse(string(buff, size)))
	{
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
		return;
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
	bool GetValue(const string& key, vector<float>& array);
	bool GetValue(const string& key, vector<string>& array);
	bool GetValue(const string& key, bool* pBool);
private:
	bool Parse(const string& key, JSONNODE* node);
public:
	map<string, float> mDataNum;
	map<string, string> mDataStr;
	map<string, vector<float> > mDataNumArray;
	map<string, vector<string> > mDataStrArray;
};
#endif//_JSON_H_

