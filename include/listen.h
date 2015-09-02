#ifndef _LISTEN_H_
#define _LISTEN_H_

#include "sock.h"

class Listen : public Sock
{
public:
	static Listen* Singleton();
private:
	static Listen* mListenSingleton;
private:
	Listen(int sock);
	~Listen();

public:
	virtual void OnRead(int timeStamp);
	virtual void OnWrite(int timeStamp);

	int Init();
};

#endif//_LISTEN_H_
