##############################################################################################
## Description: make all the mongoose source files.
## History
## Version		Date		Author		Description
## V1.0		  2019.03.28    lomboswer		Create File.
##############################################################################################

CC := arm-linux-gnueabi-gcc
AR := arm-linux-gnueabi-ar
LIBS_PATH := .
AFLAGS := -rsv

SRCS=$(wildcard *.c)
OBJS=$(SRCS:%.c=%.o)
DEP:=$(SRCS:%.c=%.d)

.PHONY: all clean

all: $(OBJS)
	$(AR) $(AFLAGS) $(LIBS_PATH)/libmg.a $(OBJS)

-include $(DEP)
$(OBJS):%.o:%.c
	$(CC) $(CFLAGS) -MM -MT $@ -MF $(patsubst %.o, %.d, $@) $<
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	-$(RM) *.o
	-$(RM) *.d
	-$(RM) $(LIBS_PATH)/libmg.a
