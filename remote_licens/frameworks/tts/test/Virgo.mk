LOCAL_PATH :=$(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := tts_test

LOCAL_MODULE_TAGS := optional

# 注释此行如果你希望保留符合和调试信息
#LOCAL_STRIP_MODULE := false

LOCAL_SRC_FILES := \
	tts_test.c

LOCAL_CFLAGS := -Wall -g3

LOCAL_LDFLAGS := -lpthread -lm

LOCAL_C_INCLUDES := \
	frameworks/tts/include \
	frameworks/media/hal/include \

LOCAL_SHARED_LIBRARIES := libtts-16k liblog libsys_sound

include $(BUILD_EXECUTABLE)
