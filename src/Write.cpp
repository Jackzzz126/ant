#include "comm.h"
#include "Write.h"
#include "Define.h"
#include "Conn.h"
#include "Log.h"

extern uv_loop_t *loop;

void OnWriteClose(uv_write_t *req, int status)
{
	OnWrite(req, status);
	ConnMgr::CloseConn(req->handle, Conn::SERVER_CLOSE);
}

void OnWrite(uv_write_t *req, int status)
{
	if (status < 0)
	{
		Log::Error("Error when write :%s.\n", uv_err_name(status));
		ConnMgr::CloseConn(req->handle, Conn::SOCK_ERROR);
	}
	write_req_t* wr = (write_req_t*) req;;
	DelBuff(&wr->buf.base);
	DELETE(wr);
}

void OnWriteNoFree(uv_write_t *req, int status)
{
	if (status < 0)
	{
		Log::Error("Error when write :%s.\n", uv_err_name(status));
		ConnMgr::CloseConn(req->handle, Conn::SOCK_ERROR);
	}
	DELETE(req);
}

