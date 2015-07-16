#include "comm.h"
#include "log.h"
#include "define.h"
#include "time.h"
#include "config.h"
#include "listen.h"
#include "gate.h"
#include "msg.h"
#include "router.h"
#include "poll.h"
#include "conn.h"
#include "event.h"

bool gGotQuitSignal = false;

void* workerFunc(void* arg);
void signalHanle(int signum);

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
	Poll* pPoll = Poll::Singleton();
	if(!pPoll->Create())
	{
		printf("Create poll error.\n");
		return 1;
	}

	//signal
	signal(SIGINT, signalHanle);
	signal(SIGTERM, signalHanle);
	//listener
	int listenSock = socket(AF_INET, SOCK_STREAM, 0);
	Listen* pListener = new Listen(listenSock);
	pPoll->Add(listenSock, pListener);

	sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(pConfig->mIp.c_str());
	sin.sin_port = htons(pConfig->mPort);
	bind(pListener->mSock, (const sockaddr*)&sin, sizeof(sin));
	listen(pListener->mSock, pConfig->mBacklog);
	Log::Out("Server start at %s:%d.\n", pConfig->mIp.c_str(), pConfig->mPort);

	//thread start
	vector<pthread_t> threads;
	for(int i = 0; i < pConfig->mWorkerThreads; i++)
	{
		pthread_t workerThreadId;
		pthread_create(&workerThreadId, NULL, workerFunc, NULL);
		threads.push_back(workerThreadId);
	}
	//main loop
	while(!gGotQuitSignal)
	{
		int fdCount = pPoll->Wait(ConnMgr::mAllConns.size() + 1);
		if(fdCount < 0)
		{
			if(errno != EINTR)
			{
				Log::Error("Poll wait error: %d.\n", fdCount);
				break;
			}
		}
		EventMgr::Update();
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
void signalHanle(int signum)
{
	gGotQuitSignal = true;
}

void* workerFunc(void* arg)
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


