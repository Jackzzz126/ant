#ifndef _WRITE_H_
#define _WRITE_H_

struct WriteReq
{
  uv_write_t mWrite;
  uv_buf_t mBuff;
  int mRef;

  WriteReq()
  {
	  mRef = 1;
  }
};

void OnWriteClose(uv_write_t *write, int status);
void OnWrite(uv_write_t *write, int status);

#endif//_WRITE_H_

