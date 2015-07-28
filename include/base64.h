#ifndef _BASE64_H_
#define _BASE64_H_

class Base64
{
	static char* Decode(char* inStr, size_t& resultSize, bool trimTrailZeros = true);
	static char* Encode(const char* data, size_t len);
};

#endif//_BASE64_H_

