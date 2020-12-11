LOCAL_PATH :=$(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := sound_svc_test

LOCAL_MODULE_TAGS := optional

# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := false

LOCAL_SRC_FILES := \
	sound_svc_test.cpp \
	../src/sound_svc.c \
	../../base/file/file_ops.c \
	../../base/queue/queue.c \

LOCAL_CFLAGS := -Wall

LOCAL_LDFLAGS := -lpthread -lm

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../../services/include \
	$(LOCAL_PATH)/../../base/include \
	frameworks/tts/include \
	frameworks/media/hal/include \

LOCAL_SHARED_LIBRARIES := libtts_encrype-16k liblog libsys_sound libiniparser

include $(BUILD_EXECUTABLE)

##################################################################


