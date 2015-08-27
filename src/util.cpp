#include "comm.h"
#include "define.h"

char* NewBuff(int size)
{
	return (char*)malloc(size);
}
void DelBuff(char** pp)
{
	free(*pp);
	*pp = NULL;
}

string Util::ToHex(const char* data, int len)
{
	char* buff = NewBuff(len * 2 + 1);
	buff[len * 2] = '\0';
	for(int i = 0; i < len; i++)
	{
		sprintf(buff + i * 2, "%02x", (unsigned char)data[i]);
	}
	string str(buff);
	DelBuff(&buff);
	return str;
}

string Util::GetIpByName(const char* hostName)
{
	struct hostent* hostEnt = gethostbyname(hostName);
	if(hostEnt == NULL)
	{
		Log::Error("Error when parse addr: %s.\n", hostName);
		return "";
	}

	char ipstr[16];
	inet_ntop(AF_INET, (hostEnt->h_addr_list)[0], ipstr, 16);
	return string(ipstr);
}

