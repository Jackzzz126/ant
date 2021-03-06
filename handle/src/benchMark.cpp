#include "benchMark.h"

#include "comm.h"
#include "packId.h"
#include "conn.h"
#include "threadData.h"
#include "redis.h"
#include "config.h"
#include "strUtil.h"
#include "json.h"
#include "char.h"
#include "msg.h"
#include "refBuff.h"
#include "udpClient.h"

#include "benchMark.pb.h"

namespace BenchMark
{//begin of name space

void Echo(int sock, int msgId, char* data, int size)
{
	RefBuff* pRefBuff = new RefBuff(size + HEAD_LENGTH, 1);
	PackId::WritePackHead(pRefBuff->mBuff, PackId::BENCHMARK_ECHO, size);
	memcpy(pRefBuff->mBuff + HEAD_LENGTH, data, size);
	ConnMgr::SendToSock(sock, pRefBuff);
	DelBuff(&data);
}
void DoubleEcho(int sock, int msgId, char* data, int size)
{
	RefBuff* pRefBuff = new RefBuff(size + HEAD_LENGTH, 2);
	PackId::WritePackHead(pRefBuff->mBuff, PackId::BENCHMARK_DOUBLE_ECHO, size);
	memcpy(pRefBuff->mBuff + HEAD_LENGTH, data, size);
	ConnMgr::SendToSock(sock, pRefBuff);
	ConnMgr::SendToSock(sock, pRefBuff);
	DelBuff(&data);
}

void Reg(int sock, int msgId, char* data, int size)
{
	DbMsgQueue::Singleton()->PushMsg(sock, msgId, data, size);
}
void RegDb(int sock, int msgId, char* data, int size)
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


	//int charId;
	//StrUtil::Format(buff, BUFF_UNIT, "incrby %s 1",
	//		pConfig->mRedis_CharIdSeed.c_str());
	//if(!pRedis->RunCmd(&charId, buff))
	//{
	//	Log::Error("Error when get new charId.");
	//	return ConnMgr::ErrorEnd(sock);
	//}

	Json json;
	json.SetValue(NULL, "name", req.name());
	json.SetValue(NULL, "pwd", req.pwd());
	StrUtil::Format(buff, BUFF_UNIT, "set %s%d %s",
			pConfig->mRedis_Char.c_str(), req.charid(), json.ToStr());
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
	ConnMgr::SendToSock(sock, pRefBuff);
}

void Login(int sock, int msgId, char* data, int size)
{
	DbMsgQueue::Singleton()->PushMsg(sock, msgId, data, size);
}
void LoginDb(int sock, int msgId, char* data, int size)
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

	string charStr;
	StrUtil::Format(buff, BUFF_UNIT, "get %s%d",
			pConfig->mRedis_Char.c_str(), req.charid());
	pRedis->RunCmd(charStr, buff);

	if(charStr == "")
	{
		res.set_status(-22);
	}
	else
	{
		Json json;
		json.Parse(charStr);
		string pwd;
		json.GetValue("pwd", pwd);
		if(pwd != req.pwd())
		{
			res.set_status(-21);
		}
		else
		{
			Char* pChar = new Char(req.charid(), sock);
			CharMgr* pCharMgr = CharMgr::Singleton();
			pCharMgr->AddChar(req.charid(), pChar);
			res.set_status(STATUS_SUCCESS);
		}
	}
	
	int packLen = res.ByteSize();
	RefBuff* pRefBuff = new RefBuff(packLen + HEAD_LENGTH, 1);
	PackId::WritePackHead(pRefBuff->mBuff, PackId::BENCHMARK_LOGIN, packLen);
	res.SerializeToArray(pRefBuff->mBuff + HEAD_LENGTH, packLen);
	ConnMgr::SendToSock(sock, pRefBuff);
}

void Move(int sock, int msgId, char* data, int size)
{
	ReqMove req;
	ResMove res;
	req.ParseFromArray(data, size);
	DelBuff(&data);

	int charId = req.charid();//send to xx1 x10
	int beginId;
	if(charId % 10 == 0)
	{
		beginId = charId - 10;
	}
	else
	{
		beginId = charId - charId % 10;
	}

	vector<int> chars;
	for(int i = 0; i < 10; i++)
	{
		chars.push_back(beginId + i + 1);
	}

	res.set_charid(req.charid());
	res.set_x(req.x());
	res.set_y(req.y());
	res.set_time(req.time());

	int packLen = res.ByteSize();
	RefBuff* pRefBuff = new RefBuff(packLen + HEAD_LENGTH, 10);
	PackId::WritePackHead(pRefBuff->mBuff, PackId::BENCHMARK_MOVE, packLen);
	res.SerializeToArray(pRefBuff->mBuff + HEAD_LENGTH, packLen);
	CharMgr* pCharMgr = CharMgr::Singleton();
	pCharMgr->SendToChars(chars, pRefBuff);
}

void UdpMove(int sock, int msgId, char* data, int size)
{
	ReqUdpMove req;
	ResUdpMove res;
	req.ParseFromArray(data, size);
	DelBuff(&data);

	int charId = req.charid();//send to xx1 x10
	int beginId;
	if(charId % 10 == 0)
	{
		beginId = charId - 10;
	}
	else
	{
		beginId = charId - charId % 10;
	}

	vector<int> chars;
	for(int i = 0; i < 10; i++)
	{
		chars.push_back(beginId + i + 1);
	}

	res.set_charid(req.charid());
	res.set_x(req.x());
	res.set_y(req.y());
	res.set_time(req.time());

	int packLen = res.ByteSize();
	char * buff = NewBuff(packLen + HEAD_LENGTH);
	PackId::WritePackHead(buff, PackId::BENCHMARK_UDPMOVE, packLen);
	res.SerializeToArray(buff + HEAD_LENGTH, packLen);
	UdpClientMgr::SendToChars(chars, buff, packLen + HEAD_LENGTH);
}

}//end of name space

