
CPPPATH=../src
HPATH=-I ../include
HPATH+=-I ../deps/libjson
HPATH+=-I ../deps/hiredis
HPATH+=-I ../deps/lua-5.3.1/src
HPATH+=-I ../handle/include
HPATH+=-I ../proto/build
SOURCES = $(wildcard $(CPPPATH)/*.cpp)
OBJS = $(notdir $(patsubst %.cpp,%.o,$(SOURCES)))
LIBS = -lrt -ldl -lm -ljson -lhandle -lproto -lprotobuf-lite -lhiredis -lcrypto -llua
LIBPATH = -L .
TARGET = ant
DEPLIBS = libjson.a libhandle.a libproto.a libhiredis.a

.PHONY:all
all:$(TARGET)

$(TARGET):$(OBJS) $(DEPLIBS)
	@$(CC) -o $(TARGET) $(CPPFLAGS) $(OBJS) $(LIBS) $(LIBPATH)
	@cp $(TARGET) ../

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

