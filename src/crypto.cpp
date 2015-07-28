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

string Crypto::Base64(const char* data, int len)
{
	const char EncodeTable[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	char* buff = NewBuff(len * 2);
	unsigned char Tmp[4]={0};
	for(int i = 0; i < (len/3); i++)
	{
		Tmp[1] = *data++;
		Tmp[2] = *data++;
		Tmp[3] = *data++;
		sprintf(buff + i * 4 + 0, "%c", EncodeTable[Tmp[1] >> 2]);
		sprintf(buff + i * 4 + 1, "%c", EncodeTable[((Tmp[1] << 4) | (Tmp[2] >> 4)) & 0x3F]);
		sprintf(buff + i * 4 + 2, "%c", EncodeTable[((Tmp[2] << 2) | (Tmp[3] >> 6)) & 0x3F]);
		sprintf(buff + i * 4 + 3, "%c", EncodeTable[Tmp[3] & 0x3F]);
	}

	int Mod=len % 3;
	if(Mod==1)
	{
		Tmp[1] = *data++;
		sprintf(buff + (len/3) * 4 + 0, "%c", EncodeTable[(Tmp[1] & 0xFC) >> 2]);
		sprintf(buff + (len/3) * 4 + 1, "%c", EncodeTable[((Tmp[1] & 0x03) << 4)]);
		sprintf(buff + (len/3) * 4 + 2, "%c", '=');
		sprintf(buff + (len/3) * 4 + 3, "%c", '=');
	}
	else if(Mod==2)
	{
		Tmp[1] = *data++;
		Tmp[2] = *data++;
		sprintf(buff + (len/3) * 4 + 0, "%c", EncodeTable[(Tmp[1] & 0xFC) >> 2]);
		sprintf(buff + (len/3) * 4 + 1, "%c", EncodeTable[((Tmp[1] & 0x03) << 4) | ((Tmp[2] & 0xF0) >> 4)]);
		sprintf(buff + (len/3) * 4 + 2, "%c", EncodeTable[((Tmp[2] & 0x0F) << 2)]);
		sprintf(buff + (len/3) * 4 + 3, "%c", '=');
	}
	buff[(len/3) * 4 + 4] = '\0';

	return string(buff);
}


