#include "comm.h"
#include "log.h"
#include "define.h"
#include "time.h"
#include "config.h"
#include "conn.h"
#include "gate.h"
#include "msg.h"
#include "router.h"

uv_loop_t* loop = NULL;

void alloc_buffer_cb(uv_handle_t *handle, size_t suggested_size, uv_buf_t* buff);
void on_read(uv_stream_t *conn, ssize_t nread, const uv_buf_t* pRecvBuff);
void on_new_connection(uv_stream_t *listener, int status);

void signal_term(uv_signal_t *handle, int signum);
void signal_int(uv_signal_t *handle, int signum);

int main(int argc, char* argv[])
{
	//config
	Config* config = Config::Singleton();
	char configFileName[] = "config/config.json";
	if(!config->Load(configFileName))
	{
		return 1;
	}
	if(config->mDaemon)
	{
		if(daemon(1,0)) 
		{
			printf("Daemonize error.\n");
			return 1;
		}
	}
	Router::Init();

	//socket
	loop = uv_default_loop();

	uv_tcp_t listener;
	uv_tcp_init(loop, &listener);

	struct sockaddr_in bind_addr;
	uv_ip4_addr(config->mIp.c_str(), config->mPort, &bind_addr);
	uv_tcp_bind(&listener, (sockaddr*)&bind_addr, 0);
	int r = uv_listen((uv_stream_t*) &listener, config->mBacklog, on_new_connection);
	if(r < 0)
	{
		Log::Error("Error when listen at%s:%d: %s.\n", config->mIp.c_str(),
			config->mPort, uv_err_name(r));
		return 1;
	}
	else
	{
		Log::Out("Server start at %s:%d.\n", config->mIp.c_str(), config->mPort);
	}

	//thread
	//uv_thread_t consoleId;
	//uv_thread_create(&consoleId, console, NULL);

	//timer
	//uv_timer_t regTimer;
	//uv_timer_init(loop, &regTimer);
	//uv_timer_start(&regTimer, Gate::Reg, 3000, config->mRegInterval * 1000);

	//signal handle, normal way can't stop uv properly.
	uv_signal_t sigTerm;
	uv_signal_init(loop, &sigTerm);
	uv_signal_start(&sigTerm, signal_term, SIGTERM);

	uv_signal_t sigInt;
	uv_signal_init(loop, &sigInt);
	uv_signal_start(&sigInt, signal_int, SIGINT);


	int rtn = uv_run(loop, UV_RUN_DEFAULT);

	//timer
	//uv_timer_stop(&regTimer);
	//thread
	//uv_thread_join(&consoleId);

	//user
	for(map<void*, Conn*>::iterator iter = ConnMgr::mAllConns.begin(); iter != ConnMgr::mAllConns.end(); iter++)
	{
		iter->second->Destroy(false);
	}
	ConnMgr::mAllConns.clear();

	return rtn;
}

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
		Conn* connObj = new Conn(conn);
		ConnMgr::mAllConns[conn] = connObj;
		uv_read_start((uv_stream_t*)conn, alloc_buffer_cb, on_read);
	}
	else
	{
		uv_close((uv_handle_t*) conn, NULL);
	}
}

void signal_term(uv_signal_t *handle, int signum)
{
	uv_stop(loop);
	uv_signal_stop(handle);
}
void signal_int(uv_signal_t *handle, int signum)
{
	uv_stop(loop);
	uv_signal_stop(handle);
}


