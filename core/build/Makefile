CPPPATH=../src
HPATH=-I ../include -I ../../handle/include -I ../../libuv/include -I ../../libjson
SOURCES = $(wildcard $(CPPPATH)/*.cpp)
OBJS = $(notdir $(patsubst %.cpp,%.o,$(SOURCES)))
LIBS = -luv -lrt -ldl -lm -ljson -lhandle
LIBPATH = -L .
TARGET = ant

.PHONY:all
all:$(TARGET)

$(TARGET):$(OBJS) $(LIBS)
	@$(CC) -o $(TARGET) $(CPPFLAGS) $(OBJS) $(LIBS) $(LIBPATH)
	@cp $(TARGET) ../../

%.o:$(CPPPATH)/%.cpp
	@$(CC) -c $(CPPFLAGS) $(HPATH) $<

.PHONY:clean
clean:
	@rm -f *.o $(TARGET) handle.a

include $(notdir $(patsubst %.cpp,%.d,$(SOURCES)))
%.d:$(CPPPATH)/%.cpp
#rm -f $@;
	@$(CC) -MM $(CPPFLAGS) $(HPATH) $< > $@.$$$$;\
	sed 's,\($*\).o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$;

