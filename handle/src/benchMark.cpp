#include "benchMark.h"

#include "comm.h"
#include "packId.h"
#include "conn.h"
#include "threadData.h"
#include "redis.h"
#include "config.h"
#include "strUtil.h"
#include "json.h"

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
		Log::Error("Error when conn to reids.");
		return ConnMgr::ErrorEnd(sock);
	}
	char buff[BUFF_UNIT];
	Config* pConfig = Config::Singleton();


	int charId;
	StrUtil::Format(buff, BUFF_UNIT, "incrby %s 1",
			pConfig->mRedis_CharIdSeed.c_str());
	if(!pRedis->RunCmd(&charId, buff))
	{
		Log::Error("Error when get new charId.");
		return ConnMgr::ErrorEnd(sock);
	}

	Json json;
	json.SetValue(NULL, "name", req.name());
	json.SetValue(NULL, "pwd", req.pwd());
	StrUtil::Format(buff, BUFF_UNIT, "set %s%d %s",
			pConfig->mRedis_Char.c_str(), charId, json.ToStr());
	if(!pRedis->RunCmd(buff))
	{
		Log::Error("Error when add char.");
		return ConnMgr::ErrorEnd(sock);
	}

	res.set_status(STATUS_SUCCESS);
	int packLen = res.ByteSize();
	RefBuff* pRefBuff = new RefBuff(packLen + HEAD_LENGTH, 1);
	PackId::WritePackHead(pRefBuff->mBuff, PackId::BENCHMARK_REG, packLen);
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
		Log::Error("Conn to redis error.");
		ConnMgr::ErrorEnd(sock);
	}
	char buff[BUFF_UNIT];
	Config* pConfig = Config::Singleton();

	string pwd;
	StrUtil::Format(buff, BUFF_UNIT, "hget %s%d %s",
			pConfig->mRedis_Char.c_str(), req.charid());
	pRedis->RunCmd(pwd, buff);

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
	PackId::WritePackHead(pRefBuff->mBuff, PackId::BENCHMARK_LOGIN, packLen);
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

