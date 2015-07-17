#ifndef _HTTPHANDLE_H_
#define _HTTPHANDLE_H_

typedef void (*HttpHandler)(int sock, char* data, int size);
class HttpHandle
{
private:
	static map<string, HttpHandler> mAllHttpHandlers;;
	static void AddHandler(const string& url, HttpHandler handler);

	static void Hello(int sock, char* data, int size);
public:
	static void Handle(int sock, const string& url, char* data, int size);
	static void Init();
};
#endif//_HTTPHANDLE_H_

