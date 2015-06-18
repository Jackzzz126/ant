# usage
# run "make init" first
TESTS=\
	client \
	bench-mark

mode = debug

ifeq ($(mode), debug)
	CPPFLAGS=-O0 -Wall -g -D debug $(HPATH)
else
	CPPFLAGS=-O3 -Wall $(HPATH)
endif

CC=g++

CPPPATH=../../src
HPATH=-I ../../include -I ../../libuv/include -I ../../libjson -I ../../proto
OBJS = $(wildcard *.o)
LIBPATH = -L . -L ../../build
LIBS = -luv -lrt -ldl -lm -ljson -lproto
	
all: 
	@for dir in $(TESTS); \
		do cd $$dir; \
			$(CC) -c $(CPPFLAGS) main.cpp; \
			$(CC) -o $$dir $(CPPFLAGS) $(OBJS) $(LIBS) $(LIBPATH) -pthread; \
			cd ..; \
		done

.PHONY: clean
clean:
	@rm -f *.o; rm -f *.a;for dir in $(TESTS); do cd $$dir; rm -f $$dir; rm -f *.o; rm -f *.a; cd ..; done

.PHONY: init
init:
	@cp ../build/*.o ./; \
	for dir in $(TESTS); \
		do cd $$dir; \
			cp ../../build/*.o ./; \
			rm main.o; \
			cd ..; \
		done


