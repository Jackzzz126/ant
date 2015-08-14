#include "comm.h"
#include "router.h"

#include "benchMark.h"
#include "admin.h"

map<int, MsgHandler> Router::mIdHandles;
void Router::AddHandler(int msgId, MsgHandler handler)
{
	assert(handler);

	map<int, MsgHandler>::iterator iter;
	iter = mIdHandles.find(msgId);
	if(iter != mIdHandles.end())
	{
		Log::Error("Duplicate msgId added.", msgId);
	}
	else
	{
		mIdHandles[msgId] = handler;
	}
}
void Router::Handle(int sock, int msgId, char* data, int size)
{
	map<int, MsgHandler>::iterator iter;
	iter = mIdHandles.find(msgId);
	if(iter != mIdHandles.end())
	{
		mIdHandles[msgId](sock, data, size);
	}
	else
	{
		Log::Error("Unknown msg id: %d.\n", msgId);
		DelBuff(&data);
	}
}
void Router::Init()
{
	AddHandler(-1, BenchMark::Echo);
	AddHandler(-2, BenchMark::DoubleEcho);

	AddHandler(-10, BenchMark::Reg);
	AddHandler(-11, BenchMark::Login);
	AddHandler(-12, BenchMark::Move);

	AddHandler(900, Admin::Hello);
}




