#ifndef _DEFINE_H_
#define _DEFINE_H_

typedef unsigned char byte;
typedef unsigned int uint;
typedef unsigned short ushort;

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

#define STATUS_SUCCESS 0
#define STATUS_WRONG_ARG -1
#define STATUS_SERVER_ERR -2

#define DELETE(p)		{ delete p; p = NULL; }

#endif//_DEFINE_H_
