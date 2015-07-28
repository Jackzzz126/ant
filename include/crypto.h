#ifndef _CRYPTO_H_
#define _CRYPTO_H_

class Crypto
{
public:
	static string Md5(char* data, int len);
	static void Md5(char* data, int len, byte md5[16]);
	static string Base64(const char* data, int len);
};

#endif//_CRYPTO_H_

