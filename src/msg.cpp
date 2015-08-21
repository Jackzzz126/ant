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

//************************************************
MsgQueue* MsgQueue::mMsgQueueSingleton = NULL;
MsgQueue* MsgQueue::Singleton()
{
	if(mMsgQueueSingleton == NULL)
	{
		mMsgQueueSingleton = new MsgQueue();
	}
	return mMsgQueueSingleton;
}

MsgQueue::MsgQueue()
{
	mHead = NULL;
	mTail = NULL;
	pthread_mutex_init(&mMsgQueueLock, NULL);
}
MsgQueue::~MsgQueue()
{
}

void MsgQueue::PushMsg(int sock, int msgId, char* data, int size)
{
	pthread_mutex_lock(&mMsgQueueLock);
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
	pthread_mutex_unlock(&mMsgQueueLock);
}

MsgNode* MsgQueue::PopMsg()
{
	pthread_mutex_lock(&mMsgQueueLock);
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
	pthread_mutex_unlock(&mMsgQueueLock);
	return ret;
}

//************************************************
DbMsgQueue* DbMsgQueue::mDbMsgQueueSingleton = NULL;
DbMsgQueue* DbMsgQueue::Singleton()
{
	if(mDbMsgQueueSingleton == NULL)
	{
		mDbMsgQueueSingleton = new DbMsgQueue();
	}
	return mDbMsgQueueSingleton;
}

DbMsgQueue::DbMsgQueue()
{
	mHead = NULL;
	mTail = NULL;
	pthread_mutex_init(&mDbMsgQueueLock, NULL);
}
DbMsgQueue::~DbMsgQueue()
{
}

void DbMsgQueue::PushMsg(int sock, int msgId, char* data, int size)
{
	pthread_mutex_lock(&mDbMsgQueueLock);
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
	pthread_mutex_unlock(&mDbMsgQueueLock);
}

MsgNode* DbMsgQueue::PopMsg()
{
	pthread_mutex_lock(&mDbMsgQueueLock);
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
	pthread_mutex_unlock(&mDbMsgQueueLock);
	return ret;
}
