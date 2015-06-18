#include "comm.h"
#include "Write.h"
#include "Define.h"
#include "Client.h"
#include "Log.h"

extern uv_loop_t *loop;

void OnWriteClose(uv_write_t *req, int status)
{
	OnWrite(req, status);
	ClientMgr::CloseClient(req->handle, Client::SERVER_CLOSE);
}

void OnWrite(uv_write_t *req, int status)
{
	if (status < 0)
	{
		Log::Error("Error when write :%s.\n", uv_err_name(status));
		ClientMgr::CloseClient(req->handle, Client::SOCK_ERROR);
	}
	write_req_t* wr = (write_req_t*) req;;
	delete [](char*)(wr->buf.base);
	DELETE(wr);
}

void OnWriteNoFree(uv_write_t *req, int status)
{
	if (status < 0)
	{
		Log::Error("Error when write :%s.\n", uv_err_name(status));
		ClientMgr::CloseClient(req->handle, Client::SOCK_ERROR);
	}
	DELETE(req);
}

