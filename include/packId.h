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

		//TEST = 299,
		//BENCHMARK = 300,
		//MAX_PACKID = 300,
	};
};

#endif//_PACKID_H_

