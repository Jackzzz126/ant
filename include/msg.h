#ifndef _MSG_H_
#define _MSG_H_

class MsgNode
{
public:
	MsgNode(void* conn, int msgId, char* data);
	~MsgNode();
private:
	void* mConn;
	int mId;
	char* mData;
public:
	MsgNode* mNext;
};

class MsgQueue
{
public:
	static MsgQueue* Singleton();
public:
	MsgQueue();
	~MsgQueue();
public:
	void PushMsg(void* conn, int msgId, char* data);
	MsgNode* PopMsg();
public:
private:
	MsgNode* mHead;
	MsgNode* mTail;

	pthread_mutex_t mMutex;

	static MsgQueue* mGlobalMsgQueue;
};

#endif//_MSG_H_