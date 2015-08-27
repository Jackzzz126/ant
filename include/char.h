#ifndef _CHAR_H_
#define _CHAR_H_

#include "refBuff.h"

class Char
{
public:
	Char(int id, int sock);
	~Char();
public:
	void Lock();
	void Unlock();
public:
	int mId;
	int mSock;

private:
	pthread_mutex_t mCharLock;
};

class CharMgr
{
public:
	static CharMgr* Singleton();
private:
	static CharMgr* mCharMgrSingleton;
private:
	CharMgr();
	~CharMgr();
public:
	void AddChar(int id, Char* pChar);
	Char* GetChar(int id);

	void SendToChar(int charId, RefBuff* pRefBuff);
	void SendToChars(const vector<int>& chars, RefBuff* pRefBuff);
private:
	map<int, Char*> mIdChars;
	pthread_mutex_t mIdCharsLock;
};

#endif//_CHAR_H_
