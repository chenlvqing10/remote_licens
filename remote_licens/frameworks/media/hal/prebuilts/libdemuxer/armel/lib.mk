LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PREBUILT_LIBS := \
	libg729demuxer.so \
	libmovdemuxer.so \
	libmp3demuxer.so \
	libwavdemuxer.so

include $(BUILD_MULTI_PREBUILT)
