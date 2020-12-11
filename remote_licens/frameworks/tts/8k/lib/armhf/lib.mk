LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PREBUILT_LIBS := \
	libtts-8k.so

include $(BUILD_MULTI_PREBUILT)
