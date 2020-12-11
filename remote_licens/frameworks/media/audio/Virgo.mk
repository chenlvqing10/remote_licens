LOCAL_PATH := $(call my-dir)
include $(LOCAL_PATH)/../config/config.mk
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	audio_stream.c

LOCAL_MODULE:= libaudio
#LOCAL_MODULE_TAGS := optional

# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := $(MEDIA_STRIP_MODULE)

LOCAL_CFLAGS += $(MEDIA_CFLAGS)
LOCAL_LDFLAGS += -lpthread

LOCAL_C_INCLUDES := \
	frameworks/media/include \
	frameworks/media/openmax/oscl \
	frameworks/media/openmax/omx_base \
	frameworks/media/openmax/omx_base/inc \
	frameworks/media/openmax/api \
	frameworks/media/hal/include \

LOCAL_SHARED_LIBRARIES := liblog libomx
LOCAL_ADDITIONAL_DEPENDENCIES := $(AUDIO_ADDITIONAL_DEPENDENCIES)

include $(BUILD_SHARED_LIBRARY)
