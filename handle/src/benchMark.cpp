#include "benchMark.h"

#include "comm.h"
#include "packId.h"
#include "conn.h"

namespace BenchMark
{//begin of name space

void Echo(int sock, char* data, int size)
{
	RefBuff* pRefBuff = new RefBuff(size + HEAD_LENGTH, 1);
	PackId::WritePackHead(pRefBuff->mBuff, PackId::NORMAL_ECHO, size);
	memcpy(pRefBuff->mBuff + HEAD_LENGTH, data, size);
	ConnMgr::SendToOne(sock, pRefBuff);
	DelBuff(&data);
}
void DoubleEcho(int sock, char* data, int size)
{
	RefBuff* pRefBuff = new RefBuff(size + HEAD_LENGTH, 1);
	PackId::WritePackHead(pRefBuff->mBuff, PackId::DOUBLE_ECHO, size * 2);
	memcpy(pRefBuff->mBuff + HEAD_LENGTH, data, size);
	memcpy(pRefBuff->mBuff + HEAD_LENGTH + size, data, size);
	ConnMgr::SendToOne(sock, pRefBuff);
	DelBuff(&data);
}

}//end of name space

