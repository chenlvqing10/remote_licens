LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PREBUILT_LIBS := \
	libaacdec.so \
	libapedec.so \
	libflacdec.so \
	libg729dec.so \
	libmp3dec.so \
	libvorbisdec.so \
	libwavdec.so

include $(BUILD_MULTI_PREBUILT)
