#include "benchMark.h"

#include "comm.h"
#include "packId.h"
#include "httpUtil.h"

namespace Admin
{//begin of name space

void Hello(int sock, char* data, int size)
{
	HttpUtil::ResStr(sock, "Hello world.");
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

