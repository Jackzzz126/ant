#include "comm.h"
#include "httpUtil.h"
#include "strUtil.h"
#include "conn.h"

void HttpUtil::ResStr(int sock, const string& str)
{
	char buff[BUFF_UNIT];
	StrUtil::Format(buff, BUFF_UNIT, "HTTP/1.1 200 OK\r\nContent-Lenght:%d\r\nContent-Type:'text/plain'\r\n\r\n%s", str.length(), str.c_str());

	int len = strlen(buff);
	RefBuff* pRefBuff = new RefBuff(len, 1);
	memcpy(pRefBuff->mBuff, buff, len);
	ConnMgr::SendToOne(sock, pRefBuff);
}


