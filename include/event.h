#ifndef _EVENT_H_
#define _EVENT_H_

typedef void (*EventHandle)();
struct Event
{
	int mId;
	EventHandle mHandle;
	int mInterval;
	bool mRepeat;
	int mTimeDelta;
	Event()
	{
		mHandle = NULL;
		mInterval = 0;
		mRepeat = true;
		mTimeDelta = 0;
	}
};

class EventMgr
{
private:
	static map<int, Event> mAllEvents;
	static int mEventIdSeed;
public:
	static int AddEvent(EventHandle handle, int interval, bool repeat = false);
	static bool DelEvent(int eventId);
};
#endif//_EVENT_H_
