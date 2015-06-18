#ifndef _GATE_H_
#define _GATE_H_

#include "HttpReq.h"

class Gate : public HttpReq
{
public:
	static void Reg(uv_timer_t *handle, int status);
	static void OnRegConn(uv_connect_t* req, int status);
	static void OnRegReturn(uv_stream_t *conn, ssize_t nread, uv_buf_t recvBuff);
public:
	Gate();
	~Gate(){};
public:
};
#endif//_GATE_H_

