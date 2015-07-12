#include "comm.h"
#include "log.h"
#include "define.h"
#include "time.h"
#include "config.h"
#include "conn.h"
#include "gate.h"
#include "msg.h"
#include "router.h"

bool gGotQuitSignal = false;

void worker_func(void* arg);

int main(int argc, char* argv[])
{
	//config
	Config* pConfig = Config::Singleton();
	char configFileName[] = "config/config.json";
	if(!pConfig->Load(configFileName))
	{
		return 1;
	}
	//init
	if(pConfig->mDaemon)
	{
		if(daemon(1,0)) 
		{
			printf("Daemonize error.\n");
			return 1;
		}
	}
	Router::Init();

	//socket poll
	Poll poll;
	if(!poll.Create())
	{
		printf("Create poll error.\n");
		return 1;
	}

	//listener
	Conn* pListener = new Conn();
	poll.Add(pListener->mSock, pListener);

	sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(pConfig->mIp);
	sin.sin_port = htons(pConfig->mPort);
	bind(pListener->mSock, (const sockaddr*)&sin, sizeof(sin));
	listen(pListener->mSock, pConfig->mBacklog);

	//thread start
	vector<uv_thread_t> threads;
	for(int i = 0; i < pConfig->mWorkerThreads; i++)
	{
		uv_thread_t workerThreadId;
		uv_thread_create(&workerThreadId, worker_func, NULL);
		threads.push_back(workerThreadId);
	}
	//main loop
	while(!gGotQuitSignal)
	{
		int fdCount = poll.Wait(ConnMgr::mAllConns.size() + 1);
		if(fdCount < 0)
		{
			Log::Error("Poll wait error: %d.", fdCount);
			break;
		}
		for(int i = 0; i < fds; i++){
			myevent_s *ev = (struct myevent_s*)events[i].data.ptr;
			if((events[i].events&EPOLLIN)&&(ev->events&EPOLLIN))  read event
			{
				ev->call_back(ev->fd, events[i].events, ev->arg);    }
			if((events[i].events&EPOLLOUT)&&(ev->events&EPOLLOUT))  write event
			{
				ev->call_back(ev->fd, events[i].events, ev->arg);
			}
		}
	}

	//thread end
	for(int i = 0; i < pConfig->mWorkerThreads; i++)
	{
		uv_thread_join(&threads[i]);
	}

	//user
	for(map<void*, Conn*>::iterator iter = ConnMgr::mAllConns.begin(); iter != ConnMgr::mAllConns.end(); iter++)
	{
		iter->second->Destroy(false);
	}
	ConnMgr::mAllConns.clear();

	return rtn;
}

//void signal_term(uv_signal_t *handle, int signum)
//{
//	uv_stop(gLoop);
//	gGotQuitSignal = true;
//	uv_signal_stop(handle);
//}
//
//void signal_int(uv_signal_t *handle, int signum)
//{
//	uv_stop(gLoop);
//	gGotQuitSignal = true;
//	uv_signal_stop(handle);
//}

void worker_func(void* arg)
{
	while(true)
	{
		MsgNode* pMsgNode = MsgQueue::Singleton()->PopMsg();
		if(pMsgNode == NULL)
		{
			if(gGotQuitSignal)
			{
				break;
			}
			else
			{
				usleep(1000 * 200);//sleep use seconds, usleep use 10^-6 seconds
				continue;
			}
		}
		else
		{
			Router::Handle(pMsgNode->mConn, pMsgNode->mId, pMsgNode->mData, pMsgNode->mSize);
			DELETE(pMsgNode);
		}
	};
}


