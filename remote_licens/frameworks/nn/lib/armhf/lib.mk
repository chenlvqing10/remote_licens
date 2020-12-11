LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PREBUILT_LIBS := \
	libdtbased_tk.so \
	libopenvx.so \
	libopenvx-nn.so \
	libnn.so \
	libnn-priv.so \
	libnn_engine.so \
	libsoft-nn.so \
	libgomp.so.1

include $(BUILD_MULTI_PREBUILT)
