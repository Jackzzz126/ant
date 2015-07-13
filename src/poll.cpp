#include "comm.h"
#include <sys/socket.h>
#include <sys/epoll.h>

#include "poll.h"
#include "sock.h"


Poll* Poll::mGlobalPoll = NULL;
Poll* Poll::Singleton()
{
	if(mGlobalConfig == NULL)
	{
		mGlobalConfig = new Config();
	}
	return mGlobalConfig;
}

Poll::Poll()
{
	mPollFd = -1;
}

Poll::~Poll()
{
	Close();
}

bool Poll::Create()
{
	mPollFd = epoll_create(1024);
	return mPollFd > 0;
}

void Poll::Close()
{
	if(mPollFd != -1)
	{
		close(mPollFd);
		mPollFd = -1;
	}
}

bool Poll::Add(int sock, void *ud)
{
	epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.ptr = ud;
	if(epoll_ctl(mPollFd, EPOLL_CTL_ADD, sock, &ev) == -1)
	{
		return false;
	}
	return true;
}

void SetWrite(int sock, void *ud)
{
	epoll_event ev;
	ev.events = EPOLLIN | EPOLLOUT;
	ev.data.ptr = ud;
	epoll_ctl(mPollFd, EPOLL_CTL_MOD, sock, &ev);
}

void Poll::Del(int sock)
{
	epoll_ctl(mPollFd, EPOLL_CTL_DEL, sock , NULL);
}

int Wait(int size)
{
	epoll_event ev[size];
	int n = epoll_wait(mPollFd, ev, size, -1);//-1 for block
	for(int i=0; i<n; i++)
	{
		Sock* pSock = ev[i].data.ptr;
		unsigned flag = ev[i].events;
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


