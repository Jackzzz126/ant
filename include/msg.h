#ifndef _MSG_H_
#define _MSG_H_

class MsgNode
{
public:
	MsgNode(int sock, int msgId, char* data, int size);
	~MsgNode();
public:
	int mSock;
	int mId;
	char* mData;
	int mSize;
public:
	MsgNode* mNext;
};

class MsgQueue
{
public:
	static MsgQueue* Singleton();
private:
	static MsgQueue* mMsgQueueSingleton;
private:
	MsgQueue();
	~MsgQueue();
public:
	void PushMsg(int sock, int msgId, char* data, int size);
	MsgNode* PopMsg();
public:
private:
	MsgNode* mHead;
	MsgNode* mTail;

	pthread_mutex_t mMsgQueueLock;
};

#endif//_MSG_H_
