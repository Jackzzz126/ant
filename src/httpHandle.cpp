#include "comm.h"
#include "httpHandle.h"

map<string, HttpHandler> HttpHandle::mAllHttpHandlers;

void HttpHandle::AddHandler(const string& url, HttpHandler handler)
{
	assert(handler);

	map<string, HttpHandler>::iterator iter;
	iter = mAllHttpHandlers.find(url);
	if(iter != mAllHttpHandlers.end())
	{
		Log::Error("Duplicate url added: %s.\n", url.c_str());
	}
	else
	{
		mAllHttpHandlers[url] = handler;
	}
}
void HttpHandle::Init()
{
	AddHandler("/hello", HttpHandle::Hello);
}
void HttpHandle::Handle(int sock, const string& url, char* data, int size)
{
	map<string, HttpHandler>::iterator iter = mAllHttpHandlers.find(url);
	if(iter != mAllHttpHandlers.end())
	{
		mAllHttpHandlers[url](sock, data, size);
	}
	else
	{
		Log::Error("Unknown url: %s.\n", url.c_str());
		DelBuff(&data);
	}
}

void HttpHandle::Hello(int sock, char* data, int size)
{
	//if(url == "/hello")
	//{
	//	uv_write_t *req = new uv_write_t;
	//	char buff[] = "HTTP/1.1 200 OK\r\nContent-Lenght:12\r\nContent-Type:'text/plain'\r\n\r\nHello world.";
	//	uv_buf_t sendBuff = NewUvBuff(77/BUFF_UNIT + BUFF_UNIT);
	//	memcpy(sendBuff, buff, 77);
	//	req->data = (void*)sendBuff;
	//	uv_write( req, (uv_stream_t*)mSock, &sendBuff, 1, OnWriteClose);
	//	return;
	//}
	//else
	//{
	//	uv_write_t *req = new uv_write_t;
	//	char buff[] = "HTTP/1.1 404 Not Found\r\nContent-Lenght:0\r\nContent-Type:'text/plain'\r\n\r\n";
	//	uv_buf_t sendBuff = NewUvBuff(72/BUFF_UNIT + BUFF_UNIT);
	//	memcpy(sendBuff, buff, 72);
	//	req->data = (void*)sendBuff;
	//	uv_write( req, (uv_stream_t*)mSock, &sendBuff, 1, OnWriteClose);
	//	return;
	//}
}

