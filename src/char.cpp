#include "comm.h"
#include "char.h"

Char::Char(int id, int socket)
{
	mId = id;
	mSocket = socket;
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

