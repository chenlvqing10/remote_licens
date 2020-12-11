# Copyright 2006 The Android Open Source Project

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= logcat.cpp event.logtags

LOCAL_CFLAGS := -DHAVE_SYS_UIO_H -DHAVE_IOCTL -DHAVE_PTHREADS

LOCAL_SHARED_LIBRARIES := liblog

LOCAL_MODULE:= logcat

LOCAL_MODULE_TAGS := eng

include $(BUILD_EXECUTABLE)
