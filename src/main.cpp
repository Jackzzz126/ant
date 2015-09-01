#include "comm.h"
#include "time.h"
#include "config.h"
#include "listen.h"
#include "udpListen.h"
#include "msg.h"
#include "router.h"
#include "poll.h"
#include "conn.h"
#include "schedule.h"
#include "scheduleTasks.h"
#include "packId.h"
#include "threadData.h"
#include "udp.h"

bool gGotQuitSignal = false;

void* WorkerFunc(void* arg);
void* DbFunc(void* arg);
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
			Log::Error("Daemonize error.\n");
			return 1;
		}
	}
	PackId::Init();
	Router::Init();

	//socket poll
	Poll* pPoll = Poll::Singleton();
	if(!pPoll->Create())
	{
		Log::Error("Create poll error.\n");
		return 1;
	}

	//signal
	signal(SIGINT, SignalHanle);
	signal(SIGTERM, SignalHanle);
	//listener
	int listenSock = socket(AF_INET, SOCK_STREAM, 0);
	Listen* pListener = new Listen(listenSock);
	if(pListener->Init() != 0)
	{
		Log::Error("Listen error: %s.\n", strerror(errno));
		return 1;
	}
	pPoll->Add(listenSock, pListener);

	int udpListenSock = socket(AF_INET, SOCK_DGRAM, 0);
	UdpMgr::mUdpSock = udpListenSock;
	UdpListen* pUdpListener = new UdpListen(udpListenSock);
	if(pUdpListener->Init() != 0)
	{
		Log::Error("Udp listen error: %s.\n", strerror(errno));
		return 1;
	}
	pPoll->Add(udpListenSock, pUdpListener);
	Log::Out("Server start at %s:%d %d.\n", pConfig->mIp.c_str(), pConfig->mPort, pConfig->mUdpPort);

	//thread start
	pthread_key_create(&ThreadData::mThreadKey, NULL);
	vector<pthread_t> threads;
	for(int i = 0; i < pConfig->mWorkerThreads; i++)
	{
		pthread_t workerThreadId;
		pthread_create(&workerThreadId, NULL, WorkerFunc, NULL);
		threads.push_back(workerThreadId);
	}
	for(int i = 0; i < pConfig->mDbThreads; i++)
	{
		pthread_t dbThreadId;
		pthread_create(&dbThreadId, NULL, DbFunc, NULL);
		threads.push_back(dbThreadId);
	}
	
	//add schedule task
	ScheduleMgr::AddTask(ScheduleTasks::CloseDeadSock, pConfig->mSockTimeout, true);

	//main loop
	while(!gGotQuitSignal)
	{
		int timeStamp = DateTime::GetTimeStamp();
		int fdCount = pPoll->Wait(ConnMgr::mSockConns.size() + 1, timeStamp);
		if(fdCount < 0)
		{
			if(errno != EINTR)
			{
				Log::Error("Poll wait error: %d %s.\n", fdCount, strerror(errno));
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
	for(map<int, Conn*>::iterator iter = ConnMgr::mSockConns.begin(); iter != ConnMgr::mSockConns.end(); iter++)
	{
		iter->second->Close(false);
	}
	ConnMgr::mSockConns.clear();

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
	ThreadData* pThreadData = new ThreadData();
	pthread_setspecific(ThreadData::mThreadKey, pThreadData);
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

	DELETE(pThreadData);
	return NULL;
}

void* DbFunc(void* arg)
{
	ThreadData* pThreadData = new ThreadData();
	pthread_setspecific(ThreadData::mThreadKey, pThreadData);
	while(true)
	{
		MsgNode* pMsgNode = DbMsgQueue::Singleton()->PopMsg();
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
			Router::DbHandle(pMsgNode->mSock, pMsgNode->mId, pMsgNode->mData, pMsgNode->mSize);
			DELETE(pMsgNode);
		}
	};

	DELETE(pThreadData);
	return NULL;
}


