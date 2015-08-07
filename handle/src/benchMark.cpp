#include "benchMark.h"

#include "comm.h"
#include "packId.h"
#include "conn.h"
#include "threadData.h"
#include "redis.h"
#include "config.h"
#include "strUtil.h"

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

	Config* pConfig = Config::Singleton();
	int charId;
	pRedis->RunCmd(&charId, "hincrby %s %s %d", pConfig->mRedis_Seed.c_str(),
			pConfig->mRedis_SeedChar.c_str(), 1);
	char buff[BUFF_UNIT];
	StrUtil::Format(buff, BUFF_UNIT, "{\"name\":%s, \"pwd\":%s}",
			req.name().c_str(), req.pwd().c_str());
	pRedis->RunCmd("set %s%d %s", pConfig->mRedis_Char.c_str(), charId, buff);

	int packLen = res.ByteSize();
	RefBuff* pRefBuff = new RefBuff(packLen + HEAD_LENGTH, 1);
	PackId::WritePackHead(pRefBuff->mBuff, PackId::BENCHMARK_REG, size);
	res.SerializeToArray(pRefBuff->mBuff + HEAD_LENGTH, packLen);
	ConnMgr::SendToOne(sock, pRefBuff);
}

void Login(int sock, char* data, int size)
{
	ReqLogin req;
	ResReg res;

	req.ParseFromArray(data, size);
	DelBuff(&data);

	ThreadData* pThreadData = (ThreadData*)pthread_getspecific(ThreadData::mThreadKey);
	Redis* pRedis = pThreadData->GetRedis();
	if(pRedis == NULL)
	{
		res.set_status(STATUS_WRONG_ARG);
	}

	Config* pConfig = Config::Singleton();
	string pwd;
	pRedis->RunCmd(pwd, "hget %s%d %s", pConfig->mRedis_Char.c_str(), req.charid());
	if(pwd == "")
	{
		res.set_status(-22);
	}
	else if(pwd != req.pwd())
	{
		res.set_status(-21);
	}
	else
	{
		res.set_status(STATUS_SUCCESS);
	}
	
	int packLen = res.ByteSize();
	RefBuff* pRefBuff = new RefBuff(packLen + HEAD_LENGTH, 1);
	PackId::WritePackHead(pRefBuff->mBuff, PackId::BENCHMARK_LOGIN, size);
	res.SerializeToArray(pRefBuff->mBuff + HEAD_LENGTH, packLen);
	ConnMgr::SendToOne(sock, pRefBuff);
}

void Move(int sock, char* data, int size)
{
	ReqMove req;
	//res.SerializeToArray();
	req.ParseFromArray(data, size);
	DelBuff(&data);
}

}//end of name space

