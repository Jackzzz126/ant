#include "comm.h"
#include "sock.h"

void Sock::SetNoBlock()
{
	int flag = fcntl(mSock, F_GETFL, 0);
	fcntl(mSock, F_SETFL, flag | O_NONBLOCK);
}

