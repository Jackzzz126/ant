#include "benchMark.h"

#include "comm.h"
#include "write.h"

namespace BenchMark
{//begin of name space

void Echo(void* conn, char* data, int size)
{
	WriteReq* pWriteReq = new WriteReq();

	pWriteReq->mBuff = NewUvBuff(size + HEAD_LENGTH);
	memcpy(pWriteReq->mBuff.base + HEAD_LENGTH, data, size);

	uv_write(&pWriteReq->mWrite, (uv_stream_t*)conn, &pWriteReq->mBuff, 1, OnWrite);
	DelBuff(&data);

	//*(int*)(sendBuff.base) = size ^ 0x79669966;
	//*((int*)(sendBuff.base) + 1) = 12 ^ 0x79669966;
}
void DoubleEcho(void* conn, char* data, int size)
{
}

}//end of name space

