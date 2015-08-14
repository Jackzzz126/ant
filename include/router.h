#ifndef _ROUTER_H_
#define _ROUTER_H_

typedef void (*MsgHandler)(int sock, char* data, int size);

class Router
{
private:
	static map<int, MsgHandler> mIdHandles;;
	static void AddHandler(int msgId, MsgHandler handler);
public:
	static void Handle(int sock, int msgId, char* data, int size);
	static void Init();
};
#endif//_ROUTER_H_
