#include "comm.h"
#include "char.h"
#include "conn.h"

Char::Char(int id, int sock)
{
	mId = id;
	mSock = sock;
}
Char::~Char()
{
}

void Char::Lock()
{
	pthread_mutex_lock(&mMutex);
}

void Char::Unlock()
{
	pthread_mutex_unlock(&mMutex);
}

//************************************************
map<int, Char*> CharMgr::mIdChars;
pthread_mutex_t CharMgr::mMutex;

CharMgr::CharMgr()
{
}

CharMgr::~CharMgr()
{
}

void CharMgr::AddChar(int id, Char* pChar)
{
	pthread_mutex_lock(&mMutex);
	mIdChars[id] = pChar;
	pthread_mutex_unlock(&mMutex);
}

Char* CharMgr::GetChar(int id)
{
	map<int, Char*>::iterator iter = mIdChars.find(id);
	if(iter != mIdChars.end())
	{
		return iter->second;
	}
	else
	{
		return NULL;
	}
}

void CharMgr::SendToChar(int charId, RefBuff* pRefBuff)
{
	Char* pChar = GetChar(charId);
	if(pChar != NULL)
	{
		ConnMgr::SendToSock(pChar->mSock, pRefBuff);
	}
}
void CharMgr::SendToChars(const vector<int>& chars, RefBuff* pRefBuff)
{
	vector<int> socks;
	for(vector<int>::const_iterator iter = chars.begin(); iter != chars.end(); iter++)
	{
		Char* pChar = GetChar(*iter);
		if(pChar != NULL)
		{
			socks.push_back(pChar->mSock);
		}
	}
	ConnMgr::SendToSocks(socks, pRefBuff);
}

