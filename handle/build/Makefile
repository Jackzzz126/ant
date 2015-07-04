
mode = debug

CPPFLAGS=-Wall -pthread
ifeq ($(mode), debug)
	CPPFLAGS+=-O0 -g
else
	CPPFLAGS+=-O3 -DNDEBUG
endif
CC=g++

CPPPATH=../src
HPATH=-I ../../include
HPATH+=-I ../include
SOURCES = $(wildcard $(CPPPATH)/*.cpp)
OBJS = $(notdir $(patsubst %.cpp,%.o,$(SOURCES)))
TARGET = libhandle.a
AR = ar

.PHONY:all
all:$(TARGET)

$(TARGET):$(OBJS)
	@$(AR) cqs $(TARGET) $(OBJS)

%.o:$(CPPPATH)/%.cpp
	@$(CC) -c $(CPPFLAGS) $(HPATH) $<

.PHONY:install
install:
	@cp $(TARGET) ../../build/modeules

.PHONY:clean
clean:
	@rm -f *.o $(TARGET) *.d

#------------------------------------------
include $(notdir $(patsubst %.cpp,%.d,$(SOURCES)))
%.d:$(CPPPATH)/%.cpp
#rm -f $@;
	@$(CC) -MM $(CPPFLAGS) $(HPATH) $< > $@.$$$$;\
	sed 's,\($*\).o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$;

