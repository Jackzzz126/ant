#include "benchMark.h"

#include "comm.h"
#include "packId.h"
#include "conn.h"

namespace BenchMark
{//begin of name space

void Echo(int sock, char* data, int size)
{
	DelBuff(&data);
}
void DoubleEcho(int sock, char* data, int size)
{
	DelBuff(&data);
}

}//end of name space

