LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= debuggerd.c unwind-arm.c pr-support.c utility.c
LOCAL_CFLAGS := -Wall
LOCAL_MODULE := debuggerd

#LOCAL_STATIC_LIBRARIES := libcutils libc
LOCAL_SHARED_LIBRARIES := liblog libcutils

include $(BUILD_EXECUTABLE)
