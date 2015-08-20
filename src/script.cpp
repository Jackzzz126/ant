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

	if(luaL_dofile(mLuaState, fileName.c_str()))
	{
		Log::Error("Error when run script %s:%s.\n",
			fileName.c_str(), lua_tostring(mLuaState, -1));
		Close();
		return false;
	}
	else
	{
		return true;
	}

}

