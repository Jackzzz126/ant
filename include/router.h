#ifndef _ROUTER_H_
#define _ROUTER_H_

typedef void (*MsgHandler)(int sock, int msgId, char* data, int size);

class Router
{
private:
	static map<int, MsgHandler> mIdHandles;;
	static map<int, MsgHandler> mIdDbHandles;;
	static void AddHandler(int msgId, MsgHandler handler);
	static void AddDbHandler(int msgId, MsgHandler handler);
public:
	static void Handle(int sock, int msgId, char* data, int size);
	static void DbHandle(int sock, int msgId, char* data, int size);
	static void Init();
};
#endif//_ROUTER_H_
