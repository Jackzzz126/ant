#ifndef _UDPLISTEN_H_
#define _UDPLISTEN_H_

#include "sock.h"

class UdpListen : public Sock
{
public:
	static UdpListen* Singleton();
private:
	static UdpListen* mUdpListenSingleton;
private:
	UdpListen(int sock);
	~UdpListen();

public:
	virtual void OnRead(int timeStamp);
	virtual void OnWrite(int timeStamp);

	int Init();
};

#endif//_UDPLISTEN_H_
