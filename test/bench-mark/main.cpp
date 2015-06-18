#include "comm.h"
#include "Log.h"
#include "Define.h"
#include "Time.h"
#include "Config.h"
#include "Proto.h"
#include "Json.h"

#include "ReqBenchMark.h"
#include "Client.h"

#include <unistd.h>

#define BUFF_SIZE 1024 * 10
int gConnNum = 0;
int gPackNumPerSec = 0;

uv_loop_t* loop = NULL;
Config* gConfig = NULL;

int gPackId = 12;

int maxTime = 0;
int minTime = 3000;
int avgTime = 0;

int gPackSendCount = 0;
int gPackRecvCount = 0;
int64 gTime = 0;

vector<uv_stream_t*> gConns;

struct RecvBuff//pack handle
{
	char mBuff[BUFF_SIZE];
	int mSize;
	RecvBuff()
	{
		mSize = 0;
	}
	void AddData(char* buff, int size)
	{
		ASSERT((mSize + size) <= BUFF_SIZE);
		memcpy(mBuff + mSize, buff, size);
		mSize += size;
		while(mSize > 8)
		{
			int packLength = *(int*)(mBuff) ^ 0x79669966;
			//int packId = *((int*)(mBuff) + 1) ^ 0x79669966;
			//pack parse
			string str = string(mBuff + 8, packLength);
			Json jsonObj = Json();
			jsonObj.Parse(str);
			int time;
			jsonObj.GetValue("time", &time);
			int timeDelta = Time::GetProcTime() - time;
			//printf("%d\n", timeDelta);
			
			//statistics
			gPackRecvCount++;
			gTime += timeDelta;
			if(timeDelta < minTime)
			{
				minTime = timeDelta;
			}
			if(timeDelta > maxTime)
			{
				maxTime = timeDelta;
			}

			//buff pointer move
			if(mSize > (packLength + 8))
			{
				memcpy(mBuff, mBuff + packLength + 8, mSize - packLength - 8);
				mSize -= packLength;
				mSize -= 8;
			}
			else
			{
				mSize = 0;
			}
		}
	}
};
map<uv_stream_t*, RecvBuff*> gRecvBuffs;


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
		delete [](char*)pRecvBuff->base;
		return;
	}

	gRecvBuffs[conn]->AddData(pRecvBuff->base, nread);
	delete [](char*)pRecvBuff->base;
}
void on_write(uv_write_t *req, int status)
{
	if (status < 0)
	{
		printf("Write error %s\n", uv_err_name(status));
	}
	delete [](char*)(req->data);
	DELETE(req);
}
void send(uv_timer_t* handle)
{
	if((int)gConns.size() != gConnNum)
		return;//connect not finished
	for(uint i = 0; i < gConns.size(); i++)
	{
		string str = String::Format("%s%d%s", "{\"time\": ", Time::GetProcTime(), ", test: \"aaaaaa, bbbbbbb, ccccccc, dddddddddddd, eeeeeeeeeeeeeee, ffffffffffffffff, gggggggggggggggggg\" }");
		uv_write_t *req = new uv_write_t;
		uv_buf_t sendBuff = NewBuffer(BUFF_SIZE);
		sendBuff.len = str.length() + Client::HEAD_LENGTH;
		memcpy(sendBuff.base + Client::HEAD_LENGTH, str.c_str(), str.length());
		*(int*)(sendBuff.base) = str.length() ^ 0x79669966;
		*((int*)(sendBuff.base) + 1) = gPackId ^ 0x79669966;
		req->data = (void*)sendBuff.base;
		uv_write( req, gConns[i], &sendBuff, 1, on_write);

		gPackSendCount++;
	}
}
void count(uv_timer_t* handle)
{
	if(gPackRecvCount != 0)
	{
		avgTime = gTime / gPackRecvCount;
		printf("send/recv:%d/%d	min/max/avg/:%d/%d/%d\n", gPackSendCount, gPackRecvCount,
				minTime, maxTime, avgTime);
		
		gPackSendCount = 0;
		gPackRecvCount = 0;
		gTime = 0;
		
		minTime = 3000;
		maxTime = 0;
		avgTime = 0;
	}
}

void on_connect(uv_connect_t* req, int status)
{
	if(status == -1)
	{
		printf("connect to server faile.\n");
	}
	else
	{
		gConns.push_back(req->handle);
		RecvBuff* buff = new RecvBuff();
		gRecvBuffs[req->handle] = buff;
		uv_read_start(req->handle, alloc_buffer_cb, on_read );
	}
}

int main( int argc, char* argv[] )
{
	if(argc < 3)
	{
		printf("usage:\ncmd connNum, packs/s.\n");
		return 0;
	}
	gConnNum = atoi(argv[1]);
	gPackNumPerSec = atoi(argv[2]);
	if(gConnNum == 0 || gPackNumPerSec == 0)
	{
		printf("wrong args.\n");
		return 0;
	}

	loop = uv_default_loop();

	struct sockaddr_in server_addr;
	uv_ip4_addr("192.168.1.230", 8100, &server_addr);
	for(int i = 0; i < gConnNum; i++)
	{
		uv_tcp_t* socket = new uv_tcp_t;
		uv_tcp_init(loop, socket);

		uv_connect_t* conn = new uv_connect_t;
		uv_tcp_connect( conn, socket, (sockaddr*)&server_addr, on_connect );
	}

	//timer
	uv_timer_t countTimer;
	uv_timer_init(loop, &countTimer);
	uv_timer_start(&countTimer, count, 3000, 1000);

	uv_timer_t sendTimer;
	uv_timer_init(loop, &sendTimer);
	uv_timer_start(&sendTimer, send, 3000, 1000 / gPackNumPerSec);

	int rtn = uv_run(loop, UV_RUN_DEFAULT);
	return rtn;
}


