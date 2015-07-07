#include "comm.h"
#include "write.h"
#include "define.h"
#include "conn.h"
#include "log.h"

void OnWriteClose(uv_write_t *write, int status)
{
	void* conn = write->handle;
	OnWrite(write, status);
	ConnMgr::CloseConn(conn, false);
}

void OnWrite(uv_write_t *write, int status)
{
	if(status < 0)
	{
		Log::Error("Error when write :%s.\n", uv_err_name(status));
		ConnMgr::CloseConn(write->handle, true);
	}

	WriteReq* pWriteReq = (WriteReq*)write;
	pWriteReq->mRef--;
	if(pWriteReq->mRef <= 0)
	{
		DelUvBuff(pWriteReq->mBuff);
		DELETE(pWriteReq);
	}
}


