#ifndef _BENCHMARK_H_
#define _BENCHMARK_H_

namespace BenchMark
{
	void Echo(int sock, char* data, int size);
	void DoubleEcho(int sock, char* data, int size);

	void Reg(int sock, char* data, int size);
	void Login(int sock, char* data, int size);
	void Move(int sock, char* data, int size);
}

#endif//_BENCHMARK_H_

