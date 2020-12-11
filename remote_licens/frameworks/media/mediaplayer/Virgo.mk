LOCAL_PATH := $(call my-dir)
include $(LOCAL_PATH)/../config/config.mk

include $(CLEAR_VARS)
LOCAL_SRC_FILES := \
	omx_mediaplayer.c

LOCAL_MODULE:= libmediaplayer
LOCAL_MODULE_TAGS := optional

# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := $(MEDIA_STRIP_MODULE)

LOCAL_CFLAGS += $(MEDIA_CFLAGS)
LOCAL_LDFLAGS += -lpthread

FILE_INCLUDES = $(shell find $(LOCAL_PATH)/../ -type d)
LOCAL_C_INCLUDES += $(FILE_INCLUDES)
LOCAL_C_INCLUDES += \
	external/libdrm \
	external/libdrm/include/drm

LOCAL_SHARED_LIBRARIES := liblog libdrm libomx liblog
LOCAL_ADDITIONAL_DEPENDENCIES := \
	$(MEDIAPLAYER_ADDITIONAL_DEPENDENCIES)
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	../openmax/component/demuxer_component/demuxer/media_probe.c \
	test/omxplayer_test.c

LOCAL_MODULE:= omxplayer_test
LOCAL_MODULE_TAGS := optional

# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := $(MEDIA_STRIP_MODULE)

LOCAL_CFLAGS += $(MEDIA_CFLAGS)
LOCAL_CFLAGS += -DOMXPLAYER_TEST
LOCAL_LDFLAGS += -lpthread

FILE_INCLUDES = $(shell find $(LOCAL_PATH)/../ -type d)
LOCAL_C_INCLUDES += $(FILE_INCLUDES)
LOCAL_C_INCLUDES += \
	external/libdrm \
	external/libdrm/include/drm

LOCAL_SHARED_LIBRARIES := \
	liblog \
	libdrm \
	libomx \
	liblog \
	libvideo \
	libmediaplayer

include $(BUILD_EXECUTABLE)


include $(CLEAR_VARS)

LOCAL_SRC_FILES := test/streamplayer_test.c

LOCAL_MODULE:= streamplayer_test
LOCAL_MODULE_TAGS := optional

# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := $(MEDIA_STRIP_MODULE)

LOCAL_CFLAGS += $(MEDIA_CFLAGS)
# LOCAL_CFLAGS += -DOMXPLAYER_TEST
LOCAL_LDFLAGS += -lpthread -ldl

FILE_INCLUDES = $(shell find $(LOCAL_PATH)/../ -type d)
LOCAL_C_INCLUDES += $(FILE_INCLUDES)
LOCAL_C_INCLUDES += \
	external/libdrm \
	external/libdrm/include/drm

LOCAL_SHARED_LIBRARIES := \
	liblog \
	libdrm \
	libomx \
	liblog \
	libvideo \
	libmediaplayer

include $(BUILD_EXECUTABLE)

