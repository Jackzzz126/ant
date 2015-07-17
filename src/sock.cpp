#include "comm.h"
#include "sock.h"
#include "dateTime.h"

Sock::Sock(int sock)
{
	mSock = sock;
	SetNoBlock();
	mLastActiveTime = DateTime::GetTimeStamp();
}
Sock::~Sock(){}
void Sock::OnRead(int timeStamp){}
void Sock::OnWrite(int timeStamp){};

void Sock::SetNoBlock()
{
	int flag = fcntl(mSock, F_GETFL, 0);
	fcntl(mSock, F_SETFL, flag | O_NONBLOCK);
}



