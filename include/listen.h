#ifndef _LISTEN_H_
#define _LISTEN_H_

#include "sock.h"

class Listen : public Sock
{
public:
	Listen(int sock);
	~Listen();

	virtual void OnRead();
	virtual void OnWrite();

	void Init();
};

#endif//_LISTEN_H_
