#include "comm.h"
#include "schedule.h"
#include "dateTime.h"

map<int, Schedule*> ScheduleMgr::mAllSchedules;
int ScheduleMgr::mScheduleIdSeed = 0;
int ScheduleMgr::mLastUpdateTime = DateTime::GetTimeStamp();

int ScheduleMgr::AddSchedule(ScheduleTask task, int interval, bool repeat)
{
	if(task == NULL || interval < 1)
		return 0;
	Schedule* pSchedule = new Schedule();
	pSchedule->mTask = task;
	pSchedule->mInterval = interval;
	pSchedule->mRepeat = repeat;
	mScheduleIdSeed++;
	pSchedule->mId = mScheduleIdSeed;
	mAllSchedules[mScheduleIdSeed] = pSchedule;
	return mScheduleIdSeed;
}
bool ScheduleMgr::DelSchedule(int eventId)
{
	map<int, Schedule*>::iterator iter = mAllSchedules.find(eventId);
	if(iter != mAllSchedules.end())
	{
		delete iter->second;
		mAllSchedules.erase(eventId);
		return true;
	}
	else
	{
		return false;
	}
}

void ScheduleMgr::Update(int timeStamp)
{
	int timeDelta = timeStamp - mLastUpdateTime;
	if(timeDelta > 0)
	{
		mLastUpdateTime += timeDelta;
		vector<int> eventsToDel;

		map<int, Schedule*>::iterator iter = mAllSchedules.begin();
		for(; iter != mAllSchedules.end(); iter++)
		{
			Schedule* pSchedule = iter->second;
			pSchedule->mTimeDelta += timeDelta;
			if(pSchedule->mTimeDelta >= pSchedule->mInterval)
			{
				pSchedule->mTimeDelta -= pSchedule->mInterval;
				pSchedule->mTask();
				if(!pSchedule->mRepeat)
				{
					eventsToDel.push_back(pSchedule->mId);
					delete pSchedule;
				}
			}
		}

		vector<int>::iterator idIter = eventsToDel.begin();
		for(; idIter != eventsToDel.end(); idIter++)
		{
			mAllSchedules.erase(*idIter);
		}
	}
}
