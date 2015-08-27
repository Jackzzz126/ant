#ifndef _REFBUFF_H_
#define _REFBUFF_H_

#include "util.h"

struct RefBuff
{
	char* mBuff;
	int mLen;
	int mRef;
	RefBuff(int len, int ref)
	{
		mBuff = NewBuff(len);
		mLen = len;
		mRef = ref;
	};
	~RefBuff()
	{
		DelBuff(&mBuff);
	};
	void Unref()
	{
		mRef--;
		if(mRef<=0)
		{
			DelBuff(&mBuff);
			delete this;
		}
	}
};

#endif//_REFBUFF_H_
