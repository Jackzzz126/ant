
mode = debug

CPPFLAGS=-Wall -pthread
ifeq ($(mode), debug)
	CPPFLAGS+=-O0 -g
else
	CPPFLAGS+=-O3 -DNDEBUG
endif
CC=g++

CPPPATH=../src
HPATH=-I ../include -I ../libuv/include -I ../libjson
SOURCES = $(wildcard $(CPPPATH)/*.cpp)
OBJS = $(notdir $(patsubst %.cpp,%.o,$(SOURCES)))
LIBS = -luv -lrt -ldl -lm -ljson
LIBPATH = -L .
TARGET = ant
DEPLIBS = libuv.a libjson.a

.PHONY:all
all:$(TARGET)

$(TARGET):$(OBJS) $(DEPLIBS)
	@$(CC) -o $(TARGET) $(CPPFLAGS) $(OBJS) $(LIBS) $(LIBPATH)

%.o:$(CPPPATH)/%.cpp
	@$(CC) -c $(CPPFLAGS) $(HPATH) $<

.PHONY:clean
clean:
	@rm -f *.o $(TARGET) *.d

include $(notdir $(patsubst %.cpp,%.d,$(SOURCES)))
%.d:$(CPPPATH)/%.cpp
#rm -f $@;
	@$(CC) -MM $(CPPFLAGS) $(HPATH) $< > $@.$$$$;\
	sed 's,\($*\).o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$;

