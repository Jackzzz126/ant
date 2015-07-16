#include "comm.h"
#include "event.h"

map<int, Event> EventMgr::mAllEvents;
int EventMgr::mEventIdSeed = 0;

int EventMgr::AddEvent(EventHandle handle, int interval, bool repeat)
{
	if(handle == NULL)
		return 0;
	Event event;
	event.mHandle = handle;
	event.mInterval = interval;
	event.mRepeat = repeat;
	mEventIdSeed++;
	event.mId = mEventIdSeed;
	mAllEvents[mEventIdSeed] = event;
	return mEventIdSeed;
}
bool EventMgr::DelEvent(int eventId)
{
	map<int, Event>::iterator iter = mAllEvents.find(eventId);
	if(iter != mAllEvents.end())
	{
		mAllEvents.erase(eventId);
		return true;
	}
	else
	{
		return false;
	}
}
