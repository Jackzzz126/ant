
CPPPATH=./
HPATH=-I ./ 
SOURCES = $(wildcard $(CPPPATH)/*.cc)
OBJS = $(notdir $(patsubst %.cc,%.o,$(SOURCES)))
TARGET = libproto.a
AR = ar

.PHONY:all
all:$(TARGET)

$(TARGET):$(OBJS)
	@$(AR) crs $(TARGET) $(OBJS)
	@cp $(TARGET) ../../build

%.o:$(CPPPATH)/%.cc
	@$(CC) -c $(CPPFLAGS) $(HPATH) $<

.PHONY:clean
clean:
	@rm -f *.o $(TARGET) *.d

#------------------------------------------
include $(notdir $(patsubst %.cc,%.d,$(SOURCES)))
%.d:$(CPPPATH)/%.cc
#rm -f $@;
	@$(CC) -MM $(CPPFLAGS) $(HPATH) $< > $@.$$$$;\
	sed 's,\($*\).o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$;

