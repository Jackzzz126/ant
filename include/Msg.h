#include<pthread.h>

#ifndef _MSG_H_
#define _MSG_H_

class MsgNode
{
public:
	MsgNode(int msgId, char* data);
	~MsgNode();
private:
	int mId;
	char* mData;
public:
	MsgNode* mNext;
};

class MsgQueue
{
public:
	MsgQueue();
	~MsgQueue();
public:
	void PushMsg(int msgId, char* data);
	MsgNode* PopMsg();
public:
private:
	MsgNode* mHead;
	MsgNode* mTail;

	pthread_mutex_t* mMutex;
};

#endif//_MSG_H_
