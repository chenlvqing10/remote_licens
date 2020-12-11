LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PREBUILT_LIBS :=   \
		libent_wechat.so \
		libgattimpl.so

include $(BUILD_MULTI_PREBUILT)
