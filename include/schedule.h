#ifndef _SCHEDULE_H_
#define _SCHEDULE_H_

typedef void (*ScheduleTask)();
struct Schedule
{
	int mId;
	ScheduleTask mTask;
	int mInterval;
	bool mRepeat;
	int mTimeDelta;
	Schedule()
	{
		mTask = NULL;
		mInterval = 0;
		mRepeat = true;
		mTimeDelta = 0;
	};
};

class ScheduleMgr
{
private:
	static map<int, Schedule*> mAllSchedules;
	static int mScheduleIdSeed;
	static int mLastUpdateTime;
public:
	//interval in seconds
	static int AddSchedule(ScheduleTask task, int interval, bool repeat);
	static bool DelSchedule(int eventId);
	static void Update(int timeStamp);
};
#endif//_SCHEDULE_H_
