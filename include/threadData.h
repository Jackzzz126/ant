#ifndef _THREADDATA_H_
#define _THREADDATA_H_

class ThreadData
{
public:
	static pthread_key_t mThreadKey;
public:
	ThreadData();
	virtual ~ThreadData();
public:
};

#endif//_THREADDATA_H_

