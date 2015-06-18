#ifndef _HTTPREQ_H_
#define _HTTPREQ_H_

class HttpConn
{
public:
	HttpConn(){ mSocket = NULL; mConn = NULL;};
public:
	uv_tcp_t* mSocket;
	uv_connect_t* mConn;
};

class HttpReq
{
public:
	static uv_buf_t OnAllocBuffer(uv_handle_t *handle, size_t suggested_size);
	static void SendHttpPack(uv_connect_t* conn, const string& url, uv_buf_t buff);
	static void OnWriteHttp(uv_write_t *req, int status);

	static void ClearConn(uv_stream_t* handle);
public:
	HttpReq();
	~HttpReq();
};
#endif//_HTTPREQ_H_

