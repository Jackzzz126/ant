#include "comm.h"
#include "schedule.h"

map<int, Schedule*> ScheduleMgr::mAllSchedules;
int ScheduleMgr::mScheduleIdSeed = 0;
int ScheduleMgr::mLastUpdateTime = DateTime::GetTimeStamp();

int ScheduleMgr::AddTask(ScheduleTask task, int interval, bool repeat)
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
bool ScheduleMgr::DelTask(int taskId)
{
	map<int, Schedule*>::iterator iter = mAllSchedules.find(taskId);
	if(iter != mAllSchedules.end())
	{
		delete iter->second;
		mAllSchedules.erase(taskId);
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
		vector<int> tasksToDel;

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
					tasksToDel.push_back(pSchedule->mId);
					delete pSchedule;
				}
			}
		}

		vector<int>::iterator idIter = tasksToDel.begin();
		for(; idIter != tasksToDel.end(); idIter++)
		{
			mAllSchedules.erase(*idIter);
		}
	}
}
