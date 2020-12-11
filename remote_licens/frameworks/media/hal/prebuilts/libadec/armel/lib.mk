LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PREBUILT_LIBS := \
	libaacdec.a \
	libg729dec.a \
	libmp3dec.a \
	libwavdec.a

include $(BUILD_MULTI_PREBUILT)
