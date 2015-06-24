#include "comm.h"
#include "Conn.h"

#include "Log.h"
#include "Define.h"
#include "PackId.h"
#include "Write.h"
#include "Json.h"
#include "Game.h"

//#include "ReqJoinRoom.h"
//#include "ReqBenchMark.h"
//#include "ReqTest.h"

int Conn::BUFF_UNIT = 1024;
int Conn::HEAD_LENGTH = 8;

Conn::Conn(uv_stream_t *conn, const string& addr )
{
	mConn = conn;
	mAddr = addr;
	mRecvBuff.base = NewBuff(Conn::BUFF_UNIT * 2);
	mRecvBuff.len = Conn::BUFF_UNIT * 2;
	mValidSize = 0;
}
Conn::~Conn()
{
	DelBuff(&mRecvBuff.base);
	mRecvBuff.len = 0;
}
void Conn::ExpandRecvBuff()
{
	char* oldBuff = mRecvBuff.base;
	mRecvBuff.base = NewBuff(mRecvBuff.len * 2);
	mRecvBuff.len *= 2;
	if(mValidSize > 0)
	{
		memcpy(mRecvBuff.base, oldBuff, mValidSize);
	}
	DelBuff(&oldBuff);
}
void Conn::ShrinkRecvBuff()
{
	char* newBuff = NewBuff(Conn::BUFF_UNIT * 2);
	if(mValidSize > 0)
	{
		memcpy(newBuff, mRecvBuff.base, mValidSize);
	}
	DelBuff(&mRecvBuff.base);
	mRecvBuff.base = newBuff;
	mRecvBuff.len = BUFF_UNIT * 2;
}

void Conn::RecvData(char* buff, int size)
{
	mValidSize += size;

	if(mValidSize < HEAD_LENGTH)
	{
		return;
	}
	if(IsHttpPack(mRecvBuff.base))
	{
		ParseHttpPack();
	}
	else
	{
		ParseNormalPack();
	}

	if((mRecvBuff.len - mValidSize) < (size_t)BUFF_UNIT / 2)//no enough space
	{
		ExpandRecvBuff();
	}
	if(mValidSize < BUFF_UNIT / 2)
	{
		ShrinkRecvBuff();
	}
}
void Conn::ParseHttpPack()
{
	vector<string> httpHead;

	char* head = mRecvBuff.base;
	char* end = mRecvBuff.base;
	while(end < ((char*)mRecvBuff.base + mValidSize))
	{
		if(*end == '\n' && *(end-1) == '\r')
		{
			if(*(end+1) == '\r' && *(end+2) == '\n')
			{
				end += 3;
				break;
			}
			else
			{
				httpHead.push_back(string(head, end - head - 1));
				head = end + 1;
			}
		}
		end++;
	}

	if(*(end-1) != '\n' || *(end-2) != '\r' )
		return;
	
	string url;
	string method;
	int dataLen = -1;
	for(size_t i = 0; i < httpHead.size(); i++)
	{
		if(httpHead[i].substr(0, 4) == "GET ")
		{
			size_t nextBlankPos = httpHead[i].find(' ',4);
			if(nextBlankPos == string::npos)
				break;
			url = httpHead[i].substr(4, nextBlankPos - 4);
			method = "GET";
		}
		else if(httpHead[i].substr(0, 4) == "POST")
		{
			size_t nextBlankPos = httpHead[i].find(' ',5);
			if(nextBlankPos == string::npos)
				break;
			url = httpHead[i].substr(5, nextBlankPos - 5);
			method = "POST";
		}
		else if(httpHead[i].substr(0, 16) == "Content-Length: ")
		{
			dataLen = atoi(httpHead[i].substr(16, httpHead[i].size() - 16).c_str());
		}
	}
	if(url == "" || method == "")
	{
		ConnMgr::CloseConn(mConn, Conn::UNKNOWN_DATA);
		return;
	}
	if(method == "GET")
	{
		HandleHttpPack(url);
		int excessDataLen = ((char*)mRecvBuff.base + mValidSize) - end;
		if(excessDataLen == 0)
		{
			mValidSize = 0;
		}
		else
		{
			memcpy(mRecvBuff.base, end, excessDataLen);
			mValidSize = excessDataLen;
		}
	}
	else
	{
		if(dataLen == -1 || dataLen == 0)
		{
			ConnMgr::CloseConn(mConn, Conn::UNKNOWN_DATA);
			return;
		}
		else
		{
			int excessDataLen = ((char*)mRecvBuff.base + mValidSize) - end - dataLen;
			if(excessDataLen < 0)
			{
				return;
			}
			else if(excessDataLen == 0)
			{
				HandleHttpPack(url, end, dataLen);
			}
			else//excessDataLen > 0
			{
				HandleHttpPack(url, end, dataLen);

				memcpy(mRecvBuff.base, end + dataLen, excessDataLen);
				mValidSize = excessDataLen;
			}
		}
	}
}
void Conn::ParseNormalPack()
{
	int packId = *((int*)mRecvBuff.base) ^ 0x79669966;
	int packLen = *((int*)(mRecvBuff.base) + 1) ^ 0x79669966;
	int excessDataLen = mValidSize - packLen - HEAD_LENGTH;
	if(excessDataLen < 0)
	{
		return;
	}
	else if(excessDataLen == 0)
	{
		HandleNormalPack(packId, mRecvBuff.base + HEAD_LENGTH, packLen);
		mValidSize = 0;
	}
	else//excessDataLen > 0
	{
		HandleNormalPack(packId, mRecvBuff.base + HEAD_LENGTH, packLen);

		memcpy(mRecvBuff.base, (char*)mRecvBuff.base + mValidSize, excessDataLen);
		mValidSize = excessDataLen;
	}
}

void Conn::Destroy(Conn::DisconnReason reason)
{
	if(reason == Conn::UNKNOWN_DATA)
	{
		Log::Out("%s: disconnected[unknown data].\n", mAddr.c_str());
	}
	else if(reason == Conn::SOCK_ERROR)
	{
		Log::Out("%s: disconnected[socket error].\n", mAddr.c_str());
	}
	else if(reason == Conn::VALID_FAIL)
	{
		Log::Out("%s: disconnected[valid fail].\n", mAddr.c_str());
	}
	else//server down or client close or server close
	{
	}
	uv_close((uv_handle_t*)mConn, NULL);
	delete this;
}

void Conn::HandleHttpPack(const string& url, char* buff, int size)
{
	if(url == "/hello")
	{
	}
	else
	{
		uv_write_t *req = new uv_write_t;
		char buff[] = "HTTP/1.1 404 Not Found\r\nContent-Lenght:0\r\nContent-Type:'text/plain'\r\n\r\n";
		uv_buf_t sendBuff = NewUvBuff(72/Conn::BUFF_UNIT + Conn::BUFF_UNIT);
		memcpy(sendBuff.base, buff, 72);
		req->data = (void*)sendBuff.base;
		uv_write( req, mConn, &sendBuff, 1, OnWriteClose);
		return;
	}
}
void Conn::HandleHttpPack(const string& url)
{
	if(url == "/hello")
	{
		uv_write_t *req = new uv_write_t;
		char buff[] = "HTTP/1.1 200 OK\r\nContent-Lenght:12\r\nContent-Type:'text/plain'\r\n\r\nHello world.";
		uv_buf_t sendBuff = NewUvBuff(77/Conn::BUFF_UNIT + Conn::BUFF_UNIT);
		memcpy(sendBuff.base, buff, 77);
		req->data = (void*)sendBuff.base;
		uv_write( req, mConn, &sendBuff, 1, OnWriteClose);
		return;
	}
	else
	{
		uv_write_t *req = new uv_write_t;
		char buff[] = "HTTP/1.1 404 Not Found\r\nContent-Lenght:0\r\nContent-Type:'text/plain'\r\n\r\n";
		uv_buf_t sendBuff = NewUvBuff(72/Conn::BUFF_UNIT + Conn::BUFF_UNIT);
		memcpy(sendBuff.base, buff, 72);
		req->data = (void*)sendBuff.base;
		uv_write( req, mConn, &sendBuff, 1, OnWriteClose);
		return;
	}
}

bool Conn::IsHttpPack(char* buff)
{
	char text[5];
	memcpy(text, buff, 4);
	text[4] = '\0';
	if(strcmp(text, "GET ") == 0)
	{
		return true;
	}
	else if(strcmp(text, "POST") == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Conn::HandleNormalPack(int packId, char* buff, int size)
{
	switch(packId)
	{
	case 11:
		break;
	case 12:
		{
			uv_buf_t sendBuff = NewUvBuff(size + HEAD_LENGTH);
			memcpy(sendBuff.base + HEAD_LENGTH, buff, size);
			*(int*)(sendBuff.base) = size ^ 0x79669966;
			*((int*)(sendBuff.base) + 1) = 12 ^ 0x79669966;
			ConnMgr::SendPackToAll(sendBuff);
			break;
		}
	default:
		break;
	}
	//if( packId < PackId::MIN_PACKID || packId > PackId::MAX_PACKID )
	//{
	//	ConnMgr::CloseConn(mConn, Conn::UNKNOWN_DATA);
	//	return;
	//}
	//if(packId == PackId::JOINROOM)
	//{
	//	ReqJoinRoom req;
	//	req.InitBuff(buff, size);
	//	if(!req.Parst())
	//	{
	//		ConnMgr::CloseConn(mConn, Conn::UNKNOWN_DATA);
	//		return;
	//	}
	//	Game::ValidChar(req.m_session, req.m_charId, req.m_ip, req.m_port, mPlayer);
	//}
	//else if( packId == PackId::TEST )
	//{
	//	ReqTest req;
	//	req.InitBuff(buff, size);
	//	if(!req.Parse())
	//	{
	//		ConnMgr::CloseConn(mConn, Conn::UNKNOWN_DATA);
	//		return;
	//	}
	//	printf("%s\n", req.m_buff.mBuff);
	//	printf("%s\n", req.m_buff.mBuff + 5);
	//	for(size_t i = 0; i < req.m_array1.size(); i++)
	//	{
	//		printf("%d, ", req.m_array1[i]);
	//	}
	//	printf("\n");
	//	for(size_t i = 0; i < req.m_array2.size(); i++)
	//	{
	//		printf("%f, ", req.m_array2[i]);
	//	}
	//	printf("\n");
	//}
	//else if( packId == PackId::BENCHMARK )
	//{
	//	ReqBenchMark req;
	//	req.InitBuff(buff, size);
	//	if(!req.Parse())
	//	{
	//		ConnMgr::CloseConn(mConn, Conn::UNKNOWN_DATA);
	//		return;
	//	}
	//	uv_buf_t sendBuff;
	//	req.Write(sendBuff);
	//	ConnMgr::SendPackToAll(sendBuff);
	//}
	//else
	//{
	//	ConnMgr::CloseConn(mConn, Conn::UNKNOWN_DATA);
	//	return;
	//}
}

//************************************************
map<void*, Conn*> ConnMgr::mAllConns;

ConnMgr::ConnMgr()
{
}

void ConnMgr::CloseConn(uv_stream_t* conn, Conn::DisconnReason reason)
{
	map<void*, Conn*>::iterator iter = ConnMgr::mAllConns.find(conn);
	if(iter != ConnMgr::mAllConns.end())
	{
		iter->second->Destroy(reason);
		ConnMgr::mAllConns.erase(conn);
	}
	else
	{
		Log::Error("Unknown client.\n");
		uv_close((uv_handle_t*)conn, NULL);
	}
}
void ConnMgr::SendPackToAll(uv_buf_t buff)
{
	map<void*, Conn*>::iterator iter = ConnMgr::mAllConns.begin();
	for(; iter != ConnMgr::mAllConns.end(); iter++)
	{
		uv_buf_t sendBuff = NewUvBuff(buff.len);
		memcpy(sendBuff.base, buff.base, buff.len);
		//uv_write_t *req = new uv_write_t;
		//req->data = buff.base;
		//uv_write( req, iter->second->mConn, &buff, 1, OnWrite);
		write_req_t* req = new write_req_t;
		req->buf = sendBuff;
		uv_write(&req->req, iter->second->mConn, &req->buf, 1, OnWrite);
	}
	DelBuff(&buff.base);
}



