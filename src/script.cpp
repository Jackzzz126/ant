#include "comm.h"
#include "script.h"

Script* Script::mScriptSingleton = NULL;
Script* Script::Singleton()
{
	if(mScriptSingleton == NULL)
	{
		mScriptSingleton = new Script();
	}
	return mScriptSingleton;
}

Script::Script()
{
	mLuaState = NULL;
	Init();
}

Script::~Script()
{
	Close();
}
void Script::Init()
{
	if(mLuaState == NULL)
	{
		mLuaState = luaL_newstate();
		luaopen_base(mLuaState);
		luaopen_io(mLuaState);
		luaopen_string(mLuaState);
		luaopen_math(mLuaState);
	}
	lua_settop(mLuaState, 0);
}
void Script::Close()
{
	lua_close(mLuaState);
	mLuaState = NULL;
}

bool Script::Test(int* sum, int x, int y)
{
	Init();
	string fileName = "lua/test.lua";
	string funcName = "xxx";
	
	//run script
	if(luaL_dofile(mLuaState, fileName.c_str()))
	{
		Log::Error("Error when run \"%s\": %s.\n",
			fileName.c_str(), lua_tostring(mLuaState, -1));
		Close();
		return false;
	}

	lua_getglobal(mLuaState, funcName.c_str());
	lua_pushnumber(mLuaState, x);
	lua_pushnumber(mLuaState, y);
	//call function
	if(lua_pcall(mLuaState, 2, 1, 0))//2 arguments, 1 result, no error handle
	{
		Log::Error("Error when run \"%s\".%s: %s.\n",
			fileName.c_str(), funcName.c_str(), lua_tostring(mLuaState, -1));
		Close();
		return false;
	}
	else
	{
		if(!lua_isnumber(mLuaState, -1))
		{
			Log::Error("Error when run %s:%s: return value error.\n",
				fileName.c_str(), funcName.c_str());
			Close();
			return false;
		}
		*sum = lua_tonumber(mLuaState, -1);
		lua_pop(mLuaState, 1);
		return true;
	}
}

