LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PREBUILT_LIBS := \
	libcurl.so \
	libcurl.so.4 \
	libcurl.so.4.6.0

include $(BUILD_MULTI_PREBUILT)

