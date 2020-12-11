LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
$(warning "enter uvc comm makeile")
FILE := $(LOCAL_PATH)/src/uvc_comm.c
src := $(wildcard $(FILE))
$(warning "$(src)")
ifneq ($(wildcard $(FILE)), )
$(warning "enter uvc comm makeile>>1")
LOCAL_MODULE := libuvc_comm

#LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := \
	src/cJSON.c \
	src/uvc_comm.c

LOCAL_C_INCLUDES := \
        $(LOCAL_PATH)/include \
	frameworks/media/audio/ \
       	frameworks/media/hal/include/ \
        frameworks/media/include/ \
        frameworks/media/openmax/api/ \
        frameworks/media/openmax/omx_base/ \
        frameworks/media/openmax/omx_base/inc/ \
        frameworks/media/openmax/oscl/ \
        frameworks/media/recorder/ \
        frameworks/media/video/ \
        frameworks/media/jpeg/jpeg \
        frameworks/media/cut \
        frameworks/media/flip \
        frameworks/services/face/include \
        frameworks/services/face/src

LOCAL_C_INCLUDES += $(shell find apps/voyage-demo/lib_face-recg/include -type d)

# 注释此行如果你希望保留符合和调试信息
#LOCAL_STRIP_MODULE := false

# C编译器选项
LOCAL_CFLAGS := -Wall -Werror
#LOCAL_CFLAGS += -DRTSP_AUDIO

# C++编译器选项
LOCAL_CPPFLAGS :=

# 链接器选项
# 如果需要引用sysroot中的库，请在此指定，例如引用pthread和math
# LOCAL_LDFLAGS := -lpthread -lm
LOCAL_LDFLAGS := -lpthread -lm -lts -lz -ljpeg -lpng -lfreetype -lxml2 -lsqlite3

# 依赖的动态库
LOCAL_SHARED_LIBRARIES :=  liblog libface_svc lib_face-recg

include $(BUILD_SHARED_LIBRARY)
else
LOCAL_PREBUILT_LIBS := \
        libuvc_comm.so

include $(BUILD_MULTI_PREBUILT)

endif
