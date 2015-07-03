#ifndef _ROUTER_H_
#define _ROUTER_H_

typedef void (*MsgHandler)(int packId, char* data, int len);

class Router
{
private:
	static map<int, MsgHandler> mGlobalRouter;;
	static void AddHandler(int packId, MsgHandler handler);
public:
	static void Init();
};
#endif//_ROUTER_H_
