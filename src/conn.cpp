#include "comm.h"
#include "packId.h"
#include "conn.h"

#include "json.h"
#include "msg.h"
#include "poll.h"

Conn::Conn(int sock) : Sock(sock)
{
	mRecvLen = BUFF_UNIT * 2;
	mRecvBuff = NewBuff(mRecvLen);
	mValidSize = 0;

	mSendBuffHead = mSendBuffTail = NULL;
	pthread_mutex_init(&mSendListLock, NULL);

	mCloseAfterWrite = false;
}
Conn::~Conn()
{
	DelBuff(&mRecvBuff);
	while(mSendBuffHead != NULL)
	{
		mSendBuffHead->mRefBuff->Unref();
		SendBuffNode* old = mSendBuffHead;
		mSendBuffHead = mSendBuffHead->mNext;
		delete old;
	}
}
void Conn::ExpandRecvBuff()
{
	char* oldBuff = mRecvBuff;
	mRecvBuff = NewBuff(mRecvLen * 2);
	mRecvLen *= 2;
	if(mValidSize > 0)
	{
		memcpy(mRecvBuff, oldBuff, mValidSize);
	}
	DelBuff(&oldBuff);
}
void Conn::ShrinkRecvBuff()
{
	char* newBuff = NewBuff(BUFF_UNIT * 2);
	if(mValidSize > 0)
	{
		memcpy(newBuff, mRecvBuff, mValidSize);
	}
	DelBuff(&mRecvBuff);
	mRecvBuff = newBuff;
	mRecvLen = BUFF_UNIT * 2;
}

void Conn::OnRead(int timeStamp)
{
	mLastActiveTime = timeStamp;

	int recvLen = recv(mSock, mRecvBuff + mValidSize, mRecvLen - mValidSize, 0);
	if(recvLen <= 0)
	{
		ConnMgr::CloseConn(mSock, false);
		return;
	}
	mValidSize += recvLen;

	if(mValidSize < HEAD_LENGTH)
	{
		return;
	}
	if(IsHttpPack(mRecvBuff))
	{
		ParseHttpPack();
	}
	else
	{
		ParseNormalPack();
	}

	if((mRecvLen - mValidSize) < (int)BUFF_UNIT / 2)//no enough space
	{
		ExpandRecvBuff();
	}
	if(mValidSize < BUFF_UNIT / 2)
	{
		ShrinkRecvBuff();
	}
}
void Conn::OnWrite(int timeStamp)
{
	pthread_mutex_lock(&mSendListLock);
	SendBuffNode* head = mSendBuffHead;

	if(head == NULL)
	{
		if(mCloseAfterWrite)
		{
			ConnMgr::CloseConn(mSock, false);
		}
		else
		{
			pthread_mutex_unlock(&mSendListLock);
			return;
		}
	}
	int sendLen = send(mSock,
			head->mRefBuff->mBuff + head->mOffset,
			head->mRefBuff->mLen - head->mOffset,
			0);
	if(sendLen <= 0)
	{
		ConnMgr::CloseConn(mSock, false);
		pthread_mutex_unlock(&mSendListLock);
		return;
	}
	head->mOffset += sendLen;
	if(!(head->mOffset < head->mRefBuff->mLen))
	{
		head->mRefBuff->Unref();
		if(mSendBuffHead == mSendBuffTail)
		{
			mSendBuffHead = mSendBuffTail = NULL;
		}
		else
		{
			SendBuffNode* oldHead = mSendBuffHead;
			mSendBuffHead = mSendBuffHead->mNext;
			DELETE(oldHead);
		}
	}
	pthread_mutex_unlock(&mSendListLock);
}
void Conn::Write(RefBuff* refBuff)
{
	pthread_mutex_lock(&mSendListLock);
	if(mSendBuffHead == NULL)
	{
		mSendBuffTail = mSendBuffHead = new SendBuffNode(refBuff);
	}
	else
	{
		SendBuffNode* node = new SendBuffNode(refBuff);
		mSendBuffTail->mNext = node;
		mSendBuffTail = node;
	}
	pthread_mutex_unlock(&mSendListLock);
}
void Conn::ParseHttpPack()
{
	char* curPos = mRecvBuff;
	while(true)
	{
		char* headEnd = NULL;
		string url;
		string method;
		int headDataLen = -1;
		if(!ParseHttpHead(curPos, &headEnd, url, method, &headDataLen))
		{
			break;
		}
		if(method == "GET")
		{
			HandleHttpPack(url, NULL, 0);
			curPos = headEnd + 1;
		}
		else
		{
			int dataLen = (mRecvBuff + mValidSize) - headEnd - 1;
			if(dataLen < headDataLen)
			{
				break;
			}
			else
			{
				HandleHttpPack(url, headEnd + 1, dataLen);
				curPos = headEnd + 1 + dataLen;
			}
		}
	}
	ResetBuff(curPos);
}

bool Conn::ParseHttpHead(char* beginPos, char** ppHeadEnd, string& url, string& method, int* pDataLen)
{
	vector<string> httpHead;

	char* head = beginPos;
	char* end = beginPos;
	bool getEnd = false;
	while(end < (mRecvBuff + mValidSize))
	{
		if(*end == '\n' && *(end-1) == '\r')
		{
			string headStr = string(head, end - head - 1);
			if(headStr == "")
			{
				getEnd = true;
				*ppHeadEnd = end;
				break;
			}
			else
			{
				httpHead.push_back(headStr);
				head = end + 1;
			}
		}
		end++;
	}

	if(!getEnd)
	{
		if((end - beginPos) > 512)
		{
			Log::Error("Http head > %d.\n", 512);
			ConnMgr::CloseConn(mSock, true);
		}
		return false;
	}
	
	url = "";
	method = "";
	*pDataLen = -1;
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
			*pDataLen = atoi(httpHead[i].substr(16, httpHead[i].size() - 16).c_str());
		}
	}
	if(url == "" || method == "")
	{
		Log::Error("Unkown http head.\n");
		for(size_t i = 0; i < httpHead.size(); i++)
		{
			Log::Error("%s.\n", httpHead[i].c_str());
		}

		ConnMgr::CloseConn(mSock, true);
		return false;
	}
	if(method == "POST")
	{
		if(*pDataLen == -1 || *pDataLen == 0)
		{
			Log::Error("Error post data: dataLen: %d.\n", *pDataLen);
			ConnMgr::CloseConn(mSock, true);
			return false;
		}
		else if(*pDataLen > 2048)
		{
			Log::Error("Error post data, dataLen: %d bigger than: %d.\n", *pDataLen, 2048);
			ConnMgr::CloseConn(mSock, true);
			return false;
		}
	}
	return true;
}
void Conn::ParseNormalPack()
{
	char* curPos = mRecvBuff;
	while(true)
	{
		if(((mRecvBuff + mValidSize) - curPos) < HEAD_LENGTH)
			break;
		int packId = ntohl(*((int*)curPos)) ^ HEAD_MASK;
		int packLen = ntohl(*((int*)(curPos) + 1)) ^ HEAD_MASK;
		int dataLen = (mRecvBuff + mValidSize) - (curPos + HEAD_LENGTH);
		if(dataLen < packLen)
		{
			break;
		}
		else
		{
			HandleNormalPack(packId, curPos + HEAD_LENGTH, packLen);
			curPos += (packLen + HEAD_LENGTH);
		}
	}
	ResetBuff(curPos);
}

void Conn::ResetBuff(char* curPos)
{
	if(curPos != mRecvBuff)
	{
		if(curPos == (mRecvBuff + mValidSize))
		{
			mValidSize = 0;
		}
		else
		{
			mValidSize = (mRecvBuff + mValidSize) - curPos;
			char* newBuff = NewBuff(mRecvLen);
			memcpy(newBuff, curPos, mValidSize);
			DelBuff(&mRecvBuff);
			mRecvBuff = newBuff;
		}
	}
}

void Conn::Close(bool logErr)
{
	if(logErr)
	{
		sockaddr_in peerAddr;
		int ret = getpeername(mSock, (sockaddr*)&peerAddr, NULL);
		if(ret >= 0)
		{
			char ipStr[64];
			inet_ntop(AF_INET, &peerAddr.sin_addr, ipStr, sizeof(ipStr));
			Log::Error("%s:%d disconnected.\n", ipStr, ntohs(peerAddr.sin_port));
		}
		else
		{
			Log::Error("Sock %d disconnected.\n", mSock);
		}
	}

	close(mSock);
	delete this;
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
	MsgQueue::Singleton()->PushMsg(mSock, packId, data, size);
}

void Conn::HandleHttpPack(const string& url, char* buff, int size)
{
	int packId = 0;
	if(url == "/null")
	{
		if(size < HEAD_LENGTH)
		{
			Log::Error("Unknown http pack, size: %d.\n", size);
			return;
		}
		packId = ntohl(*((int*)buff)) ^ HEAD_MASK;
		int packLen = ntohl(*((int*)(buff) + 1)) ^ HEAD_MASK;
		if((size - HEAD_LENGTH) != packLen)
		{
			Log::Error("Unknown http pack: packId: %d, size:%d.\n",
					packId, size);
			return;
		}
		else
		{
			buff = buff + HEAD_LENGTH;
			size = packLen;
		}
	}
	else
	{
		packId = PackId::GetPackIdByUrl(url);
	}

	if(packId == 0)
	{
		Log::Error("Unknown http req received: %s.\n", url.c_str());
	}
	else
	{
		char* data = NewBuff(size);
		memcpy(data, buff, size);
		MsgQueue::Singleton()->PushMsg(mSock, packId, data, size);
	}
}

//************************************************
map<int, Conn*> ConnMgr::mSockConns;

void ConnMgr::CloseConn(int sock, bool logErr)
{
	Poll::Singleton()->Del(sock);
	map<int, Conn*>::iterator iter = ConnMgr::mSockConns.find(sock);
	if(iter != ConnMgr::mSockConns.end())
	{
		iter->second->Close(logErr);
		ConnMgr::mSockConns.erase(sock);
	}
	else
	{
		Log::Error("Close unknown conn.\n");
		close(sock);
	}
}
void ConnMgr::ErrorEnd(int sock)
{
	RefBuff* pRefBuff = new RefBuff(HEAD_LENGTH, 1);
	PackId::WritePackHead(pRefBuff->mBuff, PackId::INTERNAL_ERROR, 0);

	map<int, Conn*>::iterator iter = ConnMgr::mSockConns.find(sock);
	if(iter != ConnMgr::mSockConns.end())
	{
		iter->second->Write(pRefBuff);
		iter->second->mCloseAfterWrite = true;
	}
	else//client disconnect
	{
		pRefBuff->Unref();
	}
}
void ConnMgr::SendToAll(RefBuff* pRefBuff)
{
	map<int, Conn*>::iterator iter = ConnMgr::mSockConns.begin();
	for(; iter != ConnMgr::mSockConns.end(); iter++)
	{
		iter->second->Write(pRefBuff);
	}
}

void ConnMgr::SendToSock(int sock, RefBuff* pRefBuff)
{
	map<int, Conn*>::iterator iter = ConnMgr::mSockConns.find(sock);
	if(iter != ConnMgr::mSockConns.end())
	{
		iter->second->Write(pRefBuff);
	}
	else//client disconnect
	{
		pRefBuff->Unref();
	}
}

void ConnMgr::SendToSocks(const vector<int>& socks, RefBuff* pRefBuff)
{
	assert(pRefBuff->mRef >= (int)socks.size());
	vector<int>::const_iterator iter = socks.begin();
	for(; iter != socks.end(); iter++)
	{
		map<int, Conn*>::iterator connIter = ConnMgr::mSockConns.find(*iter);
		if(connIter != ConnMgr::mSockConns.end())
		{
			connIter->second->Write(pRefBuff);
		}
		else
		{
			pRefBuff->Unref();
		}
	}
}

void ConnMgr::AddConn(int sock, Conn* pConn)
{
	mSockConns[sock] = pConn;
}



