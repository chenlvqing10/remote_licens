LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PREBUILT_LIBS := \
	lib_onvif_server.so

include $(BUILD_MULTI_PREBUILT)
