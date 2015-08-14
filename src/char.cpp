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

//************************************************
map<int, Char*> CharMgr::mIdChars;

CharMgr::CharMgr()
{
}
CharMgr::~CharMgr()
{
}

