#include "comm.h"
#include "Msg.h"

MsgNode::MsgNode(int msgId, char* data)
{
	mId = msgId;
	mData = data;
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

void MsgQueue::PushMsg(int msgId, char* data)
{
	pthread_mutex_lock(&mMutex);
	MsgNode* pNode = new MsgNode(msgId, data);
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
