#include "comm.h"
#include "crypto.h"

#include<openssl/md5.h>

string Crypto::Md5(char* data, int len)
{
	byte md5[16];
	Md5(data, len, md5);

	return ToHex((const char*)md5, 16);
}

void Crypto::Md5(char* data, int len, byte md5[16])
{
	MD5_CTX ctx;

	MD5_Init(&ctx);
	MD5_Update(&ctx, data, len);
	MD5_Final(md5, &ctx);
}

