#include "comm.h"
#include "Module.h"

#include "Config.h"
#include "Log.h"


vector<void*> Module::mModuleHandles;

bool Module::LoadModules()
{
	Config* config = Config::Singleton();
	vector<string>::iterator iter = config->mModuleNames.begin();
	for(; iter != config->mModuleNames.end(); iter++)
	{
		void* moduleHandle = dlopen(("modules/" + *iter).c_str(), RTLD_LAZY);
		if(!moduleHandle)
		{
			char *errorStr = dlerror();;
			Log::Error("Error when load module: %s, %s.\n", (*iter).c_str(), errorStr);
			return false;
		}
		ModuleInitFunc moduleInitFunc = (ModuleInitFunc)dlsym(moduleHandle, "Init");
		if(!moduleInitFunc)
		{
			char *errorStr = dlerror();;
			Log::Error("Error when parse module init function: %s, %s\n", (*iter).c_str(), errorStr);
			return false;
		}
		mModuleHandles.push_back(moduleHandle);
		(*moduleInitFunc)();
	}
	return true;
}

void Module::ReleaseModules()
{
	vector<void*>::iterator iter = mModuleHandles.begin();
	for(; iter != mModuleHandles.end(); iter++)
	{
		dlclose(*iter);
	}
}


