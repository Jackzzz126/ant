#vpath %.c ./src
#VPATH=../src:../inlude

mode = debug

ifeq ($(mode), debug)
	CPPFLAGS=-O0 -Wall -g -D debug $(HPATH)
else
	CPPFLAGS=-O3 -Wall $(HPATH)
endif
CC=g++

CPPPATH=../src
HPATH=-I ../include -I ../libuv/include -I ../libjson -I ../proto
SOURCES = $(wildcard $(CPPPATH)/*.cpp)
OBJS = $(notdir $(patsubst %.cpp,%.o,$(SOURCES)))
LIBS = -luv -lrt -ldl -lm -ljson
LIBPATH = -L .
TARGET = ant

all:$(TARGET)

$(TARGET):$(OBJS)
	@$(CC) -o $(TARGET) $(CPPFLAGS) $(OBJS) $(LIBS) $(LIBPATH) -pthread 

%.o:$(CPPPATH)/%.cpp
	@$(CC) -c $(CPPFLAGS) $<

.PHONY:clean
clean:
	@rm *.o *.d

include $(notdir $(patsubst %.cpp,%.d,$(SOURCES)))
%.d:$(CPPPATH)/%.cpp
#rm -f $@;
	@$(CC) -MM $(CPPFLAGS) $< > $@.$$$$;\
	sed 's,\($*\).o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$;

