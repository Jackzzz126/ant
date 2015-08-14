
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
	@cd proto && ./proto.sh;
	@cd proto/build && make;
	@cd handle/build && make;
	@cd build && make;

.PHONY:clean
clean:
	@cd proto/build && make clean;
	@cd handle/build && make clean;
	@cd build && make clean;
