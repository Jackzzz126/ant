#ifndef _DEFINE_H_
#define _DEFINE_H_

/*#ifdef DEBUG
	#define ASSERT(x) \
	{ \
		if ( !(x) ) \
			printf( "[Assert]%s,%d: %s\n", __FILE__, __LINE__, #x ); \
	}
#else
	#define ASSERT(x)
#endif
*/

//typedef unsigned int uint;
# if __WORDSIZE == 64
typedef long int		int64;
typedef unsigned long int	uint64;
# else
__extension__
typedef long long int		int64;
typedef unsigned long long int	uint64;
# endif

#define MAX_INT 2147483647
#define MIN_INT -2147483648
#define BUFF_UNIT 1024
#define HEAD_LENGTH 8

#define DELETE(p)		{ delete p; p = NULL; }

char* NewBuff(int size);
void DelBuff(char** pp);

class RefBuff
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
	void UnRef()
	{
		mRef--;
		if(mRef<=0)
		{
			DelBuff(&mBuff);
			delete this;
		}
	}
};
#endif//_DEFINE_H_
