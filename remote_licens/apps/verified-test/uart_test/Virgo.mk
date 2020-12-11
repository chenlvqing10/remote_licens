LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES :=   \
	uart_test.c

LOCAL_MODULE:= uart_test
LOCAL_MODULE_TAGS := optional

# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := false

LOCAL_LDFLAGS += -lpthread

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_MODULE := uart_temp_test

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := \
	uart_temp/src/uart_temp.c \
	uart_temp/main.c

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/uart_temp/inc

LOCAL_STRIP_MODULE = false

LOCAL_LDFLAGS := -lpthread

LOCAL_SHARED_LIBRARIES := liblog

include $(BUILD_EXECUTABLE)

