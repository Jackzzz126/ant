#ifndef _POLL_H_
#define _POLL_H_

class Poll
{
public:
	static Poll* Singleton();
private:
	static Poll* mPollSingleton;
private:
	Poll();
	~Poll();
public:
	bool Create();
	void Close();
	bool Add(int sock, void *ud);
	void SetWrite(int sock, void *ud);
	void Del(int sock);
	int Wait(int size, int timeStamp);
private:
	int mPoll;
};
#endif//_POLL_H_
