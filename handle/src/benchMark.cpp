#include "benchMark.h"

#include "comm.h"
#include "packId.h"
#include "conn.h"

namespace BenchMark
{//begin of name space

void Echo(void* conn, char* data, int size)
{
	uv_buf_t buff = NewUvBuff(size + HEAD_LENGTH);
	memcpy(buff.base + HEAD_LENGTH, data, size);
	ConnMgr::SendPack(conn, PackId::NORMAL_ECHO, buff);
	DelBuff(&data);
}
void DoubleEcho(void* conn, char* data, int size)
{
	uv_buf_t buff = NewUvBuff(size * 2 + HEAD_LENGTH);
	memcpy(buff.base + HEAD_LENGTH, data, size);
	memcpy(buff.base + HEAD_LENGTH + size, data, size);
	ConnMgr::SendPack(conn, PackId::DOUBLE_ECHO, buff);
	DelBuff(&data);
}

}//end of name space

