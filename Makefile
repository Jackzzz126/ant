
mode = debug

CPPFLAGS=-Wall -pthread
ifeq ($(mode), debug)
	CPPFLAGS+=-O0 -g
else
	CPPFLAGS+=-O3 -DNDEBUG
endif
CC=g++

#use in child make
export CPPFLAGS
export CC

.PHONY:all
all:
	@cd core/build && make;

.PHONY:clean
clean:
	@cd core/build && make clean;
