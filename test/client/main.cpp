#include "comm.h"
#include "Log.h"
#include "Define.h"
#include "Time.h"
#include "Config.h"
#include "Proto.h"
#include "Json.h"

//#include "ReqTest.h"

#define BUFF_SIZE 1024

uv_loop_t* loop = NULL;
Config* gConfig = NULL;

uv_connect_t *conn;
uv_buf_t gRecevBuff;

void alloc_buffer_cb(uv_handle_t *handle, size_t suggested_size, uv_buf_t* buff)
{
	buff->base = new char[BUFF_SIZE];
	memset(buff->base, 0, BUFF_SIZE);
	buff->len = BUFF_SIZE;
}
void on_read(uv_stream_t *conn, ssize_t nread, const uv_buf_t* pRecvBuff)
{
	if (nread < 0)
	{
		if (nread != UV_EOF)
		{
			Log::Error("Error when read :%s.\n", uv_err_name(nread));
		}
	}

	string clientAddr = GetClientAddr(conn);
	Log::Debug("%s	received data.\n", clientAddr.c_str());
	
	delete [](char*)pRecvBuff->base;
}
void on_write(uv_write_t *req, int status)
{
	if (status == -1)
	{
		printf("Write error %s\n", uv_err_name(status));
	}
	delete [](char*)(req->data);
	DELETE(req);
}
void on_connect(uv_connect_t* req, int status)
{
	if(status == -1)
	{
		printf("connect to server faile.\n");
	}
	else
	{
		printf("connect to server successfully.\n");
		uv_read_start(req->handle, alloc_buffer_cb, on_read);
	}
}
//void console(void* arg )
//{
//	char buff[256];
//	while(true)
//	{
//		if(!gets(buff))
//		{
//			printf( "error when read from console.\n" );
//			break;
//		}
//		if(strcmp( buff, "send http" )  == 0)
//		{
//			char buff[] = "GET ";
//			uv_buf_t sendBuff = NewBuffer(4);
//			memcpy(sendBuff.base, buff, 4);
//			sendBuff.len = 4;
//
//			uv_write_t *req = new uv_write_t;
//			req->data = (void*)sendBuff.base;
//			uv_write( req, conn->handle, &sendBuff, 1, on_write);
//		}
//		/*else if(strcmp( buff, "send reg" )  == 0)
//		{
//			ReqReg reg;
//			reg.m_mailAddr = "aaaaa";
//			reg.m_pwdMd5 = "12345";
//			reg.m_signature = "bbbbb";
//			uv_buf_t sendBuff;
//			reg.Write(sendBuff);
//
//			uv_write_t *req = new uv_write_t;
//			req->data = sendBuff.base;
//			uv_write( req, conn->handle, &sendBuff, 1, on_write);
//		}*/
//		else if(strcmp( buff, "send test" )  == 0)
//		{
//			ReqTest reg;
//			reg.m_buff.mBuff = new char[10];
//			reg.m_buff.mBuff[0] = '1';
//			reg.m_buff.mBuff[1] = '2';
//			reg.m_buff.mBuff[2] = '3';
//			reg.m_buff.mBuff[3] = '4';
//			reg.m_buff.mBuff[4] = '\0';
//
//			reg.m_buff.mBuff[5] = '5';
//			reg.m_buff.mBuff[6] = '6';
//			reg.m_buff.mBuff[7] = '7';
//			reg.m_buff.mBuff[8] = '8';
//			reg.m_buff.mBuff[9] = '\0';
//			reg.m_buff.mSize = 10;
//
//			reg.m_array1.push_back(1);
//			reg.m_array1.push_back(2);
//			reg.m_array1.push_back(3);
//			reg.m_array1.push_back(4);
//			reg.m_array1.push_back(5);
//
//			reg.m_array2.push_back(1.0f);
//			reg.m_array2.push_back(2.0f);
//			reg.m_array2.push_back(3.0f);
//			reg.m_array2.push_back(4.0f);
//			reg.m_array2.push_back(5.0f);
//
//			uv_buf_t sendBuff;
//			reg.Write(sendBuff);
//
//			uv_write_t *req = new uv_write_t;
//			req->data = sendBuff.base;
//			uv_write( req, conn->handle, &sendBuff, 1, on_write);
//		}
//		else
//		{
//			printf( "	unkown command.\n" );
//		}
//	}
//}

int main(int argc, char* argv[])
{
	loop = uv_default_loop();

	uv_tcp_t socket;
	uv_tcp_init(loop, &socket);

	conn = new uv_connect_t;
	struct sockaddr_in server_addr;
	uv_ip4_addr("10.10.10.139", 8000, &server_addr);
	uv_tcp_connect( conn, &socket, (sockaddr*)&server_addr, on_connect );

	//uv_thread_t consoleId;
	//uv_thread_create(&consoleId, console, NULL);

	int rtn = uv_run(loop, UV_RUN_DEFAULT);
	//uv_thread_join(&consoleId);
	return rtn;
}


