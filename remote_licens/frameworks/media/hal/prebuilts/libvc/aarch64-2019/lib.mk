LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PREBUILT_LIBS := \
	libalc.so \
	libvccore.so \
	vdec_h264.so \
	vce_h264.so \
	vce_h265.so \
	vce_mjpeg.so

include $(BUILD_MULTI_PREBUILT)

