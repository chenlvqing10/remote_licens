##############################################################################################
## Description: Entry makefile.
## History
## Version		Date		Author		Description
## V1.0		  2019.03.28    lomboswer		Create File.
##############################################################################################

############################
## System commands #########
############################
export RM=rm

############################
## Compile tools ###########
############################
export CC=arm-linux-gnueabi-gcc
export AR=arm-linux-gnueabi-ar

############################
## Include and Lib paths ###
############################
export LIBS_PATH=$(PWD)/libs

############################
## Compile options #########
############################
export AFLAGS=-cr
export CFLAGS=-g -Wall -O0 -fPIC
export LDFLAGS=-L./libs

############################
## Webserver sources #######
############################
export WEBSERVER_MG=0
export WEBSERVER_GOAHEAD=1
export WEBSERVER_TYPE=$(WEBSERVER_MG)

WEBSERVER_SRC_PATH=webserver/src

APP_INCS=-I./webserver/inc -I./inc
ifeq ($(WEBSERVER_TYPE),$(WEBSERVER_MG))
APP_LD_LIBS=-lwebapi -lws_http -lmg -lpthread -lsqlite3 -lm
APP_INCS+= -I./webserver/src/mongoose
CFLAGS+= -DWS_MG
else
ifeq ($(WEBSERVER_TYPE),$(WEBSERVER_GOAHEAD))
APP_LD_LIBS=-lwebapi -lws_http -lgoahead -lpthread -lsqlite3 -lm
APP_INCS+= -I./webserver/src/goahead
CFLAGS+= -DWS_GOAHEAD
else
APP_LD_LIBS=-lwebapi -lws_http -lmg -lpthread -lsqlite3 -lm
APP_INCS+= -I./webserver/src/mongoose
CFLAGS+= -DWS_MG
endif
endif

TARGET=ws_http

.PHONY: all clean webserver webserver_clean api api_clean

all: webserver api
	$(CC) $(CFLAGS) $(APP_INCS) -o $(TARGET) main.c parse.c $(LDFLAGS) $(APP_LD_LIBS)

webserver:
	make -C $(WEBSERVER_SRC_PATH)

api:
	make -C api

clean: webserver_clean api_clean
	-$(RM) $(TARGET)

webserver_clean:
	make -C $(WEBSERVER_SRC_PATH) clean

api_clean:
	make -C api clean
