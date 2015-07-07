#include "comm.h"
#include "define.h"

string GetClientAddr(uv_stream_t *conn)
{
	struct sockaddr clientAddr;
	int addrLen = sizeof clientAddr;
	uv_tcp_getpeername((uv_tcp_t*)conn, &clientAddr, &addrLen);

	struct sockaddr_in* inetAddr = (struct sockaddr_in*)(&clientAddr);
	char ip[17];
	uv_ip4_name(inetAddr, ip, sizeof(ip));
	return StrUtil::Format("%s:%d", ip, ntohs(inetAddr->sin_port));
}

uv_buf_t NewUvBuff(int size)
{
	uv_buf_t buff;
	buff.base = NewBuff(size);
	//memset(buff.base, 0, size);
	buff.len = size;
	return buff;
}

void DelUvBuff(uv_buf_t& buff)
{
	DelBuff(&buff.base);
	buff.len = 0;
}

char* NewBuff(int size)
{
	return (char*)malloc(size);
}
void DelBuff(char** pp)
{
	free(*pp);
	*pp = NULL;
}

