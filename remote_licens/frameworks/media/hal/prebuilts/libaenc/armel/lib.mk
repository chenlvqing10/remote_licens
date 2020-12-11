LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PREBUILT_LIBS := \
	libaacenc.a \
	libwavenc.a

include $(BUILD_MULTI_PREBUILT)
