#ifndef _SOCK_H_
#define _SOCK_H_

class Sock 
{
public:
	Sock(int sock);
	virtual ~Sock();

	virtual void OnRead();
	virtual void OnWrite();
private:
	void SetNoBlock();
public:
	int mSock;
};

#endif//_SOCK_H_