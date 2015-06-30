#include "comm.h"
#include "Log.h"
#include "Define.h"
#include "Time.h"
#include "Config.h"
#include "Conn.h"
#include "Gate.h"
#include "Msg.h"

uv_loop_t* loop = NULL;
Config* gConfig = NULL;
MsgQueue* gMsgQueue = NULL;

void alloc_buffer_cb(uv_handle_t *handle, size_t suggested_size, uv_buf_t* buff)
{
	//suggested_size always be 65536
	Conn* connObj = ConnMgr::mAllConns[handle];
	buff->base = connObj->mRecvBuff.base + connObj->mValidSize;
	buff->len = connObj->mRecvBuff.len - connObj->mValidSize;
}

void on_read(uv_stream_t *conn, ssize_t nread, const uv_buf_t* pRecvBuff)
{
	//pRecvBuff: same buff in cb
	if (nread < 0)
	{
		if (nread != UV_EOF)
		{
			Log::Error("Error when read :%s.\n", uv_err_name(nread));
			ConnMgr::CloseConn(conn, true);
		}
		else
		{
			//EOF means close;
			ConnMgr::CloseConn(conn, false);
		}
		return;
	}

	map<void*, Conn*>::iterator iter;
	iter = ConnMgr::mAllConns.find(conn);
	if(iter != ConnMgr::mAllConns.end())
	{
		iter->second->RecvData(pRecvBuff->base, nread);
	}
	else
	{
		Log::Error("Read data from unknown conn.\n");
		uv_close((uv_handle_t*)conn, NULL);
	}
}
void on_new_connection(uv_stream_t *listener, int status)
{
	if (status < 0)
	{
		Log::Error("Error when connect: %s.\n", uv_err_name(status));
		return;
	}

	uv_tcp_t *conn = new uv_tcp_t;
	uv_tcp_init(loop, conn);
	if (uv_accept(listener, (uv_stream_t*)conn) == 0)
	{
		assert(conn->type == UV_TCP || conn->type == UV_NAMED_PIPE ||
      conn->type == UV_TTY);
		string clientAddr = GetClientAddr((uv_stream_t*)conn);
		Conn* connObj = new Conn((uv_stream_t*)conn, clientAddr);
		ConnMgr::mAllConns[conn] = connObj;
		uv_read_start((uv_stream_t*)conn, alloc_buffer_cb, on_read);
	}
	else
	{
		uv_close((uv_handle_t*) conn, NULL);
	}
}

//void console(void* arg)
//{
//	char buff[256];
//	while(true)
//	{
//		if(gets(buff))
//		{
//			printf("error when read from console.\n");
//			break;
//		}
//		if(strcmp(buff, "exit")  == 0)
//		{
//			printf("quit console...\n");
//			break;
//		}
//		else if(strcmp(buff, "time")  == 0)
//		{
//			string timeStr = Time::GetTimeStr();
//			printf("%s\n", timeStr.c_str());
//		}
//		else if(strcmp(buff, "users")  == 0)
//		{
//			for(map<void*, Conn*>::iterator iter = ConnMgr::mAllConns.begin(); iter != ConnMgr::mAllConns.end(); iter++)
//			{
//				printf("%s\n", iter->second->mAddr.c_str());
//			}
//		}
//		else
//		{
//			printf("unknown command.\n");
//		}
//	}
//}

int main(int argc, char* argv[])
{
	//config
	gConfig = new Config();
	char configFileName[] = "config.json";
	if(!gConfig->Load(configFileName))
	{
		return 1;
	}
	if(gConfig->mDaemon)
	{
		if(daemon(1,0)) 
		{
			printf("Daemonize error.\n");
			return 1;
		}
	}

	gMsgQueue = new MsgQueue();

	//socket
	loop = uv_default_loop();

	uv_tcp_t listener;
	uv_tcp_init(loop, &listener);

	struct sockaddr_in bind_addr;
	uv_ip4_addr(gConfig->mIp.c_str(), gConfig->mPort, &bind_addr);
	uv_tcp_bind(&listener, (sockaddr*)&bind_addr, 0);
	int r = uv_listen((uv_stream_t*) &listener, gConfig->mBacklog, on_new_connection);
	if(r < 0)
	{
		Log::Error("Error when listen at%s:%d: %s.\n", gConfig->mIp.c_str(),
			gConfig->mPort, uv_err_name(r));
		return 1;
	}
	else
	{
		Log::Out("Server start at %s:%d.\n", gConfig->mIp.c_str(), gConfig->mPort);
	}

	//thread
	//uv_thread_t consoleId;
	//uv_thread_create(&consoleId, console, NULL);

	//timer
	//uv_timer_t regTimer;
	//uv_timer_init(loop, &regTimer);
	//uv_timer_start(&regTimer, Gate::Reg, 3000, gConfig->mRegInterval * 1000);

	int rtn = uv_run(loop, UV_RUN_DEFAULT);

	//timer
	//uv_timer_stop(&regTimer);
	//thread
	//uv_thread_join(&consoleId);
	//config
	DELETE(gConfig);

	//user
	for(map<void*, Conn*>::iterator iter = ConnMgr::mAllConns.begin(); iter != ConnMgr::mAllConns.end(); iter++)
	{
		iter->second->Destroy(false);
	}
	ConnMgr::mAllConns.clear();

	return rtn;
}


