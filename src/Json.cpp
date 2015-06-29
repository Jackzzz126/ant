#include "comm.h"
#include "Json.h"

Json::Json()
{
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
				mDataArray[nodeName].push_back(json_as_int(*i));
				i++;
			}
		}
		else if(json_type(*iter) == JSON_NUMBER)
		{
			mDataFloat[nodeName] = json_as_float(*iter);
		}
		else if(json_type(*iter) == JSON_BOOL)
		{
			mDataString[nodeName] = json_as_string(*iter);
		}
		else if(json_type(*iter) == JSON_STRING)
		{
			mDataString[nodeName] = json_as_string(*iter);
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
				mDataArray[key + ":" + nodeName].push_back(json_as_int(*i));
				i++;
			}
		}
		else if(json_type(*iter) == JSON_NUMBER)
		{
			mDataFloat[key + ":" + nodeName] = json_as_float(*iter);
		}
		else if(json_type(*iter) == JSON_BOOL)
		{
			mDataString[key + ":" + nodeName] = json_as_string(*iter);
		}
		else if(json_type(*iter) == JSON_STRING)
		{
			mDataString[key + ":" + nodeName] = json_as_string(*iter);
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
	map<string, string>::iterator iter = mDataString.find(key);
	if(iter == mDataString.end())
	{
		return false;
	}
	else
	{
		str = mDataString[key];
		return true;
	}
}
bool Json::GetValue(const string& key, vector<int>& array)
{
	map<string, vector<int> >::iterator iter = mDataArray.find(key);
	if(iter == mDataArray.end())
	{
		return false;
	}
	else
	{
		array = mDataArray[key];
		return true;
	}
}
