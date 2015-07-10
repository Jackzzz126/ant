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

