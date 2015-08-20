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
private:
	void Init();
	void Close();
private:
	lua_State* mLuaState;
};

#endif//_SCRIPT_H_

