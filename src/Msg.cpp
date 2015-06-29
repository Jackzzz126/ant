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

MsgQueue::MsgQueue()
{
	mHead = NULL;
	mTail = NULL;
}
MsgQueue::~MsgQueue()
{
}

void MsgQueue::PushMsg(int msgId, char* data)
{
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
}

MsgNode* MsgQueue::PopMsg()
{
	if(mHead == NULL)
	{
		return NULL;
	}
	else if(mHead == mTail)
	{
		MsgNode* pNode = mHead;
		mHead = mTail = NULL;
		return pNode;
	}
	else
	{
		MsgNode* pNode = mHead;
		mHead = mHead->mNext;
		return pNode;
	}
}
