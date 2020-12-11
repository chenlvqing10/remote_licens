CC = /home/luojunhua/virgo_restful/toolchain/gcc-linaro-5.5.0-2017.10-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc

#DEBUG ?= YES
DEBUG ?= NO

USE_WSSE_AUTH := YES
USE_DIGEST_AUTH := YES

DEBUG_CFLAGS     := -fPIC -Wall -Wno-format -g -DDEBUG  -DONVIF_VERSION  -DSUPPORT_ONVIF_2_6 -DWITH_OPENSSL
RELEASE_CFLAGS   := -fPIC -Wall -Wno-unknown-pragmas -Wno-format -O3 -DONVIF_VERSION -DSUPPORT_ONVIF_2_6 -DWITH_OPENSSL -DONVIF_PORT

ifeq (YES, ${USE_WSSE_AUTH})
	DEBUG_CFLAGS += -DUSE_WSSE_AUTH
	RELEASE_CFLAGS += -DUSE_WSSE_AUTH
endif

ifeq (YES, ${USE_DIGEST_AUTH})
	DEBUG_CFLAGS += -DUSE_DIGEST_AUTH
	RELEASE_CFLAGS += -DUSE_DIGEST_AUTH
endif

LIBS = ./ssl/armhf/libssl.a ./ssl/armhf/libcrypto.a

DEBUG_LDFLAGS    := -g 
RELEASE_LDFLAGS  := 

ifeq (YES, ${DEBUG})
   CFLAGS       := ${DEBUG_CFLAGS}
   CXXFLAGS     := ${DEBUG_CFLAGS}
   LDFLAGS      := ${DEBUG_LDFLAGS}
else
   CFLAGS       := ${RELEASE_CFLAGS}
   CXXFLAGS     := ${RELEASE_CFLAGS}
   LDFLAGS      := ${RELEASE_LDFLAGS}
endif

LDFLAGS      += -lpthread -lm -ldl

CFLAGS   += -I./inc -I./src -I./ssl/include
CXXFLAGS += -I./inc -I./src -I./ssl/include

EXEC := onvif_svr

all: ${EXEC}

${EXEC}:
	$(CC) -o ${EXEC} \
		./nvt/onvif_server.c \
		./nvt/onvif_func.c \
		./nvt/onvif_proc.c \
		./nvt/onvif_subscribe.c \
		./nvt/onvif_param.c \
		./nvt/onvifface.c \
		./nvt/base64.c \
		./src/dom.c \
		./src/duration.c \
		./src/httpda.c \
		./src/md5evp.c \
		./src/mecevp.c \
		./src/smdevp.c \
		./src/soapC.c \
		./src/soapClient.c \
		./src/soapServer.c \
		./src/stdsoap2.c \
		./src/threads.c \
		./src/wsaapi.c \
		./src/wsseapi.c \
		$(CFLAGS) $(LIBS) $(LDFLAGS)

clean:
	rm -f ${EXEC}
