#include "packId.h"

void PackId::WritePackHead(char* buff, PackIdType packId, int len)
{
	*(int*)(buff) = packId ^ 0x79669966;
	*((int*)(buff) + 1) = len ^ 0x79669966;
}
