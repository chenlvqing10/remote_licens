##############################################################################################
## Description: make all the mongoose source files.
## History
## Version		Date		Author		Description
## V1.0		  2019.03.28    lomboswer		Create File.
##############################################################################################

WEBSERVER_MG := 0
WEBSERVER_GOAHEAD := 1
WEBSERVER_TYPE := 0

CC := arm-linux-gnueabi-gcc
AR := arm-linux-gnueabi-ar
LIBS_PATH := .
AFLAGS := -rsv

SRCS=$(wildcard *.c)
OBJS=$(SRCS:%.c=%.o)
DEP:=$(SRCS:%.c=%.d)
MG_OBJS=$(wildcard mongoose/*.o)
GOAHEAD_OBJS=$(wildcard goahead/*.o)

ifeq ($(WEBSERVER_TYPE),$(WEBSERVER_MG))
WEBSERVER_DIR=mongoose
CFLAGS+=-DWS_MG
CFLAGS+= -I../inc -Imongoose
else
ifeq ($(WEBSERVER_TYPE),$(WEBSERVER_GOAHEAD))
WEBSERVER_DIR=goahead
CFLAGS+=-DWS_GOAHEAD
CFLAGS+= -I../inc -Igoahead
else
WEBSERVER_DIR=mongoose
CFLAGS+=-DWS_MG
CFLAGS+= -I../inc -Imongoose
endif
endif

.PHONY: all clean mongoose mongoose_clean goahead goahead_clean

all: $(WEBSERVER_DIR) $(OBJS)
	$(AR) $(AFLAGS) $(LIBS_PATH)/libws_http.a $(OBJS)
#	$(CC) -fPIC -shared -o $(LIBS_PATH)/libwebserver.so $(OBJS) $(MG_OBJS)

-include $(DEP)
$(OBJS):%.o:%.c
	$(CC) $(CFLAGS) -MM -MT $@ -MF $(patsubst %.o, %.d, $@) $<
	$(CC) $(CFLAGS) -c -o $@ $<

mongoose:
	make -C $@

mongoose_clean:
	make -C mongoose clean

goahead:
	make -C goahead

goahead_clean:
	make -C goahead clean

clean: $(WEBSERVER_DIR)_clean
	-$(RM) *.o
	-$(RM) *.d
	-$(RM) $(LIBS_PATH)/libws_http.a
