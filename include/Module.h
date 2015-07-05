#ifndef _MODULE_H_
#define _MODULE_H_

typedef void (*ModuleInitFunc)();

class Module
{
public:
	static bool LoadModules();
	static void ReleaseModules();
private:
	static vector<void*> mModuleHandles;
};
#endif//_MODULE_H_
