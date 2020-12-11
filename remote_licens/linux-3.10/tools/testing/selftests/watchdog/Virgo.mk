LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := wdog_test

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := \
	wdog_test.c

LOCAL_LDFLAGS := -lpthread -lm -lts -lz

include $(BUILD_EXECUTABLE)
