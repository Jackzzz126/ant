#ifndef _BENCHMARK_H_
#define _BENCHMARK_H_

namespace BenchMark
{
	void Echo(int sock, int msgId, char* data, int size);
	void DoubleEcho(int sock, int msgId, char* data, int size);

	void Reg(int sock, int msgId, char* data, int size);
	void RegDb(int sock, int msgId, char* data, int size);
	void Login(int sock, int msgId, char* data, int size);
	void LoginDb(int sock, int msgId, char* data, int size);
	void Move(int sock, int msgId, char* data, int size);

	void UdpMove(int sock, int msgId, char* data, int size);
}

#endif//_BENCHMARK_H_

