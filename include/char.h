#ifndef _CHAR_H_
#define _CHAR_H_

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
	pthread_mutex_t mMutex;
};

class CharMgr
{
private:
	static map<int, Char*> mIdChars;
	static pthread_mutex_t mMutex;
private:
	CharMgr();
	~CharMgr();
public:
	static void AddChar(int id, Char* pChar);
	static Char* GetChar(int id);

	static void SendToChar(int charId, RefBuff* pRefBuff);
	static void SendToChars(const vector<int>& chars, RefBuff* pRefBuff);
private:
};

#endif//_CHAR_H_
