LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PREBUILT_LIBS := \
	libtts-16k.so

include $(BUILD_MULTI_PREBUILT)
