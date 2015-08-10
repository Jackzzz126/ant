#ifndef _JSON_H_
#define _JSON_H_
#ifndef NDEBUG
#define NDEBUG
#include "libjson.h"
#undef NDEBUG
#else
#include "libjson.h"
#endif


//all value must have a key
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
	
	//to string example
	JSONNODE *n = json_new(JSON_NODE);
	json_push_back(n, json_new_a("String Node", "String Value"));
	json_push_back(n, json_new_i("Integer Node", 42));
	json_push_back(n, json_new_f("Floating Point Node", 3.14));
	json_push_back(n, json_new_b("Boolean Node", 1));
	json_char *jc = json_write_formatted(n);
	printf("%s\n", jc);
	json_free(jc);
	json_delete(n);
	
	JSONNODE *n = json_new(JSON_NODE);
	json_push_back(n, json_new_a("RootA", "Value in parent node"));
	JSONNODE *c = json_new(JSON_NODE);
	json_set_name(c, "ChildNode");
	json_push_back(c, json_new_a("ChildA", "String Value"));
	json_push_back(c, json_new_i("ChildB", 42));
	json_push_back(n, c);
	json_char *jc = json_write_formatted(n);
	printf("%s\n", jc);
	json_free(jc);
	json_delete(n);

	JSONNODE *n = json_new(JSON_NODE);
	json_push_back(n, json_new_a("RootA", "Hello World"));
	JSONNODE *c = json_new(JSON_ARRAY);
	json_set_name(c, "ArrayOfNumbers");
	json_push_back(c, json_new_i(NULL, 16));
	json_push_back(c, json_new_i(NULL, 42));
	json_push_back(c, json_new_i(NULL, 128));
	json_push_back(n, c);
	json_char *jc = json_write_formatted(n);
	printf("%s\n", jc);
	json_free(jc);
	json_delete(n);
*/
class Json
{
public:
	Json();
	~Json();
public:
	bool Parse(const string& str);
	bool GetValue(const string& key, int* value);
	bool GetValue(const string& key, float* value);
	bool GetValue(const string& key, string& str);
	bool GetValue(const string& key, bool* pBool);
	bool GetValue(const string& key, vector<int>& array);
	bool GetValue(const string& key, vector<float>& array);
	bool GetValue(const string& key, vector<string>& array);

	//for form str
	string ToString();
private:
	bool Parse(const string& key, JSONNODE* node);
private:
	//for parse
	map<string, float> mDataFloat;
	map<string, string> mDataStr;
	map<string, bool> mDataBool;
	map<string, vector<float> > mDataFloatArray;
	map<string, vector<string> > mDataStrArray;
public:
	//for form str
	JSONNODE* mpRootNode;
};
#endif//_JSON_H_

