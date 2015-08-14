#ifndef _CHAR_H_
#define _CHAR_H_

class Char
{
public:
	Char(int id, int socket);
	~Char();
public:
	int mId;
	int mSocket;
};

class CharMgr
{
private:
	static map<int, Char*> mIdChars;
private:
	CharMgr();
	~CharMgr();
public:
public:
private:
	pthread_mutex_t mMutex;
};

#endif//_CHAR_H_
