#ifndef _PACKID_H_
#define _PACKID_H_

class PackId
{
public:
	enum PackIdType
	{
		// -1001 - -1005, for test
		NORMAL_ECHO = -1001,
		DOUBLE_ECHO = -1002,
		//1-50
		//LOGIN = 2,
		//LOGOUT = 3,

		//TOKEN = 11,

		////51-100
		//GETGAMESERVER = 51,
		//GETCOMBATSERVER = 52,
		//REGCOMBATSERVER = 53,
		//UNREGCOMBATSERVER = 54,

		////101-200
		//CREATECHARACTER = 101,
		//GETCHARACTER = 102,
		//INITCHARACTER = 102,
		//VALIDCHARACTER = 103,

		////201-300
		//JOINROOM = 201,

		//1900-1999, for admin http
	};
public:
	static void WritePackHead(char* buff, PackIdType packId, int len);
	static int GetPackIdByUrl(const string& url);
	static void Init();
private:
	static map<string, int> mUrlIdMap;
};

#endif//_PACKID_H_

