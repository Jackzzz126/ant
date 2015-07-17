#ifndef _POLL_H_
#define _POLL_H_

class Poll
{
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

	static Poll* Singleton();
private:
	int mPoll;

	static Poll* mGlobalPoll;
};
#endif//_POLL_H_
