#ifndef _SOCK_H_
#define _SOCK_H_

class Sock 
{
public:
	Sock(int sock);
	virtual ~Sock();

	virtual void OnRead(int timeStamp);
	virtual void OnWrite(int timeStamp);
private:
	void SetNoBlock();
public:
	int mSock;
	int mLastActiveTime;
};

#endif//_SOCK_H_
