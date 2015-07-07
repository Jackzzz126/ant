#ifndef _PROTO_H_
#define _PROTO_H_

class Buffer
{
public:
	char* mBuff;
	int mSize;
	Buffer()
	{
		mBuff = NULL;
		mSize = 0;
	}
	~Buffer()
	{
		delete []mBuff;
	}
};

class Proto
{
public:
	enum Type
	{
		VARINT = 0,//int32
		//BIT64 = 1,
		LENGTH_DELIMITED = 2,//string, buff
		//START_GROUP = 3,
		//END_GROUP = 4,
		INT_ARRAY = 3,//intArray
		FLOAT_ARRAY = 4,//floatArray
		BIT32 = 5,//float
	};
	static int BUFF_UNIT;
public:
	Proto();
	~Proto(){};

	bool ReadInt(int* value);
	bool ReadFloat(float* value);
	bool ReadString(string& str);
	bool ReadBuffer(Buffer& buff);
	bool ReadIntArray(vector<int>& intArray);
	bool ReadFloatArray(vector<float>& floatArray);

	bool ReadTag(Proto::Type& type, int& number);

	void WriteInt(int value);
	void WriteFloat(float value);
	void WriteString(const string& str);
	void WriteBuffer(const Buffer& buff);
	void WriteIntArray(const vector<int>& intArray);
	void WriteFloatArray(const vector<float>& floatArray);

	void WriteTag(Proto::Type type, int number);
	
	void InitBuff(char* buff, int size);
	void ToHex();
public:
	char* mBuff;
	int mSize;
	char* mCurPos;
};
#endif//_PROTO_H_
