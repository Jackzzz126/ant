#include "comm.h"
#include "router.h"
#include "log.h"

#include "benchMark.h"

map<int, MsgHandler> Router::mAllHandlers;
void Router::AddHandler(int msgId, MsgHandler handler)
{
	assert(handler);

	map<int, MsgHandler>::iterator iter;
	iter = mAllHandlers.find(msgId);
	if(iter != mAllHandlers.end())
	{
		Log::Error("Duplicate msgId added.", msgId);
	}
	else
	{
		mAllHandlers[msgId] = handler;
	}
}
void Router::Handle(void* conn, int msgId, char* data, int size)
{
	map<int, MsgHandler>::iterator iter;
	iter = mAllHandlers.find(msgId);
	if(iter != mAllHandlers.end())
	{
		mAllHandlers[msgId](conn, data, size);
	}
	else
	{
		Log::Error("Unknown msg id: %d.\n", msgId);
		DelBuff(&data);
	}
}
void Router::Init()
{
	AddHandler(-1, &BenchMark::Echo);
	AddHandler(-2, &BenchMark::DoubleEcho);
}




