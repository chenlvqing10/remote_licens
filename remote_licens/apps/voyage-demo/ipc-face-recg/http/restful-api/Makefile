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

CFLAGS+= -I./inc -I../json/inc
CFLAGS+= -I../../../../system/core/include
CFLAGS+= -I../../api/db/inc
CFLAGS+= -I../../api/record/inc
CFLAGS+= -I../../api/system/inc
CFLAGS+= -I../../api/network/inc
CFLAGS+= -I../../api/hal/camera/inc
CFLAGS+= -I../../api/hal/light/inc
CFLAGS+= -I../../api/recognition/inc
CFLAGS+= -I../../api/management/person/inc
CFLAGS+= -I../../common/include
CFLAGS+= -I../../../../frameworks/services/face/include
CFLAGS+= -I../../../../frameworks/network/wifi/wifi_station/lib
CFLAGS+= -I../../../../frameworks/media/jpeg/jpeg

.PHONY: all clean

all: $(OBJS)
#	$(AR) $(AFLAGS) $(LIBS_PATH)/librestful-api.a $(OBJS)
#	$(CC) -fPIC -shared -o $(LIBS_PATH)/librestful-api.so $(OBJS)
	$(CC) -shared -o $(LIBS_PATH)/librestful-api.so $(OBJS)

-include $(DEP)
$(OBJS):%.o:%.c
#	$(CC) $(CFLAGS) -MM -MT $@ -MF $(patsubst %.o, %.d, $@) $<
#	$(CC) $(CFLAGS) -c -o $@ $<
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

clean:
	-$(RM) ./src/*.o
	-$(RM) ./src/*.d
	-$(RM) $(LIBS_PATH)/librestful-api.a $(LIBS_PATH)/librestful-api.so

