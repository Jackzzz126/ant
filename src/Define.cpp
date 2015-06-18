#include "comm.h"
#include "Define.h"

string GetClientAddr(uv_stream_t *conn)
{
	struct sockaddr clientAddr;
	int addrLen = sizeof clientAddr;
	uv_tcp_getpeername((uv_tcp_t*)conn, &clientAddr, &addrLen);

	struct sockaddr_in* inetAddr = (struct sockaddr_in*)(&clientAddr);
	char ip[17];
	uv_ip4_name(inetAddr, ip, sizeof(ip));
	return String::Format("%s:%d", ip, ntohs(inetAddr->sin_port));
}

uv_buf_t NewBuffer(int size)
{
	uv_buf_t buff;
	buff.base = new char[size];
	//memset(buff.base, 0, size);
	buff.len = size;
	return buff;
}

void DelBuffer(uv_buf_t& buff)
{
	delete [](char*)buff.base;
	buff.base = NULL;
	buff.len = 0;
}


