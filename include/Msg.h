#ifndef _MSG_H_
#define _MSG_H_

class MsgNode
{
public:
	MsgNode(int msgId, char* data);
	~MsgNode();
public:
	int mId;
	char* mData;
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
	MsgNode* mHead;
	MsgNode* mTail;
};

#endif//_MSG_H_
