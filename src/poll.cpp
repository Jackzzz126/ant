#include "comm.h"
#include <sys/socket.h>
#include <sys/epoll.h>

#include "poll.h"
#include "sock.h"
#include "conn.h"
#include "log.h"


Poll* Poll::mGlobalPoll = NULL;
Poll* Poll::Singleton()
{
	if(mGlobalPoll == NULL)
	{
		mGlobalPoll = new Poll();
	}
	return mGlobalPoll;
}

Poll::Poll()
{
	mPoll = -1;
}

Poll::~Poll()
{
	Close();
}

bool Poll::Create()
{
	mPoll = epoll_create(1024);
	return mPoll > 0;
}

void Poll::Close()
{
	if(mPoll != -1)
	{
		close(mPoll);
		mPoll = -1;
	}
}

bool Poll::Add(int sock, void *ud)
{
	epoll_event ev;
	ev.events = EPOLLIN | EPOLLRDHUP;
	ev.data.ptr = ud;
	if(epoll_ctl(mPoll, EPOLL_CTL_ADD, sock, &ev) == -1)
	{
		return false;
	}
	return true;
}

void Poll::SetWrite(int sock, void *ud)
{
	epoll_event ev;
	ev.events = EPOLLIN | EPOLLOUT;
	ev.data.ptr = ud;
	epoll_ctl(mPoll, EPOLL_CTL_MOD, sock, &ev);
}

void Poll::Del(int sock)
{
	epoll_ctl(mPoll, EPOLL_CTL_DEL, sock , NULL);
}

int Poll::Wait(int size)
{
	epoll_event ev[size];
	int n = epoll_wait(mPoll, ev, size, 1000 * 1);//-1 for block
	for(int i=0; i<n; i++)
	{
		Sock* pSock = (Sock*)(ev[i].data.ptr);
		unsigned flag = ev[i].events;
		if((flag & EPOLLRDHUP) != 0)
		{
			ConnMgr::CloseConn(pSock->mSock, true);
		}
		if((flag & EPOLLOUT) != 0)
		{
			pSock->OnWrite();
		}
		if((flag & EPOLLIN) != 0)
		{
			pSock->OnRead();
		}
	}
	return n;
}

