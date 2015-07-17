#include "comm.h"
#include "event.h"
#include "dateTime.h"

map<int, Event*> EventMgr::mAllEvents;
int EventMgr::mEventIdSeed = 0;
int EventMgr::mLastUpdateTime = DateTime::GetTimeStamp();

int EventMgr::AddEvent(EventHandle handle, int interval, bool repeat)
{
	if(handle == NULL)
		return 0;
	Event* pEv = new Event();
	pEv->mHandle = handle;
	pEv->mInterval = interval;
	pEv->mRepeat = repeat;
	mEventIdSeed++;
	pEv->mId = mEventIdSeed;
	mAllEvents[mEventIdSeed] = pEv;
	return mEventIdSeed;
}
bool EventMgr::DelEvent(int eventId)
{
	map<int, Event*>::iterator iter = mAllEvents.find(eventId);
	if(iter != mAllEvents.end())
	{
		delete iter->second;
		mAllEvents.erase(eventId);
		return true;
	}
	else
	{
		return false;
	}
}

void EventMgr::Update(int timeStamp)
{
	int timeDelta = timeStamp - mLastUpdateTime;
	if(timeDelta > 0)
	{
		mLastUpdateTime += timeDelta;
		vector<int> eventsToDel;

		map<int, Event*>::iterator iter = mAllEvents.begin();
		for(; iter != mAllEvents.end(); iter++)
		{
			Event* pEv = iter->second;
			pEv->mTimeDelta += timeDelta;
			if(pEv->mTimeDelta >= pEv->mInterval)
			{
				pEv->mTimeDelta -= pEv->mInterval;
				pEv->mHandle();
				if(!pEv->mRepeat)
				{
					delete pEv;
					eventsToDel.push_back(pEv->mId);
				}
			}
		}

		vector<int>::iterator idIter = eventsToDel.begin();
		for(; idIter != eventsToDel.end(); idIter++)
		{
			mAllEvents.erase(*idIter);
		}
	}
}
