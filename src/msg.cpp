#include "comm.h"
#include "msg.h"

MsgNode::MsgNode(int sock, int msgId, char* data, int size)
{
	mSock = sock;
	mId = msgId;
	mData = data;
	mSize = size;
	mNext = NULL;
}
MsgNode::~MsgNode()
{
}

MsgQueue* MsgQueue::mGlobalMsgQueue = NULL;
MsgQueue* MsgQueue::Singleton()
{
	if(mGlobalMsgQueue == NULL)
	{
		mGlobalMsgQueue = new MsgQueue();
	}
	return mGlobalMsgQueue;
}

MsgQueue::MsgQueue()
{
	mHead = NULL;
	mTail = NULL;
	pthread_mutex_init(&mMutex, NULL);
}
MsgQueue::~MsgQueue()
{
}

void MsgQueue::PushMsg(int sock, int msgId, char* data, int size)
{
	pthread_mutex_lock(&mMutex);
	MsgNode* pNode = new MsgNode(sock, msgId, data, size);
	if(mTail == NULL)
	{
		mHead = mTail = pNode;
	}
	else
	{
		mTail->mNext = pNode;
		mTail = pNode;
	}
	pthread_mutex_unlock(&mMutex);
}

MsgNode* MsgQueue::PopMsg()
{
	pthread_mutex_lock(&mMutex);
	MsgNode* ret = NULL;
	if(mHead == NULL)
	{
		ret = NULL;
	}
	else if(mHead == mTail)
	{
		MsgNode* pNode = mHead;
		mHead = mTail = NULL;
		ret = pNode;
	}
	else
	{
		MsgNode* pNode = mHead;
		mHead = mHead->mNext;
		ret = pNode;
	}
	pthread_mutex_unlock(&mMutex);
	return ret;
}
