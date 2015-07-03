
CPPPATH=../src
HPATH=-I ../include -I ../../core/include
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

