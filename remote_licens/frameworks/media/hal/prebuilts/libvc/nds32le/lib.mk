LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PREBUILT_LIBS := \
	libalc.so \
	libvccore.so \
	vce_h264.so

include $(BUILD_MULTI_PREBUILT)

