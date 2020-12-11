LOCAL_PATH :=$(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := watchdog_test

LOCAL_MODULE_TAGS := optional

# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := false

LOCAL_SRC_FILES := \
	watchdog_test.c \
	../hal/watchdog.c \

LOCAL_CFLAGS := -Wall

LOCAL_LDFLAGS := -lpthread -lm

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../../api/include \

LOCAL_SHARED_LIBRARIES := liblog

include $(BUILD_EXECUTABLE)