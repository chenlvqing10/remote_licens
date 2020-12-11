LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES :=   \
	uart_virtualtp.c

LOCAL_MODULE:= uart_virtualtp
LOCAL_MODULE_TAGS := optional

# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := false

LOCAL_LDFLAGS += -lpthread

include $(BUILD_EXECUTABLE)


