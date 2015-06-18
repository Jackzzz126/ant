#include "comm.h"
#include "Log.h"
#include "Define.h"
#include "Time.h"
#include "Config.h"
#include "Client.h"
#include "Gate.h"

uv_loop_t* loop = NULL;
Config* gConfig = NULL;

void alloc_buffer_cb(uv_handle_t *handle, size_t suggested_size, uv_buf_t* buff)
{
	buff->base = ClientMgr::mAllClients[handle]->mReceiveBuff.base;
	buff->len = ClientMgr::mAllClients[handle]->mReceiveBuff.len;
}

void on_read(uv_stream_t *conn, ssize_t nread, const uv_buf_t* pRecvBuff)
{
	if (nread < 0)
	{
		if (nread != UV_EOF)
		{
			Log::Error("Error when read :%s.\n", uv_err_name(nread));
			ClientMgr::CloseClient(conn, Client::SOCK_ERROR);
		}
		else
		{
			//EOF means close;
			ClientMgr::CloseClient(conn, Client::CLIENT_CLOSE);
		}
		return;
	}

	string clientAddr = GetClientAddr(conn);
	if( nread < 4 )
	{
		ClientMgr::CloseClient(conn, Client::UNKNOWN_DATA);
	}
	map<void*, Client*>::iterator iter;
	iter = ClientMgr::mAllClients.find(conn);
	if(iter != ClientMgr::mAllClients.end())
	{
		iter->second->ReceiveData(pRecvBuff->base, nread);
	}
	else
	{
		Log::Error("Unknown client.\n");
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
	if (uv_accept(listener, (uv_stream_t*) conn) == 0)
	{
		string clientAddr = GetClientAddr((uv_stream_t*)conn);
		Client* user = new Client((uv_stream_t*)conn, clientAddr);
		ClientMgr::mAllClients[conn] = user;
		uv_read_start((uv_stream_t*)conn, alloc_buffer_cb, on_read);
	}
	else
	{
		uv_close((uv_handle_t*) conn, NULL);
	}
}

//void console(void* arg )
//{
//	char buff[256];
//	while(true)
//	{
//		if(gets(buff))
//		{
//			printf( "error when read from console.\n" );
//			break;
//		}
//		if(strcmp( buff, "exit" )  == 0)
//		{
//			printf( "quit console...\n" );
//			break;
//		}
//		else if(strcmp( buff, "time" )  == 0)
//		{
//			string timeStr = Time::GetTimeStr();
//			printf("%s\n", timeStr.c_str());
//		}
//		else if(strcmp( buff, "users" )  == 0)
//		{
//			for(map<void*, Client*>::iterator iter = ClientMgr::mAllClients.begin(); iter != ClientMgr::mAllClients.end(); iter++)
//			{
//				printf( "%s\n", iter->second->mAddr.c_str());
//			}
//		}
//		else
//		{
//			printf( "unknown command.\n" );
//		}
//	}
//}

int main( int argc, char* argv[] )
{
	//config
	gConfig = new Config();
	char configFileName[] = "config.json";
	if(!gConfig->Load(configFileName))
	{
		Log::Error("Error when parse config file.\n");
		return 1;
	}

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
		Log::Error( "Error when listen at%s:%d: %s.\n", gConfig->mIp.c_str(), gConfig->mPort,
					uv_err_name(r));
		return 1;
	}
	else
	{
		string timeStr = Time::GetTimeStr();
		Log::Out("Server start at %s:%d %s\n", gConfig->mIp.c_str(), gConfig->mPort, timeStr.c_str());
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
	for(map<void*, Client*>::iterator iter = ClientMgr::mAllClients.begin(); iter != ClientMgr::mAllClients.end(); iter++)
	{
		iter->second->Destroy(Client::SERVER_DOWN);
	}
	ClientMgr::mAllClients.clear();

	return rtn;
}


