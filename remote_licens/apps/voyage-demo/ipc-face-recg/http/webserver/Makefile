##############################################################################################
## Description: make all the mongoose source files.
## History
## Version		Date		Author		Description
## V1.0		  2019.03.28        		Create File.
##############################################################################################

WEBSERVER_MG := 0
WEBSERVER_GOAHEAD := 1
WEBSERVER_TYPE := 0

CC := arm-linux-gnueabi-gcc
AR := arm-linux-gnueabi-ar
LIBS_PATH := ./lib
AFLAGS := -rsv

SRCS=$(wildcard ./src/*.c)
OBJS=$(SRCS:%.c=%.o)
DEP:=$(SRCS:%.c=%.d)

MG_OBJS=$(wildcard mongoose/*.o)
GOAHEAD_OBJS=$(wildcard goahead/*.o)

ifeq ($(WEBSERVER_TYPE),$(WEBSERVER_MG))
WEBSERVER_DIR=mongoose
CFLAGS+=-DWS_MG
CFLAGS+= -I./inc -I./src/mongoose
WEB_SRCS=$(wildcard ./src/mongoose/*.c)
WEB_OBJS=$(WEB_SRCS:%.c=%.o)
else
ifeq ($(WEBSERVER_TYPE),$(WEBSERVER_GOAHEAD))
WEBSERVER_DIR=goahead
CFLAGS+=-DWS_GOAHEAD
CFLAGS+= -I./inc -I./src/goahead
WEB_SRCS=$(wildcard ./src/goahead/*.c)
WEB_OBJS=$(WEB_SRCS:%.c=%.o)
else
WEBSERVER_DIR=mongoose
CFLAGS+=-DWS_MG
CFLAGS+= -I./inc -I./src/mongoose
WEB_SRCS=$(wildcard ./src/mongoose/*.c)
WEB_OBJS=$(WEB_SRCS:%.c=%.o)
endif
endif

.PHONY: all clean

all: $(WEB_OBJS) $(OBJS)
#	$(AR) $(AFLAGS) $(LIBS_PATH)/libws_http.a $(OBJS)
#	$(CC) -fPIC -shared -o $(LIBS_PATH)/libws_http.so $(OBJS) $(WEB_OBJS)
	$(CC) -fPIC -shared -o $(LIBS_PATH)/libws_http.so $(OBJS) $(WEB_OBJS)

-include $(DEP)
$(OBJS):%.o:%.c
#	$(CC) $(CFLAGS) -MM -MT $@ -MF $(patsubst %.o, %.d, $@) $<
#	$(CC) $(CFLAGS) -c -o $@ $<
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

-include $(DEP)
$(WEB_OBJS):%.o:%.c
#	$(CC) $(CFLAGS) -MM -MT $@ -MF $(patsubst %.o, %.d, $@) $<
#	$(CC) $(CFLAGS) -c -o $@ $<
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

clean:
	-$(RM) ./src/*.o ./src/mongoose/*.o ./src/goahead/*.o
	-$(RM) ./src/*.d ./src/mongoose/*.d ./src/goahead/*.d
	-$(RM) $(LIBS_PATH)/libws_http.so $(LIBS_PATH)/libws_http.a

