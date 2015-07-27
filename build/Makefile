
CPPPATH=../src
HPATH=-I ../include
HPATH+=-I ../deps/libjson
HPATH+=-I ../handle/include
SOURCES = $(wildcard $(CPPPATH)/*.cpp)
OBJS = $(notdir $(patsubst %.cpp,%.o,$(SOURCES)))
LIBS = -lrt -ldl -lm -ljson -lhandle -l proto -lprotobuf-lite
LIBPATH = -L .
TARGET = ant
DEPLIBS = libjson.a libhandle.a libproto.a

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

