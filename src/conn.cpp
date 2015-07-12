#include "comm.h"
#include "packId.h"
#include "conn.h"

#include "log.h"
#include "define.h"
#include "write.h"
#include "json.h"
#include "game.h"
#include "msg.h"

//#include "ReqJoinRoom.h"
//#include "ReqBenchMark.h"
//#include "ReqTest.h"

Conn::Conn(void* conn)
{
	mConn = conn;
	mRecvBuff.base = NewBuff(BUFF_UNIT * 2);
	mRecvBuff.len = BUFF_UNIT * 2;
	mValidSize = 0;

	mSock = socket(AF_INET, SOCK_STREAM, 0);
	int flag = fcntl(mSock, F_GETFL, 0);
	fcntl(mSock, F_SETFL, flag | O_NONBLOCK);
	mRead = false;
	mWrite = true;
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
	char* newBuff = NewBuff(BUFF_UNIT * 2);
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

	if(*(end-1) != '\n' || *(end-2) != '\r' )//not found http head end
	{
		if((end - mRecvBuff.base) > 512)
		{
			Log::Error("Http head > %d.\n", 512);
			ConnMgr::CloseConn(mConn, true);
			return;
		}
		else
		{
			return;
		}
	}
	
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
		Log::Error("Unkown http head.\n");
		for(size_t i = 0; i < httpHead.size(); i++)
		{
			Log::Error("%s.\n", httpHead[i].c_str());
		}

		ConnMgr::CloseConn(mConn, true);
		return;
	}
	if(method == "GET")
	{
		HandleHttpGet(url);
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
	else//post
	{
		if(dataLen == -1 || dataLen == 0)
		{
			Log::Error("Error post data: dataLen: %d.\n", dataLen);
			ConnMgr::CloseConn(mConn, true);
			return;
		}
		else if(dataLen > 2048)
		{
			Log::Error("Error post data, dataLen: %d bigger than: %d.\n", dataLen, 2048);
			ConnMgr::CloseConn(mConn, true);
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
				HandleHttpPost(url, end, dataLen);
			}
			else//excessDataLen > 0
			{
				HandleHttpPost(url, end, dataLen);

				memcpy(mRecvBuff.base, end + dataLen, excessDataLen);
				mValidSize = excessDataLen;
			}
		}
	}
}
void Conn::ParseNormalPack()
{
	while(!(mValidSize < HEAD_LENGTH))
	{
		int packId = *((int*)mRecvBuff.base) ^ 0x79669966;
		int packLen = *((int*)(mRecvBuff.base) + 1) ^ 0x79669966;
		int excessDataLen = mValidSize - packLen - HEAD_LENGTH;
		if(excessDataLen < 0)
		{
			break;
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
}

void Conn::Destroy(bool logErr)
{
	if(logErr)
	{
		string clientAddr = GetClientAddr((uv_stream_t*)mConn);
		Log::Error("%s: disconnected.\n", clientAddr.c_str());
	}

	uv_close((uv_handle_t*)mConn, NULL);
	delete this;
}

void Conn::HandleHttpPost(const string& url, char* buff, int size)
{
	if(url == "/hello")
	{
	}
	else
	{
		uv_write_t *req = new uv_write_t;
		char buff[] = "HTTP/1.1 404 Not Found\r\nContent-Lenght:0\r\nContent-Type:'text/plain'\r\n\r\n";
		uv_buf_t sendBuff = NewUvBuff(72/BUFF_UNIT + BUFF_UNIT);
		memcpy(sendBuff.base, buff, 72);
		req->data = (void*)sendBuff.base;
		uv_write( req, (uv_stream_t*)mConn, &sendBuff, 1, OnWriteClose);
		return;
	}
}
void Conn::HandleHttpGet(const string& url)
{
	if(url == "/hello")
	{
		uv_write_t *req = new uv_write_t;
		char buff[] = "HTTP/1.1 200 OK\r\nContent-Lenght:12\r\nContent-Type:'text/plain'\r\n\r\nHello world.";
		uv_buf_t sendBuff = NewUvBuff(77/BUFF_UNIT + BUFF_UNIT);
		memcpy(sendBuff.base, buff, 77);
		req->data = (void*)sendBuff.base;
		uv_write( req, (uv_stream_t*)mConn, &sendBuff, 1, OnWriteClose);
		return;
	}
	else
	{
		uv_write_t *req = new uv_write_t;
		char buff[] = "HTTP/1.1 404 Not Found\r\nContent-Lenght:0\r\nContent-Type:'text/plain'\r\n\r\n";
		uv_buf_t sendBuff = NewUvBuff(72/BUFF_UNIT + BUFF_UNIT);
		memcpy(sendBuff.base, buff, 72);
		req->data = (void*)sendBuff.base;
		uv_write( req, (uv_stream_t*)mConn, &sendBuff, 1, OnWriteClose);
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
	char* data = NewBuff(size);
	memcpy(data, buff, size);
	MsgQueue::Singleton()->PushMsg(mConn, packId, data, size);
	//switch(packId)
	//{
	//case 11:
	//	break;
	//case 12:
	//	{
	//		uv_buf_t sendBuff = NewUvBuff(size + HEAD_LENGTH);
	//		memcpy(sendBuff.base + HEAD_LENGTH, buff, size);
	//		*(int*)(sendBuff.base) = size ^ 0x79669966;
	//		*((int*)(sendBuff.base) + 1) = 12 ^ 0x79669966;
	//		ConnMgr::SendPackToAll(sendBuff);
	//		break;
	//	}
	//default:
	//	break;
	//}
	//if( packId < PackId::MIN_PACKID || packId > PackId::MAX_PACKID )
	//{
	//	ConnMgr::CloseConn(mConn, UNKNOWN_DATA);
	//	return;
	//}
	//if(packId == PackId::JOINROOM)
	//{
	//	ReqJoinRoom req;
	//	req.InitBuff(buff, size);
	//	if(!req.Parst())
	//	{
	//		ConnMgr::CloseConn(mConn, UNKNOWN_DATA);
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
	//		ConnMgr::CloseConn(mConn, UNKNOWN_DATA);
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
	//		ConnMgr::CloseConn(mConn, UNKNOWN_DATA);
	//		return;
	//	}
	//	uv_buf_t sendBuff;
	//	req.Write(sendBuff);
	//	ConnMgr::SendPackToAll(sendBuff);
	//}
	//else
	//{
	//	ConnMgr::CloseConn(mConn, UNKNOWN_DATA);
	//	return;
	//}
}

//************************************************
map<void*, Conn*> ConnMgr::mAllConns;

ConnMgr::ConnMgr()
{
}

void ConnMgr::CloseConn(void* conn, bool logErr)
{
	map<void*, Conn*>::iterator iter = ConnMgr::mAllConns.find(conn);
	if(iter != ConnMgr::mAllConns.end())
	{
		iter->second->Destroy(logErr);
		ConnMgr::mAllConns.erase(conn);
	}
	else
	{
		Log::Error("Close unknown conn.\n");
		uv_close((uv_handle_t*)conn, NULL);
	}
}
void ConnMgr::SendToAll(PackId::PackIdType packId, uv_buf_t buff)
{
	WriteReq* pWriteReq = new WriteReq();

	*(int*)(buff.base) = packId ^ 0x79669966;
	*((int*)(buff.base) + 1) = (buff.len - HEAD_LENGTH) ^ 0x79669966;
	pWriteReq->mBuff = buff;

	pWriteReq->mRef = mAllConns.size();
	map<void*, Conn*>::iterator iter = ConnMgr::mAllConns.begin();
	for(; iter != ConnMgr::mAllConns.end(); iter++)
	{
		uv_write(&pWriteReq->mWrite, (uv_stream_t*)iter->second->mConn, &pWriteReq->mBuff, 1, OnWrite);
	}
}

void ConnMgr::SendToOne(void* conn, PackId::PackIdType packId, uv_buf_t buff)
{
	WriteReq* pWriteReq = new WriteReq();

	*(int*)(buff.base) = packId ^ 0x79669966;
	*((int*)(buff.base) + 1) = (buff.len - HEAD_LENGTH) ^ 0x79669966;
	pWriteReq->mBuff = buff;

	uv_write(&pWriteReq->mWrite, (uv_stream_t*)conn, &pWriteReq->mBuff, 1, OnWrite);
}

void ConnMgr::SendToMulti(const vector<void*>& conns, PackId::PackIdType packId, uv_buf_t buff)
{
	WriteReq* pWriteReq = new WriteReq();

	*(int*)(buff.base) = packId ^ 0x79669966;
	*((int*)(buff.base) + 1) = (buff.len - HEAD_LENGTH) ^ 0x79669966;
	pWriteReq->mBuff = buff;

	pWriteReq->mRef = conns.size();
	vector<void*>::const_iterator iter = conns.begin();
	for(; iter != conns.end(); iter++)
	{
		uv_write(&pWriteReq->mWrite, (uv_stream_t*)(*iter), &pWriteReq->mBuff, 1, OnWrite);
	}
}



