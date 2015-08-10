#include "comm.h"
#include "json.h"

Json::Json()
{
	mpRootNode = json_new(JSON_NODE);
	mJsonStr = NULL;
}
Json::~Json()
{
	json_free(mJsonStr);
	json_delete(mpRootNode);
}

bool Json::Parse(const string& str)
{
	JSONNODE* node = json_parse(str.c_str());

	if (node == NULL)
		return false;

	JSONNODE_ITERATOR iter = json_begin(node);
	while (iter != json_end(node))
	{
		json_char* nodeName = json_name(*iter);
		if(string(nodeName) == "" || json_type(*iter) == JSON_NULL)
		{
			json_free(nodeName);
			break;
		}

		if (json_type(*iter) == JSON_NODE)
		{
			if(!Parse(nodeName, *iter))
				return false;
		}
		else if(json_type(*iter) == JSON_ARRAY)
		{
			JSONNODE_ITERATOR i = json_begin(*iter);
			while (i != json_end(*iter))
			{
				if(json_type(*iter) == JSON_NUMBER)
				{
					mDataFloatArray[nodeName].push_back(json_as_float(*i));
				}
				else
				{
					mDataStrArray[nodeName].push_back(json_as_string(*i));
				}
				i++;
			}
		}
		else if(json_type(*iter) == JSON_NUMBER)
		{
			mDataFloat[nodeName] = json_as_float(*iter);
		}
		else if(json_type(*iter) == JSON_BOOL)
		{
			mDataBool[nodeName] = json_as_bool(*iter);
		}
		else if(json_type(*iter) == JSON_STRING)
		{
			mDataStr[nodeName] = json_as_string(*iter);
		}
		json_free(nodeName);
		iter++;
	}
	json_delete(node);
	return true;
}

bool Json::Parse(const string& key, JSONNODE* node)
{
	JSONNODE_ITERATOR iter = json_begin(node);
	while (iter != json_end(node))
	{
		json_char* nodeName = json_name(*iter);
		if(string(nodeName) == "" || json_type(*iter) == JSON_NULL)
		{
			json_free(nodeName);
			break;
		}

		if (json_type(*iter) == JSON_NODE)
		{
			if(!Parse(key + ":" + nodeName, *iter))
				return false;
		}
		else if(json_type(*iter) == JSON_ARRAY)
		{
			JSONNODE_ITERATOR i = json_begin(*iter);
			while (i != json_end(*iter))
			{
				if(json_type(*iter) == JSON_NUMBER)
				{
					mDataFloatArray[nodeName].push_back(json_as_float(*i));
				}
				else
				{
					mDataStrArray[nodeName].push_back(json_as_string(*i));
				}
				i++;
			}
		}
		else if(json_type(*iter) == JSON_NUMBER)
		{
			mDataFloat[key + ":" + nodeName] = json_as_float(*iter);
		}
		else if(json_type(*iter) == JSON_BOOL)
		{
			mDataBool[key + ":" + nodeName] = json_as_bool(*iter);
		}
		else if(json_type(*iter) == JSON_STRING)
		{
			mDataStr[key + ":" + nodeName] = json_as_string(*iter);
		}
		json_free(nodeName);
		iter++;
	}
	return true;
}

bool Json::GetValue(const string& key, int* value)
{
	map<string, float>::iterator iter = mDataFloat.find(key);
	if(iter == mDataFloat.end())
	{
		return false;
	}
	else
	{
		*value = (int)mDataFloat[key];
		return true;
	}
}
bool Json::GetValue(const string& key, float* value)
{
	map<string, float>::iterator iter = mDataFloat.find(key);
	if(iter == mDataFloat.end())
	{
		return false;
	}
	else
	{
		*value = mDataFloat[key];
		return true;
	}
}
bool Json::GetValue(const string& key, string& str)
{
	map<string, string>::iterator iter = mDataStr.find(key);
	if(iter == mDataStr.end())
	{
		return false;
	}
	else
	{
		str = mDataStr[key];
		return true;
	}
}
bool Json::GetValue(const string& key, bool* pBool)
{
	map<string, string>::iterator iter = mDataStr.find(key);
	if(iter == mDataStr.end())
	{
		return false;
	}
	else
	{
		*pBool = mDataBool[key];
		return true;
	}
}
bool Json::GetValue(const string& key, vector<int>& array)
{
	map<string, vector<float> >::iterator iter = mDataFloatArray.find(key);
	if(iter == mDataFloatArray.end())
	{
		return false;
	}
	else
	{
		vector<float>::iterator iter2 = mDataFloatArray[key].begin();
		for(; iter2 != mDataFloatArray[key].end(); iter2++)
		{
			array.push_back((int)(*iter2));
		}
		return true;
	}
}
bool Json::GetValue(const string& key, vector<float>& array)
{
	map<string, vector<float> >::iterator iter = mDataFloatArray.find(key);
	if(iter == mDataFloatArray.end())
	{
		return false;
	}
	else
	{
		array = mDataFloatArray[key];
		return true;
	}
}
bool Json::GetValue(const string& key, vector<string>& array)
{
	map<string, vector<string> >::iterator iter = mDataStrArray.find(key);
	if(iter == mDataStrArray.end())
	{
		return false;
	}
	else
	{
		array = mDataStrArray[key];
		return true;
	}
}

char* Json::ToStr()
{
	if(mJsonStr == NULL)
	{
		mJsonStr = json_write(mpRootNode);
	}
	return mJsonStr;
}

void Json::Reset()
{
	json_free(mJsonStr);
	json_delete(mpRootNode);

	mpRootNode = json_new(JSON_NODE);
	mJsonStr = NULL;
}

void Json::SetValue(JSONNODE* node, const string& key, int value)
{
	if(node == NULL)
		node = mpRootNode;
	json_push_back(node, json_new_i(key.c_str(), value));
}
void Json::SetValue(JSONNODE* node, const string& key, float value)
{
	if(node == NULL)
		node = mpRootNode;
	json_push_back(node, json_new_f(key.c_str(), value));
}
void Json::SetValue(JSONNODE* node, const string& key, string& str)
{
	if(node == NULL)
		node = mpRootNode;
	json_push_back(node, json_new_a(key.c_str(), str.c_str()));
}
void Json::SetValue(JSONNODE* node, const string& key, bool value)
{
	if(node == NULL)
		node = mpRootNode;
	json_push_back(node, json_new_b(key.c_str(), value));
}
void Json::SetValue(JSONNODE* node, const string& key, vector<int>& array)
{
	if(node == NULL)
		node = mpRootNode;

	JSONNODE* c = json_new(JSON_ARRAY);
	json_set_name(c, key.c_str());
	for(size_t i = 0; i < array.size(); i++)
	{
		json_push_back(c, json_new_i(NULL, array[i]));
	}
	json_push_back(node, c);
}
void Json::SetValue(JSONNODE* node, const string& key, vector<float>& array)
{
	if(node == NULL)
		node = mpRootNode;

	JSONNODE* c = json_new(JSON_ARRAY);
	json_set_name(c, key.c_str());
	for(size_t i = 0; i < array.size(); i++)
	{
		json_push_back(c, json_new_f(NULL, array[i]));
	}
	json_push_back(node, c);
}
void Json::SetValue(JSONNODE* node, const string& key, vector<string>& array)
{
	if(node == NULL)
		node = mpRootNode;

	JSONNODE* c = json_new(JSON_ARRAY);
	json_set_name(c, key.c_str());
	for(size_t i = 0; i < array.size(); i++)
	{
		json_push_back(c, json_new_a(NULL, array[i].c_str()));
	}
	json_push_back(node, c);
}

void Json::AddChild(JSONNODE* node, JSONNODE* childNode)
{
	if(node == NULL)
		node = mpRootNode;
	json_push_back(node, childNode);
}


