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

string ToHex(const char* data, int len)
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
