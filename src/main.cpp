#include "comm.h"
#include "time.h"
#include "config.h"
#include "listen.h"
#include "gate.h"
#include "msg.h"
#include "router.h"
#include "poll.h"
#include "conn.h"
#include "schedule.h"
#include "scheduleTasks.h"
#include "httpHandle.h"
#include "packId.h"

bool gGotQuitSignal = false;

void* WorkerFunc(void* arg);
void SignalHanle(int signum);
void CloseDeadSock();

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
	PackId::Init();
	Router::Init();
	HttpHandle::Init();

	//socket poll
	Poll* pPoll = Poll::Singleton();
	if(!pPoll->Create())
	{
		printf("Create poll error.\n");
		return 1;
	}

	//signal
	signal(SIGINT, SignalHanle);
	signal(SIGTERM, SignalHanle);
	//listener
	int listenSock = socket(AF_INET, SOCK_STREAM, 0);
	Listen* pListener = new Listen(listenSock);
	pListener->Init();
	pPoll->Add(listenSock, pListener);
	Log::Out("Server start at %s:%d.\n", pConfig->mIp.c_str(), pConfig->mPort);

	//thread start
	vector<pthread_t> threads;
	for(int i = 0; i < pConfig->mWorkerThreads; i++)
	{
		pthread_t workerThreadId;
		pthread_create(&workerThreadId, NULL, WorkerFunc, NULL);
		threads.push_back(workerThreadId);
	}
	
	//add schedule task
	ScheduleMgr::AddTask(ScheduleTasks::CloseDeadSock, pConfig->mSockTimeout, true);

	//main loop
	while(!gGotQuitSignal)
	{
		int timeStamp = DateTime::GetTimeStamp();
		int fdCount = pPoll->Wait(ConnMgr::mAllConns.size() + 1, timeStamp);
		if(fdCount < 0)
		{
			if(errno != EINTR)
			{
				Log::Error("Poll wait error: %d.\n", fdCount);
				break;
			}
		}
		ScheduleMgr::Update(timeStamp);
	}

	//thread end
	for(int i = 0; i < pConfig->mWorkerThreads; i++)
	{
		pthread_join(threads[i], NULL);
	}

	//user
	for(map<int, Conn*>::iterator iter = ConnMgr::mAllConns.begin(); iter != ConnMgr::mAllConns.end(); iter++)
	{
		iter->second->Close(false);
	}
	ConnMgr::mAllConns.clear();

	//delete singleton
	pPoll->Close();
	pConfig->Release();

	return 0;
}
void SignalHanle(int signum)
{
	gGotQuitSignal = true;
}

void* WorkerFunc(void* arg)
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
			Router::Handle(pMsgNode->mSock, pMsgNode->mId, pMsgNode->mData, pMsgNode->mSize);
			DELETE(pMsgNode);
		}
	};
	return NULL;
}


