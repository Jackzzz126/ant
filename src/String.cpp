#include "comm.h"
#include "String.h"

void String::Trim(string& str)
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

char String::mBuff[1024];
string String::Format(const char* str, ...)
{
	va_list arg;
	va_start(arg, str);
	vsnprintf(mBuff, 1024, str, arg);
	va_end(arg);
	return string(String::mBuff);
}

