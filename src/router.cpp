#include "comm.h"
#include "router.h"
#include "log.h"

map<int, MsgHandler> Router::mGlobalRouter;
void Router::AddHandler(int packId, MsgHandler handler)
{
	assert(handler);

	map<int, MsgHandler>::iterator iter;
	iter = Router::mGlobalRouter.find(packId);
	if(iter != Router::mGlobalRouter.end())
	{
		Log::Error("Duplicate packid added.", packId);
	}
	else
	{
		Router::mGlobalRouter[packId] = handler;
	}
}
void Router::Init()
{
}


