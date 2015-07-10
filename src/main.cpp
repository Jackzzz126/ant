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

	//thread start
	vector<uv_thread_t> threads;
	for(int i = 0; i < pConfig->mWorkerThreads; i++)
	{
		uv_thread_t workerThreadId;
		uv_thread_create(&workerThreadId, worker_func, NULL);
		threads.push_back(workerThreadId);
	}

	//timer
	//uv_timer_stop(&regTimer);
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


void signal_term(uv_signal_t *handle, int signum)
{
	uv_stop(gLoop);
	gGotQuitSignal = true;
	uv_signal_stop(handle);
}
void signal_int(uv_signal_t *handle, int signum)
{
	uv_stop(gLoop);
	gGotQuitSignal = true;
	uv_signal_stop(handle);
}


