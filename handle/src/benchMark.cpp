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
	ConnMgr::SendToOne(conn, PackId::NORMAL_ECHO, buff);

	DelBuff(&data);
}
void DoubleEcho(void* conn, char* data, int size)
{
	uv_buf_t buff = NewUvBuff(size + HEAD_LENGTH);
	memcpy(buff.base + HEAD_LENGTH, data, size);
	vector<void*> conns;
	conns.push_back(conn);
	conns.push_back(conn);
	ConnMgr::SendToMulti(conns, PackId::DOUBLE_ECHO, buff);

	DelBuff(&data);

}

}//end of name space

