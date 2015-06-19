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

typedef unsigned int uint;
# if __WORDSIZE == 64
typedef long int		int64;
typedef unsigned long int	uint64;
# else
__extension__
typedef long long int		int64;
typedef unsigned long long int	uint64;
# endif

#define MAX_INT 2147483647;
#define MIN_INT -2147483648;


#define DELETE(p)		{ delete p; p = NULL; }

string GetClientAddr(uv_stream_t *conn);

uv_buf_t NewUvBuff(int size);
void DelUvBuff(uv_buf_t& buff);
char* NewBuff(int size);
void DelBuff(char** pp);

typedef struct {
  uv_write_t req;
  uv_buf_t buf;
} write_req_t;


#endif//_DEFINE_H_
