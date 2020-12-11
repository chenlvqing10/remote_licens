LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PREBUILT_LIBS := \
	libascdec.so \
	libassdec.so \
	libjssdec.so \
	liblrcdec.so \
	libovrdec.so \
	libsmidec.so \
	libsrtdec.so \
	libstldec.so \
	libsubdec.so \
	libttsdec.so \
	libtxtdec.so \
	libvsfdec.so \
	libpgsdec.so

include $(BUILD_MULTI_PREBUILT)
