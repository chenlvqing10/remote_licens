LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	lombo_system_sound.c \
	audio_gain.c

LOCAL_MODULE:= libsys_sound
#LOCAL_MODULE_TAGS := optional

# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := $(MEDIA_STRIP_MODULE)

LOCAL_CFLAGS += $(MEDIA_CFLAGS)
LOCAL_LDFLAGS += -lasound

LOCAL_C_INCLUDES := \
    frameworks/media/hal/include

LOCAL_SHARED_LIBRARIES := liblog

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_SRC_FILES :=           \
    system_sound_test.c

LOCAL_MODULE:= system_sound_test
LOCAL_MODULE_TAGS := optional

# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := $(MEDIA_STRIP_MODULE)

LOCAL_LDFLAGS += -lasound

LOCAL_C_INCLUDES := $(shell find $(LOCAL_PATH)/ -type d)
LOCAL_C_INCLUDES += \
	frameworks/media/include \
	frameworks/media/hal/include \

LOCAL_SHARED_LIBRARIES := \
	liblog \
	libsys_sound

include $(BUILD_EXECUTABLE)