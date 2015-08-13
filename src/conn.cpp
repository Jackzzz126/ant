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
	pthread_mutex_init(&mMutex, NULL);

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
	SendBuffNode* head = mSendBuffHead;
	if(head == NULL)
	{
		if(mCloseAfterWrite)
		{
			ConnMgr::CloseConn(mSock, false);
		}
		else
		{
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
			mSendBuffHead = mSendBuffHead->mNext;
			DELETE(mSendBuffHead);
		}
	}
}
void Conn::Write(RefBuff* refBuff)
{
	pthread_mutex_lock(&mMutex);
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
	pthread_mutex_unlock(&mMutex);
}
void Conn::ParseHttpPack()
{
	vector<string> httpHead;

	char* head = mRecvBuff;
	char* end = mRecvBuff;
	while(end < (mRecvBuff + mValidSize))
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
		if((end - mRecvBuff) > 512)
		{
			Log::Error("Http head > %d.\n", 512);
			ConnMgr::CloseConn(mSock, true);
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

		ConnMgr::CloseConn(mSock, true);
		return;
	}
	if(method == "GET")
	{
		HandleHttpPack(url, NULL, 0);
		int excessDataLen = (mRecvBuff + mValidSize) - end;
		if(excessDataLen == 0)
		{
			mValidSize = 0;
		}
		else
		{
			memcpy(mRecvBuff, end, excessDataLen);
			mValidSize = excessDataLen;
		}
	}
	else//post
	{
		if(dataLen == -1 || dataLen == 0)
		{
			Log::Error("Error post data: dataLen: %d.\n", dataLen);
			ConnMgr::CloseConn(mSock, true);
			return;
		}
		else if(dataLen > 2048)
		{
			Log::Error("Error post data, dataLen: %d bigger than: %d.\n", dataLen, 2048);
			ConnMgr::CloseConn(mSock, true);
			return;
		}
		else
		{
			int excessDataLen = (mRecvBuff + mValidSize) - end - dataLen;
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

				memcpy(mRecvBuff, end + dataLen, excessDataLen);
				mValidSize = excessDataLen;
			}
		}
	}
}
void Conn::ParseNormalPack()
{
	while(!(mValidSize < HEAD_LENGTH))
	{
		int packId = *((int*)mRecvBuff) ^ 0x79669966;
		int packLen = *((int*)(mRecvBuff) + 1) ^ 0x79669966;
		int excessDataLen = mValidSize - packLen - HEAD_LENGTH;
		if(excessDataLen < 0)
		{
			break;
		}
		else if(excessDataLen == 0)
		{
			HandleNormalPack(packId, mRecvBuff + HEAD_LENGTH, packLen);
			mValidSize = 0;
		}
		else//excessDataLen > 0
		{
			HandleNormalPack(packId, mRecvBuff + HEAD_LENGTH, packLen);

			memcpy(mRecvBuff, mRecvBuff + packLen + HEAD_LENGTH, excessDataLen);
			mValidSize = excessDataLen;
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
		packId = *((int*)buff) ^ 0x79669966;
		int packLen = *((int*)(buff) + 1) ^ 0x79669966;
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
map<int, Conn*> ConnMgr::mAllConns;

ConnMgr::ConnMgr()
{
}

void ConnMgr::CloseConn(int sock, bool logErr)
{
	Poll::Singleton()->Del(sock);
	map<int, Conn*>::iterator iter = ConnMgr::mAllConns.find(sock);
	if(iter != ConnMgr::mAllConns.end())
	{
		iter->second->Close(logErr);
		ConnMgr::mAllConns.erase(sock);
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

	map<int, Conn*>::iterator iter = ConnMgr::mAllConns.find(sock);
	if(iter != ConnMgr::mAllConns.end())
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
	map<int, Conn*>::iterator iter = ConnMgr::mAllConns.begin();
	for(; iter != ConnMgr::mAllConns.end(); iter++)
	{
		iter->second->Write(pRefBuff);
	}
}

void ConnMgr::SendToOne(int sock, RefBuff* pRefBuff)
{
	map<int, Conn*>::iterator iter = ConnMgr::mAllConns.find(sock);
	if(iter != ConnMgr::mAllConns.end())
	{
		iter->second->Write(pRefBuff);
	}
	else//client disconnect
	{
		pRefBuff->Unref();
	}
}

void ConnMgr::SendToMulti(const vector<int>& socks, RefBuff* pRefBuff)
{
	assert(pRefBuff->mRef >= (int)socks.size());
	vector<int>::const_iterator iter = socks.begin();
	for(; iter != socks.end(); iter++)
	{
		map<int, Conn*>::iterator connIter = ConnMgr::mAllConns.find(*iter);
		if(connIter != ConnMgr::mAllConns.end())
		{
			connIter->second->Write(pRefBuff);
		}
		else
		{
			pRefBuff->Unref();
		}
	}
}



