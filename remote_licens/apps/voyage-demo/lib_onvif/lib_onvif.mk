LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := lib_onvif_server

#LOCAL_MODULE_TAGS := optional

DEBUG ?= NO

USE_WSSE_AUTH := YES
USE_DIGEST_AUTH := YES

LOCAL_SRC_FILES := \
	nvt/onvif_server.c \
	nvt/onvif_func.c \
	nvt/onvif_proc.c \
	nvt/onvif_subscribe.c \
	nvt/onvif_param.c \
	nvt/onvifface.c \
	nvt/base64.c \
	src/dom.c \
	src/duration.c \
	src/httpda.c \
	src/md5evp.c \
	src/mecevp.c \
	src/smdevp.c \
	src/soapC.c \
	src/soapClient.c \
	src/soapServer.c \
	src/stdsoap2.c \
	src/threads.c \
	src/wsaapi.c \
	src/wsseapi.c

LOCAL_C_INCLUDES := \
	frameworks/network/net_utils

LOCAL_C_INCLUDES += $(shell find $(LOCAL_PATH)/inc -type d)
LOCAL_C_INCLUDES += $(shell find $(LOCAL_PATH)/nvt -type d)
LOCAL_C_INCLUDES += $(shell find $(LOCAL_PATH)/src -type d)
LOCAL_C_INCLUDES += $(shell find $(LOCAL_PATH)/../lib_face-recg/include/api/db/inc -type d)

# 注释此行如果你希望保留符合和调试信息
#LOCAL_STRIP_MODULE := false

# C编译器选项
LOCAL_CFLAGS := -Wall -Werror -fPIC -Wno-unknown-pragmas -Wno-format -O3 -DONVIF_VERSION -DSUPPORT_ONVIF_2_6 -DWITH_OPENSSL
#LOCAL_CFLAGS := -Wall -Werror -fPIC -Wno-format -g -DDEBUG  -DONVIF_VERSION  -DSUPPORT_ONVIF_2_6 -DWITH_OPENSSL

#LOCAL_CFLAGS :=

ifeq (YES, ${USE_WSSE_AUTH})
LOCAL_CFLAGS += -DUSE_WSSE_AUTH
endif

ifeq (YES, ${USE_DIGEST_AUTH})
LOCAL_CFLAGS += -DUSE_DIGEST_AUTH
endif

# C++编译器选项
LOCAL_CPPFLAGS :=

# 链接器选项
# 如果需要引用sysroot中的库，请在此指定，例如引用pthread和math
LOCAL_LDFLAGS := -lpthread -lm -lts -lz -ldl

# 依赖的静态库
#LOCAL_STATIC_LIBRARIES := ./ssl/armhf/libssl ./ssl/armhf/libcrypto
LOCAL_STATIC_LIBRARIES := libssl libcrypto

# 依赖的动态库
LOCAL_SHARED_LIBRARIES := liblog libnet_utils lib_face-recg

include $(BUILD_SHARED_LIBRARY)
