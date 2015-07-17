#ifndef _LISTEN_H_
#define _LISTEN_H_

#include "sock.h"

class Listen : public Sock
{
public:
	Listen(int sock);
	~Listen();

	virtual void OnRead(int timeStamp);
	virtual void OnWrite(int timeStamp);

	void Init();
};

#endif//_LISTEN_H_
