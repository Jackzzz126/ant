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

string String::Format(const char* str, ...)
{
	char buff[1024];
	va_list arg;
	va_start(arg, str);
	vsnprintf(buff, 1024, str, arg);
	va_end(arg);
	return string(buff);
}

