#include "comm.h"
#include "HttpReq.h"
#include "Proto.h"
#include "Log.h"
#include "Define.h"

vector<HttpConn> gHttpConns;

HttpReq::HttpReq()
{
}
HttpReq::~HttpReq()
{
}

void HttpReq::ClearConn(uv_stream_t* handle)
{
	vector<HttpConn>::iterator iter = gHttpConns.begin();
	for(; iter != gHttpConns.end(); iter++)
	{
		if(iter->mConn->handle == handle)
		{
			delete iter->mSocket;
			delete iter->mConn;
			gHttpConns.erase(iter);
			break;
		}
	}
}

uv_buf_t HttpReq::OnAllocBuffer(uv_handle_t *handle, size_t suggested_size)
{
	uv_buf_t receiveBuff;
	receiveBuff.base = NewBuff(Proto::BUFF_UNIT * 4);
	receiveBuff.len = Proto::BUFF_UNIT * 4;
	return receiveBuff;
}

void HttpReq::SendHttpPack(uv_connect_t* conn, const string& url, uv_buf_t buff)
{
	string httpHead;
	httpHead += ("POST " + url + " HTTP/1.1\r\n");
	httpHead += ("Content-Length: " + String::Format("%d", buff.len) + "\r\n");
	//"Host: 192.168.61.3:8079"
	httpHead += "Connection: close\r\n";
	httpHead += "\r\n";
	
	uv_buf_t httpBuff;
	httpBuff.base = NewBuff((httpHead.length() + buff.len + Proto::BUFF_UNIT) / Proto::BUFF_UNIT * Proto::BUFF_UNIT);
	memcpy(httpBuff.base, httpHead.c_str(), httpHead.length());
	memcpy((char*)httpBuff.base + httpHead.length(), buff.base, buff.len);
	httpBuff.len = httpHead.length() + buff.len;
	DelBuff(&buff.base);
	buff.base = NULL;
	buff.len = 0;


	uv_write_t *req = new uv_write_t;
	req->data = httpBuff.base;
	uv_write( req, conn->handle, &httpBuff, 1, OnWriteHttp);
}

void HttpReq::OnWriteHttp(uv_write_t *req, int status)
{
	if (status == -1)
	{
		Log::Out("Write http error.\n");
		uv_close((uv_handle_t*)req->handle, NULL);
		ClearConn(req->handle);
	}
	DelBuff((char**)&req->data);
	DELETE(req);
}
