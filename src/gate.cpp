#include "comm.h"
#include "gate.h"
#include "config.h"

//#include "ReqRegCombatServer.h"
//#include "ResRegCombatServer.h"

//extern Config* gConfig;
//extern uv_loop_t *loop;
//extern vector<HttpConn> gHttpConns;
//
//Gate::Gate()
//{
//}
//
//void Gate::Reg(uv_timer_t *handle, int status)
//{
//	//HttpConn conn;
//	//conn.mSocket = new uv_tcp_t;
//	//uv_tcp_init(loop, conn.mSocket);
//
//	//conn.mConn = new uv_connect_t;
//	//struct sockaddr_in server_addr;
//	//uv_ip4_addr(gConfig->mGateIp.c_str(), gConfig->mGatePort, &server_addr);
//	//uv_tcp_connect(conn.mConn, conn.mSocket, (sockaddr*)&server_addr, OnRegConn);
//	//gHttpConns.push_back(conn);
//}
//
//void Gate::OnRegConn(uv_connect_t* conn, int status)
//{
//	//if(status == -1)
//	//{
//	//	Log::Out("connect to gate server faile.\n");
//	//	ClearConn(conn->handle);
//	//}
//	//else
//	//{
//	//	uv_read_start(conn->handle, OnAllocBuffer, OnRegReturn );
//
//	//	ReqRegCombatServer reg;
//
//	//	reg.m_ip = gConfig->mIp;
//	//	reg.m_port = gConfig->mPort;
//
//	//	uv_buf_t sendBuff;
//	//	reg.Write(sendBuff);
//
//	//	SendHttpPack(conn, "/gate/reg-combat-server", sendBuff);
//	//}
//}
//
//void Gate::OnRegReturn(uv_stream_t *handle, ssize_t nread, uv_buf_t recvBuff)
//{
//	//if (nread == -1)
//	//{
//	//	if(uv_last_error(loop).code != UV_EOF)
//	//	{
//	//		Log::Error("Error when reg combat server:%s.\n", uv_err_name(uv_last_error(loop)));
//	//		uv_close((uv_handle_t*)handle, NULL);
//	//		ClearConn(handle);
//	//	}
//	//	else
//	//	{
//	//		Log::Error("Error when reg combat server:lost conn.\n");
//	//		ClearConn(handle);
//	//	}
//	//	return;
//	//}
//
//	//string httpHead = string(recvBuff.base);
//	//uint httpHeadPos = httpHead.find("\r\n\r\n") + 4;
//	//httpHead = httpHead.substr(0, httpHeadPos);
//	//if(httpHead.substr(0, 12) != "HTTP/1.1 200")
//	//{
//	//	Log::Out("reg combat server faile.\n");
//	//	DelBuff(&recvBuff.base);
//	//}
//	//else
//	//{
//	//	ResRegCombatServer res;
//	//	char* data = (char*)recvBuff.base + httpHeadPos;
//	//	res.InitBuff(data, *((int*)data + 1) ^ 0x79669966);
//	//	if(!res.Parse())
//	//	{
//	//		Log::Out("unkown return when reg combat server.\n");
//	//	}
//	//	else
//	//	{
//	//		if(res.m_status < 0)
//	//		{
//	//			Log::Out("error when reg combat server.\n");
//	//		}
//	//	}
//	//	DelBuff(&recvBuff.base);
//	//}
//}


