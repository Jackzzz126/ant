#include "comm.h"
#include "strUtil.h"

void StrUtil::Trim(string& str)
{
	string::size_type pos = str.find_last_not_of(' ');
	if(pos != string::npos)
	{
		str.erase(pos + 1);
		pos = str.find_first_not_of(' ');
		if(pos != string::npos)
		{
			str.erase(0, pos);
		}
	}
	else
	{
		str.erase(str.begin(), str.end());
	}
}

void StrUtil::Format(char* buff, int len, const char* str, ...)
{
	va_list arg;
	va_start(arg, str);
	vsnprintf(buff, len, str, arg);
	va_end(arg);
}

void StrUtil::Split(const string& str, const string& spliter, vector<string>& results)
{
	size_t startPos = 0;
	size_t curPos = str.find_first_of(spliter, startPos);
	while(curPos != string::npos)
	{
		results.push_back(str.substr(startPos, curPos - startPos));
		startPos = curPos + 1;
		curPos = str.find_first_of(spliter, startPos);
	}
	if((curPos - startPos) > 0)
	{
		results.push_back(str.substr(startPos, curPos + startPos));
	}
}

