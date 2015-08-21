#ifndef _SCRIPT_H_
#define _SCRIPT_H_

class Script
{
public:
	static Script* Singleton();
private:
	static Script* mScriptSingleton;
private:
	Script();
	~Script();

public:
	bool Test(int* sum, int x, int y);
	bool GetValue(const char* fileName, const char* key, string& value);
	bool GetValue(const char* fileName, const char* key, int* value);
	bool GetValue(const char* fileName, const char* key, double* value);
	bool Call(const char* fileName, const char* funcName, const char* fmt, ...);
private:
	void Init();
	void Close();
private:
	lua_State* mLuaState;
};

#endif//_SCRIPT_H_

