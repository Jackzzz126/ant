#include "comm.h"
#include "luaState.h"

extern "C" int add(lua_State* L) 
{
	double op1 = luaL_checknumber(L,1);
	double op2 = luaL_checknumber(L,2);
	lua_pushnumber(L,op1 + op2);
	return 1;
}

LuaState::LuaState()
{
	mLuaState = NULL;
	Init();
}

LuaState::~LuaState()
{
	Close();
}
void LuaState::Init()
{
	if(mLuaState == NULL)
	{
		mLuaState = luaL_newstate();
		luaopen_base(mLuaState);
		luaopen_io(mLuaState);
		luaopen_string(mLuaState);
		luaopen_math(mLuaState);

		lua_register(mLuaState, "add", add);
	}
	lua_settop(mLuaState, 0);
}
void LuaState::Close()
{
	lua_close(mLuaState);
	mLuaState = NULL;
}
bool LuaState::DoFile(const char* fileName)
{
	Init();
	//run script
	if(luaL_dofile(mLuaState, fileName))
	{
		Log::Error("Error when run %s: %s.\n",
			fileName, lua_tostring(mLuaState, -1));
		Close();
		return false;
	}
	return true;
}

bool LuaState::GetValue(const char* fileName, const char* key, string& value)
{
	if(!DoFile(fileName))
		return false;

	lua_getglobal(mLuaState, key);

	if(!lua_isstring(mLuaState, -1))
	{
		Log::Error("Error when run %s: return value error.\n", fileName);
		Close();
		return false;
	}
	value = string(lua_tostring(mLuaState, -1));
	return true;
}

bool LuaState::GetValue(const char* fileName, const char* key, int* value)
{
	if(!DoFile(fileName))
		return false;

	lua_getglobal(mLuaState, key);

	if(!lua_isnumber(mLuaState, -1))
	{
		Log::Error("Error when run %s: return value error.\n", fileName);
		Close();
		return false;
	}
	*value = lua_tonumber(mLuaState, -1);
	return true;
}

bool LuaState::GetValue(const char* fileName, const char* key, double* value)
{
	if(!DoFile(fileName))
		return false;

	lua_getglobal(mLuaState, key);

	if(!lua_isnumber(mLuaState, -1))
	{
		Log::Error("Error when run %s: return value error.\n", fileName);
		Close();
		return false;
	}
	*value = lua_tonumber(mLuaState, -1);
	return true;
}
bool LuaState::GetValue(const char* fileName, const char* tableName, const char* key, string& value)
{
	if(!DoFile(fileName))
		return false;

	lua_getglobal(mLuaState, tableName);
	if (!lua_istable(mLuaState, -1))
	{
		Log::Error("Error when run %s: return value error.\n", fileName);
		Close();
		return false;
	}
	lua_pushstring(mLuaState, key);
	lua_gettable(mLuaState, -2);
	if(!lua_isstring(mLuaState, -1))
	{
		Log::Error("Error when run %s: return value error.\n", fileName);
		Close();
		return false;
	}
	value = string(lua_tostring(mLuaState, -1));
	return true;

}
bool LuaState::GetValue(const char* fileName, const char* tableName, const char* key, int* value)
{
	if(!DoFile(fileName))
		return false;

	lua_getglobal(mLuaState, tableName);
	if (!lua_istable(mLuaState, -1))
	{
		Log::Error("Error when run %s: return value error.\n", fileName);
		Close();
		return false;
	}
	lua_pushstring(mLuaState, key);
	lua_gettable(mLuaState, -2);
	if(!lua_isnumber(mLuaState, -1))
	{
		Log::Error("Error when run %s: return value error.\n", fileName);
		Close();
		return false;
	}
	*value = lua_tonumber(mLuaState, -1);
	return true;
}
bool LuaState::GetValue(const char* fileName, const char* tableName, const char* key, double* value)
{
	if(!DoFile(fileName))
		return false;

	lua_getglobal(mLuaState, tableName);
	if (!lua_istable(mLuaState, -1))
	{
		Log::Error("Error when run %s: return value error.\n", fileName);
		Close();
		return false;
	}
	lua_pushstring(mLuaState, key);
	lua_gettable(mLuaState, -2);
	if(!lua_isnumber(mLuaState, -1))
	{
		Log::Error("Error when run %s: return value error.\n", fileName);
		Close();
		return false;
	}
	*value = lua_tonumber(mLuaState, -1);
	return true;
}

//'d':double,'i':integer,'s':strings, '|': separator
bool LuaState::Call(const char* fileName, const char* funcName, const char* fmt, ...)
{
	if(!DoFile(fileName))
		return false;

	if(string(funcName) == "")
		return true;

	lua_getglobal(mLuaState, funcName);

	va_list args;
	va_start(args, fmt);

	int argCount = 0;
	int retCount = 0;
	const char* p = fmt;
	while(*p)
	{
		if(*p++ == '|')
			break;
		argCount++;
	}
	while(*p++)
	{
		retCount++;
	}
	luaL_checkstack(mLuaState, argCount, "lua error: too many arguments.");

	for(int i = 0; i < argCount; i++)
	{
		if(*(fmt+i) == 'd')
		{
			lua_pushnumber(mLuaState, va_arg(args, double));
		}
		else if(*(fmt+i) == 'i')
		{
			lua_pushnumber(mLuaState, va_arg(args, int));
		}
		else if(*(fmt+i) == 's')
		{
			lua_pushstring(mLuaState, va_arg(args, char *));
		}
		else
		{
			Log::Error("Error when call %s:%s: wrong format str.\n", fileName, funcName);
			Close();
			return false;
		}
	}

	if(lua_pcall(mLuaState, argCount, retCount, 0))
	{
		Log::Error("Error when run \"%s\".%s: %s.\n",
			fileName, funcName, lua_tostring(mLuaState, -1));
		Close();
		return false;
	}

	for(int i = argCount + 1; i < argCount + 1 + retCount; i++)
	{
		int index = -retCount + (i - argCount - 1);
		if(*(fmt+i) == 'd')
		{
			if(!lua_isnumber(mLuaState, index))
			{
				Log::Error("Error when run %s:%s: return value error.\n",
						fileName, funcName);
				Close();
				return false;
			}
			*va_arg(args, double *) = lua_tonumber(mLuaState, index);
		}
		else if(*(fmt+i) == 'i')
		{
			if(!lua_isnumber(mLuaState, index))
			{
				Log::Error("Error when run %s:%s: return value error.\n",
						fileName, funcName);
				Close();
				return false;
			}
			*va_arg(args, int *) = lua_tonumber(mLuaState, index);
		}
		else if(*(fmt+i) == 's')
		{
			if(!lua_isstring(mLuaState, index))
			{
				Log::Error("Error when run %s:%s: return value error.\n",
						fileName, funcName);
				Close();
				return false;
			}
			*va_arg(args, const char **) = lua_tostring(mLuaState, index);
		}
		else
		{
			Log::Error("Error when call %s:%s: wrong format str.\n", fileName, funcName);
			Close();
			return false;
		}
	}

	va_end(args);
	return true;
}


