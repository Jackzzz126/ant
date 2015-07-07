#include "comm.h"
#include "write.h"
#include "define.h"
#include "conn.h"
#include "log.h"

extern uv_loop_t *loop;

void OnWriteClose(uv_write_t *req, int status)
{
	OnWrite(req, status);
	ConnMgr::CloseConn(req->handle, false);
}

void OnWrite(uv_write_t *req, int status)
{
	//if (status < 0)
	//{
	//	Log::Error("Error when write :%s.\n", uv_err_name(status));
	//	ConnMgr::CloseConn(req->handle, true);
	//}
	//write_req_t* wr = (write_req_t*) req;;
	//DelBuff(&wr->buf.base);
	//DELETE(wr);
}

void OnWriteNoFree(uv_write_t *req, int status)
{
	//if (status < 0)
	//{
	//	Log::Error("Error when write :%s.\n", uv_err_name(status));
	//	ConnMgr::CloseConn(req->handle, true);
	//}
	//DELETE(req);
}

