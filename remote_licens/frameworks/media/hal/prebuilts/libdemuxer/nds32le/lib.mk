LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PREBUILT_LIBS := \
	libaacdemuxer.so \
	libapedemuxer.so \
	libflacdemuxer.so \
	libg729demuxer.so \
	libmovdemuxer.so \
	libmp3demuxer.so \
	liboggdemuxer.so \
	libwavdemuxer.so

include $(BUILD_MULTI_PREBUILT)
