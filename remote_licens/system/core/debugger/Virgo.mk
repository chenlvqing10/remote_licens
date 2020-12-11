LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := libdebugger

LOCAL_MODULE_TAGS := eng

LOCAL_SRC_FILES := \
	debugger.c

LOCAL_C_INCLUDES :=

LOCAL_CFLAGS :=

LOCAL_SHARED_LIBRARIES := liblog libcutils

include $(BUILD_SHARED_LIBRARY)
