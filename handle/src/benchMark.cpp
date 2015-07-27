#include "benchMark.h"

#include "comm.h"
#include "packId.h"
#include "conn.h"
#include "threadData.h"
#include "redis.h"

#include "benchMark.pb.h"

namespace BenchMark
{//begin of name space

void Echo(int sock, char* data, int size)
{
	RefBuff* pRefBuff = new RefBuff(size + HEAD_LENGTH, 1);
	PackId::WritePackHead(pRefBuff->mBuff, PackId::BENCHMARK_ECHO, size);
	memcpy(pRefBuff->mBuff + HEAD_LENGTH, data, size);
	ConnMgr::SendToOne(sock, pRefBuff);
	DelBuff(&data);
}
void DoubleEcho(int sock, char* data, int size)
{
	RefBuff* pRefBuff = new RefBuff(size + HEAD_LENGTH, 1);
	PackId::WritePackHead(pRefBuff->mBuff, PackId::BENCHMARK_DOUBLE_ECHO, size * 2);
	memcpy(pRefBuff->mBuff + HEAD_LENGTH, data, size);
	memcpy(pRefBuff->mBuff + HEAD_LENGTH + size, data, size);
	ConnMgr::SendToOne(sock, pRefBuff);
	DelBuff(&data);
}

void Reg(int sock, char* data, int size)
{
	ReqReg req;
	ResReg res;

	req.ParseFromArray(data, size);
	DelBuff(&data);

	ThreadData* pThreadData = (ThreadData*)pthread_getspecific(ThreadData::mThreadKey);
	Redis* pRedis = pThreadData->GetRedis();
	if(pRedis == NULL)
	{
		res.set_status(STATUS_WRONG_ARG);
	}

	int packLen = res.ByteSize();
	RefBuff* pRefBuff = new RefBuff(packLen + HEAD_LENGTH, 1);
	PackId::WritePackHead(pRefBuff->mBuff, PackId::BENCHMARK_REG, size);
	res.SerializeToArray(pRefBuff->mBuff + HEAD_LENGTH, packLen);
	ConnMgr::SendToOne(sock, pRefBuff);
	//Log::Out("aaaaaaaaaaaa %s, %s\n", req.name().c_str(), req.pwd().c_str());
}

void Login(int sock, char* data, int size)
{
	ReqLogin req;
	//res.SerializeToArray();
	req.ParseFromArray(data, size);
	DelBuff(&data);
}

void Move(int sock, char* data, int size)
{
	ReqMove req;
	//res.SerializeToArray();
	req.ParseFromArray(data, size);
	DelBuff(&data);
}

}//end of name space

