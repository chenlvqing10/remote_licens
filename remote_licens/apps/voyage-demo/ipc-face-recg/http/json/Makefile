##############################################################################################
## Description: make the json source files.
## History
## Version		Date		Author		Description
## V1.0		  2019.08.26        		Create File.
##############################################################################################

CC := arm-linux-gnueabi-gcc
AR := arm-linux-gnueabi-ar
LIBS_PATH := ./lib
AFLAGS := -rsv

SRCS=$(wildcard ./src/*.c)
OBJS=$(SRCS:%.c=%.o)
DEP:=$(SRCS:%.c=%.d)

CFLAGS+= -I./inc

.PHONY: all clean

all: $(OBJS)
#	$(AR) $(AFLAGS) $(LIBS_PATH)/libjson.a $(OBJS)
#	$(CC) -fPIC -shared -o $(LIBS_PATH)/libjson.so $(OBJS)
	$(CC) -shared -o $(LIBS_PATH)/libjson.so $(OBJS)

-include $(DEP)
$(OBJS):%.o:%.c
#	$(CC) $(CFLAGS) -MM -MT $@ -MF $(patsubst %.o, %.d, $@) $<
#	$(CC) $(CFLAGS) -c -o $@ $<
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

clean:
	-$(RM) ./src/*.o
	-$(RM) ./src/*.d
	-$(RM) $(LIBS_PATH)/libjson.a $(LIBS_PATH)/libjson.so

