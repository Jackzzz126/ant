
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
	@$(AR) crs $(TARGET) $(OBJS)
	@cp $(TARGET) ../../build

%.o:$(CPPPATH)/%.cpp
	@$(CC) -c $(CPPFLAGS) $(HPATH) $<

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

