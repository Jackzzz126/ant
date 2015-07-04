
mode = debug

CPPFLAGS=-Wall -pthread
ifeq ($(mode), debug)
	CPPFLAGS+=-O0 -g
else
	CPPFLAGS+=-O3 -DNDEBUG
endif
CC=g++

CPPPATH=..
HPATH=-I ../../include
HPATH+=-I ../../include/handle
SOURCES = $(wildcard $(CPPPATH)/*.cpp)
OBJS = $(notdir $(patsubst %.cpp,%.o,$(SOURCES)))
TARGET = libhandle.a
AR = ar

.PHONY:all
all:$(TARGET)

$(TARGET):$(OBJS)
	@$(AR) cqs $(TARGET) $(OBJS)
	@cp $(TARGET) ../../core/build/

%.o:$(CPPPATH)/%.cpp
	@$(CC) -c $(CPPFLAGS) $(HPATH) $<

.PHONY:clean
clean:
	@rm -f *.o $(TARGET)

include $(notdir $(patsubst %.cpp,%.d,$(SOURCES)))
%.d:$(CPPPATH)/%.cpp
#rm -f $@;
	@$(CC) -MM $(CPPFLAGS) $(HPATH) $< > $@.$$$$;\
	sed 's,\($*\).o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$;

