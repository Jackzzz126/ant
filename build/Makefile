
CPPPATH=../src
HPATH=-I ../libuv/include -I ../libjson
HPATH+=-I ../include
HPATH+=-I ../handle/include
SOURCES = $(wildcard $(CPPPATH)/*.cpp)
OBJS = $(notdir $(patsubst %.cpp,%.o,$(SOURCES)))
LIBS = -luv -lrt -ldl -lm -ljson -lhandle
LIBPATH = -L .
TARGET = ant
DEPLIBS = libuv.a libjson.a libhandle.a

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

