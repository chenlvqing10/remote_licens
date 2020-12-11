LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PREBUILT_LIBS := \
	libg729enc.so \
	libaacenc.so \
	libpcmenc.so \
	libwavenc.so

include $(BUILD_MULTI_PREBUILT)
