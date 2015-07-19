#include "benchMark.h"

#include "comm.h"
#include "packId.h"
#include "conn.h"

namespace Admin
{//begin of name space

void Hello(int sock, char* data, int size)
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

	//if(url == "/hello")
	//{
	//}
	//else
	//{
	//	char buff[] = "HTTP/1.1 404 Not Found\r\nContent-Lenght:0\r\nContent-Type:'text/plain'\r\n\r\n";
	//	RefBuff* refBuff = new RefBuff(sizeof(buff), 1);
	//	ConnMgr::SendToOne(mSock, refBuff);
	//	return;
	//}
}

}//end of name space

