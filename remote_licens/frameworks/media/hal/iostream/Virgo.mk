LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := libiostream

LOCAL_MODULE_TAGS := optional

#LOCAL_STRIP_MODULE := false

LOCAL_SRC_FILES := \
	iostream_plugin.c \
	iostream_file.c

LOCAL_SRC_FILES += \
	iostream_external.c \
	ring_buffer_mgr.c

LOCAL_C_INCLUDES := \
	frameworks/media/openmax/oscl \
	frameworks/media/hal/include \
	frameworks/media/openmax/omx_base/inc

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../include

LOCAL_SHARED_LIBRARIES += liblog

include $(BUILD_SHARED_LIBRARY)

