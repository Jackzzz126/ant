#ifndef _WRITE_H_
#define _WRITE_H_

void OnWriteClose(uv_write_t *req, int status);
void OnWrite(uv_write_t *req, int status);
void OnWriteNoFree(uv_write_t *req, int status);
#endif//_WRITE_H_

