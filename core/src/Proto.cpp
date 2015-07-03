#include "comm.h"
#include "Log.h"
#include "Proto.h"

int Proto::BUFF_UNIT = 512;

Proto::Proto()
{
	mBuff = NULL;
	mSize = 0;
	mCurPos = mBuff;
}

bool Proto::ReadInt(int* value)
{
	int rtn = 0;
	int bitsRead = 0;
	do
	{
		if((mCurPos - mBuff) > mSize)
			return false;
		unsigned char byte = *mCurPos++;
		if((byte & 0x80) > 0)
		{
			rtn |= ((byte & 0x7f) << bitsRead);
		}
		else
		{
			rtn |= (byte << bitsRead);
			break;
		}
		bitsRead += 7;
		if(bitsRead > 35)
		{
			return false;
		}
	}while(true);
	*value = rtn;
	return true;
}

bool Proto::ReadFloat(float* value)
{
	int rtn = 0;
	if((mCurPos + 4 - mBuff) > mSize)
		return false;
	rtn = *((int*)mCurPos);
	mCurPos += 4;
	rtn = ntohl(rtn);
	*value = (float)rtn;
	return true;
}

bool Proto::ReadString(string& str)
{
	int strLen = 0; 
	if(!ReadInt(&strLen))
	{
		return false;
	}
	if((mCurPos + strLen - mBuff) > mSize)
		return false;
	string strRead = string(mCurPos, strLen);
	if((unsigned int)strLen != strRead.length())
	{
		return false;
	}
	mCurPos += strLen;
	str = strRead;
	return true;
}

bool Proto::ReadBuffer(Buffer& buff)
{
	int buffLen = 0; 
	if(!ReadInt(&buffLen))
	{
		return false;
	}
	if((mCurPos + buffLen - mBuff) > mSize)
		return false;

	buff.mBuff = NewBuff((buffLen/Proto::BUFF_UNIT + 1) * Proto::BUFF_UNIT);
	memcpy(buff.mBuff, mCurPos, buffLen);
	buff.mSize = buffLen;

	mCurPos += buffLen;
	return true;
}

bool Proto::ReadIntArray(vector<int>& intArray)
{
	int arrayLen = 0;
	if(!ReadInt(&arrayLen))
	{
		return false;
	}
	for(int i = 0; i < arrayLen; i++)
	{
		int value = 0;
		if(!ReadInt(&value))
		{
			return false;
		}
		intArray.push_back(value);
	}

	return true;
}

bool Proto::ReadFloatArray(vector<float>& floatArray)
{
	int arrayLen = 0;
	if(!ReadInt(&arrayLen))
	{
		return false;
	}
	for(int i = 0; i < arrayLen; i++)
	{
		float value = 0;
		if(!ReadFloat(&value))
		{
			return false;
		}
		floatArray.push_back(value);
	}

	return true;
}

bool Proto::ReadTag(Proto::Type& type, int& number)
{
	int value = 0;
	if(!ReadInt(&value))
	{
		return false;
	}
	type = (Proto::Type)(value & 0x07);
	number = (value >> 3) & 0x1FFFFFFF;
	return true;
}

void Proto::WriteInt(int value)
{
	int bitsWrite = 0;
	do
	{
		if((value & ~0x7f) == 0)
		{
			*(mCurPos++) = (char)value;
			bitsWrite += 7;
			break;
		}
		else
		{
			*(mCurPos++) = (char)((char)(value & 0x7f) | 0x80);
			bitsWrite += 7;
			value >>= 7;
			value &= 0x01FFFFFF;
		}
	}while(bitsWrite < 35);
}

void Proto::WriteFloat(float value)
{
	int tmp = (int)value;
	*((int*)mCurPos) = htonl(tmp);
	mCurPos += 4;
}

void Proto::WriteString(const string& str)
{
	int strLen = str.length();
	WriteInt(strLen);
	memcpy( mCurPos, str.c_str(), strLen);
	mCurPos += strLen;
}

void Proto::WriteBuffer(const Buffer& buff)
{
	WriteInt(buff.mSize);
	memcpy( mCurPos, buff.mBuff, buff.mSize);
	mCurPos += buff.mSize;
}

void Proto::WriteIntArray(const vector<int>& intArray)
{
	int arrayLen = intArray.size();
	WriteInt(arrayLen);
	for(int i = 0; i < arrayLen; i++)
	{
		WriteInt(intArray[i]);
	}
}

void Proto::WriteFloatArray(const vector<float>& floatArray)
{
	int arrayLen = floatArray.size();
	WriteInt(arrayLen);
	for(int i = 0; i < arrayLen; i++)
	{
		WriteFloat(floatArray[i]);
	}
}

void Proto::WriteTag(Proto::Type type, int number)
{
	WriteInt((number << 3) | type );
}

void Proto::InitBuff(char* buff, int size)
{
	mBuff = buff;
	mCurPos = mBuff;
	mSize = size;
}

void Proto::ToHex()
{
	printf( "hex file:\n");
	for(int i = 0; i < mSize; i++)
	{
		char h = (*(mBuff + i) >> 4) & 0x0F;
		char l = *(mBuff + i) & 0x0f;
		printf("%X%X ", h, l);
		if((i != 0) && ((i + 1) % 20 == 0))
			printf("\n");
		else if((i != 0) && ((i + 1) % 4 == 0))
			printf("\t");
	}
	printf("\n");
}




