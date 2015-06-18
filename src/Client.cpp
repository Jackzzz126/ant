#include "comm.h"
#include "Client.h"

#include "Log.h"
#include "Define.h"
#include "PackId.h"
#include "Write.h"
#include "Json.h"
#include "Game.h"

//#include "ReqJoinRoom.h"
//#include "ReqBenchMark.h"
//#include "ReqTest.h"

int Client::BUFF_UNIT = 1024;
int Client::HEAD_LENGTH = 8;

Client::Client(uv_stream_t *conn, const string& addr )
{
	mConn = conn;
	mAddr = addr;
	mReceiveBuff.base = new char[1024 * 16];
	mReceiveBuff.len = 1024 * 16;
	mDataBuff.base = new char[Client::BUFF_UNIT];
	mDataBuff.len = Client::BUFF_UNIT;
	mValidSize = 0;

	mPlayer = new Player(conn);
}
Client::~Client()
{
	delete [](char*)mReceiveBuff.base;
	mReceiveBuff.len = 0;
	delete [](char*)mDataBuff.base;
	mDataBuff.len = 0;

	delete mPlayer;
}

void Client::ReceiveData(char* buff, int size)
{
	if(((int)mDataBuff.len - mValidSize) < size)
	{
		char* oldBuff = mDataBuff.base;
	 	int expandSize = (size / Client::BUFF_UNIT + 1) * Client::BUFF_UNIT;
		mDataBuff.base = new char[mDataBuff.len + expandSize];
		mDataBuff.len += expandSize;
		memcpy(mDataBuff.base, oldBuff, mValidSize);
		delete []oldBuff;
	}
	memcpy((char*)mDataBuff.base + mValidSize, buff, size);
	mValidSize += size;

	if(mValidSize < HEAD_LENGTH)
	{
		return;
	}
	if(IsHttpPack(mDataBuff.base))
	{
		ParseHttpPack();
	}
	else
	{
		ParseNormalPack();
	}
}
void Client::ParseHttpPack()
{
	vector<string> httpHead;

	char* head = mDataBuff.base;
	char* end = mDataBuff.base;
	while(end < ((char*)mDataBuff.base + mValidSize))
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
	for(uint i = 0; i < httpHead.size(); i++)
	{
		if(httpHead[i].substr(0, 4) == "GET ")
		{
			uint nextBlankPos = httpHead[i].find(' ',4);
			if(nextBlankPos == string::npos)
				break;
			url = httpHead[i].substr(4, nextBlankPos - 4);
			method = "GET";
		}
		else if(httpHead[i].substr(0, 4) == "POST")
		{
			uint nextBlankPos = httpHead[i].find(' ',5);
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
		ClientMgr::CloseClient(mConn, Client::UNKNOWN_DATA);
		return;
	}
	if(method == "GET")
	{
		HandleHttpPack(url);
		int excessDataLen = ((char*)mDataBuff.base + mValidSize) - end;
		if(excessDataLen == 0)
		{
			mValidSize = 0;
		}
		else
		{
			memcpy(mDataBuff.base, end, excessDataLen);
			mValidSize = excessDataLen;
		}
	}
	else
	{
		if(dataLen == -1 || dataLen == 0)
		{
			ClientMgr::CloseClient(mConn, Client::UNKNOWN_DATA);
			return;
		}
		else
		{
			int excessDataLen = ((char*)mDataBuff.base + mValidSize) - end - dataLen;
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

				memcpy(mDataBuff.base, end + dataLen, excessDataLen);
				mValidSize = excessDataLen;
			}
		}
	}
}
void Client::ParseNormalPack()
{
	int packLen = *((int*)mDataBuff.base) ^ 0x79669966;
	int packId = *((int*)(mDataBuff.base) + 1) ^ 0x79669966;
	int excessDataLen = mValidSize - packLen - HEAD_LENGTH;
	if(excessDataLen < 0)
	{
		return;
	}
	else if(excessDataLen == 0)
	{
		HandleNormalPack(packId, mDataBuff.base + HEAD_LENGTH, packLen);
		mValidSize = 0;
	}
	else//excessDataLen > 0
	{
		HandleNormalPack(packId, mDataBuff.base + HEAD_LENGTH, packLen);

		memcpy(mDataBuff.base, (char*)mDataBuff.base + mValidSize, excessDataLen);
		mValidSize = excessDataLen;
	}
}

void Client::Destroy(Client::DisconnectReason reason)
{
	if(reason == Client::UNKNOWN_DATA)
	{
		Log::Out("%s: disconnected[unknown data].\n", mAddr.c_str());
	}
	else if(reason == Client::SOCK_ERROR)
	{
		Log::Out("%s: disconnected[socket error].\n", mAddr.c_str());
	}
	else if(reason == Client::VALID_FAIL)
	{
		Log::Out("%s: disconnected[valid fail].\n", mAddr.c_str());
	}
	else//server down or client close or server close
	{
	}
	uv_close((uv_handle_t*)mConn, NULL);
	delete this;
}

void Client::HandleHttpPack(const string& url, char* buff, int size)
{
	if(url == "/hello")
	{
	}
	else
	{
		uv_write_t *req = new uv_write_t;
		char buff[] = "HTTP/1.1 404 Not Found\r\nContent-Lenght:0\r\nContent-Type:'text/plain'\r\n\r\n";
		uv_buf_t sendBuff = NewBuffer(72/Client::BUFF_UNIT + Client::BUFF_UNIT);
		memcpy(sendBuff.base, buff, 72);
		req->data = (void*)sendBuff.base;
		uv_write( req, mConn, &sendBuff, 1, OnWriteClose);
		return;
	}
}
void Client::HandleHttpPack(const string& url)
{
	if(url == "/hello")
	{
		uv_write_t *req = new uv_write_t;
		char buff[] = "HTTP/1.1 200 OK\r\nContent-Lenght:12\r\nContent-Type:'text/plain'\r\n\r\nHello world.";
		uv_buf_t sendBuff = NewBuffer(77/Client::BUFF_UNIT + Client::BUFF_UNIT);
		memcpy(sendBuff.base, buff, 77);
		req->data = (void*)sendBuff.base;
		uv_write( req, mConn, &sendBuff, 1, OnWriteClose);
		return;
	}
	else
	{
		uv_write_t *req = new uv_write_t;
		char buff[] = "HTTP/1.1 404 Not Found\r\nContent-Lenght:0\r\nContent-Type:'text/plain'\r\n\r\n";
		uv_buf_t sendBuff = NewBuffer(72/Client::BUFF_UNIT + Client::BUFF_UNIT);
		memcpy(sendBuff.base, buff, 72);
		req->data = (void*)sendBuff.base;
		uv_write( req, mConn, &sendBuff, 1, OnWriteClose);
		return;
	}
}

bool Client::IsHttpPack(char* buff)
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

void Client::HandleNormalPack(int packId, char* buff, int size)
{
	switch(packId)
	{
	case 11:
		break;
	case 12:
		{
			uv_buf_t sendBuff = NewBuffer(size + HEAD_LENGTH);
			memcpy(sendBuff.base + HEAD_LENGTH, buff, size);
			*(int*)(sendBuff.base) = size ^ 0x79669966;
			*((int*)(sendBuff.base) + 1) = 12 ^ 0x79669966;
			ClientMgr::SendPackToAll(sendBuff);
			break;
		}
	default:
		break;
	}
	//if( packId < PackId::MIN_PACKID || packId > PackId::MAX_PACKID )
	//{
	//	ClientMgr::CloseClient(mConn, Client::UNKNOWN_DATA);
	//	return;
	//}
	//if(packId == PackId::JOINROOM)
	//{
	//	ReqJoinRoom req;
	//	req.InitBuff(buff, size);
	//	if(!req.Parst())
	//	{
	//		ClientMgr::CloseClient(mConn, Client::UNKNOWN_DATA);
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
	//		ClientMgr::CloseClient(mConn, Client::UNKNOWN_DATA);
	//		return;
	//	}
	//	printf("%s\n", req.m_buff.mBuff);
	//	printf("%s\n", req.m_buff.mBuff + 5);
	//	for(uint i = 0; i < req.m_array1.size(); i++)
	//	{
	//		printf("%d, ", req.m_array1[i]);
	//	}
	//	printf("\n");
	//	for(uint i = 0; i < req.m_array2.size(); i++)
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
	//		ClientMgr::CloseClient(mConn, Client::UNKNOWN_DATA);
	//		return;
	//	}
	//	uv_buf_t sendBuff;
	//	req.Write(sendBuff);
	//	ClientMgr::SendPackToAll(sendBuff);
	//}
	//else
	//{
	//	ClientMgr::CloseClient(mConn, Client::UNKNOWN_DATA);
	//	return;
	//}
}

//************************************************
map<void*, Client*> ClientMgr::mAllClients;

ClientMgr::ClientMgr()
{
}

void ClientMgr::CloseClient(uv_stream_t* conn, Client::DisconnectReason reason)
{
	map<void*, Client*>::iterator iter = ClientMgr::mAllClients.find(conn);
	if(iter != ClientMgr::mAllClients.end())
	{
		iter->second->Destroy(reason);
		ClientMgr::mAllClients.erase(conn);
	}
	else
	{
		Log::Error("Unknown client.\n");
		uv_close((uv_handle_t*)conn, NULL);
	}
}
void ClientMgr::SendPackToAll(uv_buf_t buff)
{
	map<void*, Client*>::iterator iter = ClientMgr::mAllClients.begin();
	for(; iter != ClientMgr::mAllClients.end(); iter++)
	{
		uv_buf_t sendBuff = NewBuffer(buff.len);
		memcpy(sendBuff.base, buff.base, buff.len);
		//uv_write_t *req = new uv_write_t;
		//req->data = buff.base;
		//uv_write( req, iter->second->mConn, &buff, 1, OnWrite);
		write_req_t* req = new write_req_t;
		req->buf = sendBuff;
		uv_write(&req->req, iter->second->mConn, &req->buf, 1, OnWrite);
	}
	delete [](char*)(buff.base);
}



