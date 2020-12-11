LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := uvc-gadget

LOCAL_SRC_FILES := uvc-gadget.c

LOCAL_C_INCLUDES += \
	frameworks/media/audio/ \
	frameworks/media/hal/include/ \
	frameworks/media/include/ \
	frameworks/media/openmax/api/ \
	frameworks/media/openmax/omx_base/ \
	frameworks/media/openmax/omx_base/inc/ \
	frameworks/media/openmax/oscl/ \
	frameworks/media/recorder/ \
	frameworks/media/video/

LOCAL_LDFLAGS := -lpthread -lm

LOCAL_SHARED_LIBRARIES += \
	liblog \
	libdrm \
	libomx \
	librecorder \
	libvideo \
	libaudio

include $(BUILD_EXECUTABLE)
