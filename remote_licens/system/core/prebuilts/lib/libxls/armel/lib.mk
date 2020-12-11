LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PREBUILT_LIBS := \
	libxlsreader.so  libxlsreader.so.1  libxlsreader.so.1.5.1

include $(BUILD_MULTI_PREBUILT)
