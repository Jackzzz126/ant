#ifndef _COMM_H_
#define _COMM_H_

//c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>
//sock
#include <sys/socket.h>
#include <sys/epoll.h>
//thread lib
#include<pthread.h>

//stl
#include<string>
#include<vector>
#include<map>
using namespace std;

//owen
#include "strUtil.h"
#include "define.h"

#endif//_COMM_H_
