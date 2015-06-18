#vpath %.c ./src
#VPATH=../src:../inlude

CC=g++
CPPFLAGS=-O0 -Wall -g $(HPATH)

HPATH=-I ../include -I ../libuv/include
SOURCES = $(wildcard *.cpp)
OBJS = $(notdir $(patsubst %.cpp,%.o,$(SOURCES)))
LIBS = -lrt -ldl -lm
LIBPATH = -L .
TARGET = libproto.a

all:$(TARGET)

$(TARGET):$(OBJS)
	ar rc $(TARGET) $(OBJS)

%.o:%.cpp
	$(CC) -c $(CPPFLAGS) $<

.PHONY:clean
clean:
	@rm *.o *.d *.a

.PHONY:move
move:
	@cp $(TARGET) ../build/

include $(notdir $(patsubst %.cpp,%.d,$(SOURCES)))
%.d:%.cpp
#rm -f $@;
	@$(CC) -MM $(CPPFLAGS) $< > $@.$$$$;\
	sed 's,\($*\).o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$;

