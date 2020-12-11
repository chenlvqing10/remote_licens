LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := librawstreamdemuxer

LOCAL_MODULE_TAGS := optional

#LOCAL_STRIP_MODULE := false

LOCAL_SRC_FILES := \
	rawstream_demuxer.c

LOCAL_C_INCLUDES := \
	frameworks/media/openmax/oscl \
	frameworks/media/hal/include \
	frameworks/media/openmax/omx_base/inc

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../include

LOCAL_SHARED_LIBRARIES += liblog liblombo_malloc

include $(BUILD_SHARED_LIBRARY)

