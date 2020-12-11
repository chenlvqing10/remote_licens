LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := libcamera

LOCAL_STRIP_MODULE := false

LOCAL_SRC_FILES := \
	v4l2_camera.c

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../include

LOCAL_LDFLAGS := -lpthread
LOCAL_SHARED_LIBRARIES := liblog

include $(BUILD_STATIC_LIBRARY)

