#include "comm.h"
#include "packId.h"

map<string, int> PackId::mUrlIdMap;

void PackId::WritePackHead(char* buff, PackIdType packId, int len)
{
	*(int*)(buff) = packId ^ 0x79669966;
	*((int*)(buff) + 1) = len ^ 0x79669966;
}

int PackId::GetPackIdByUrl(const string& url)
{
	map<string, int>::iterator iter = mUrlIdMap.find(url);
	if(iter == mUrlIdMap.end())
	{
		return 0;
	}
	else
	{
		return iter->second;
	}
}

void PackId::Init()
{
	mUrlIdMap["/hello"] = 900;
}
