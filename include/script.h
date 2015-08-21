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
	bool GetValue(const char* fileName, const char* key, string& value);
	bool GetValue(const char* fileName, const char* key, int* value);
	bool GetValue(const char* fileName, const char* key, double* value);

	bool GetValue(const char* fileName, const char* tableName, const char* key, string& value);
	bool GetValue(const char* fileName, const char* tableName, const char* key, int* value);
	bool GetValue(const char* fileName, const char* tableName, const char* key, double* value);

	/*Script* pScript = Script::Singleton();
	double x = 0.0;
	int y = 0;
	char* z = NULL;
	if(pScript->Call("lua/test.lua", "f", "dis|dis", 5.0, 5, "bbb", &x, &y, &z))
	{
		printf("sum is: %f %d %s\n", x, y, z);
	}*/
	bool Call(const char* fileName, const char* funcName, const char* fmt, ...);
private:
	void Init();
	void Close();
	bool DoFile(const char* fileName);
private:
	lua_State* mLuaState;
};

#endif//_SCRIPT_H_

