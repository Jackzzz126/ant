#ifndef _MD5_H_
#define _MD5_H_

/* Md5 declaration. */
class Md5 
{
public:
	Md5();
	Md5(const byte *input, size_t length);

	void Reset();
	void Update(const byte *input, size_t length);

	const byte* Digest();
	string ToHex();
private:
	void final();
	void transform(const byte block[64]);
	void encode(const uint *input, byte *output, size_t length);
	void decode(const byte *input, uint *output, size_t length);
	string bytesToHexString(const byte *input, size_t length);

	/* class uncopyable */
	Md5(const Md5&);
	Md5& operator=(const Md5&);
private:
	uint _state[4];	/* state (ABCD) */
	uint _count[2];	/* number of bits, modulo 2^64 (low-order word first) */
	byte _buffer[64];	/* input buffer */
	byte _digest[16];	/* message digest */
	bool _finished;		/* calculate finished ? */

	static const byte PADDING[64];	/* padding for calculate */
	static const char HEX[16];
	static const size_t BUFFER_SIZE = 1024;
};

#endif//_MD5_H_

