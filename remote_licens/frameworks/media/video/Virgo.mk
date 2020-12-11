LOCAL_PATH := $(call my-dir)
include $(LOCAL_PATH)/../config/config.mk
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	video_stream.c vsrc_camera.c vsrc_manager.c

LOCAL_MODULE:= libvideo
#LOCAL_MODULE_TAGS := optional

# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := $(MEDIA_STRIP_MODULE)

LOCAL_CFLAGS += $(MEDIA_CFLAGS)
LOCAL_LDFLAGS += -lpthread

FILE_INCLUDES = $(shell find $(LOCAL_PATH)/ -type d)
LOCAL_C_INCLUDES += $(FILE_INCLUDES)
LOCAL_C_INCLUDES := \
	external/libdrm \
	external/libdrm/include/drm \
	$(LOCAL_PATH)/../hal/include \
	$(LOCAL_PATH)/../include \
	frameworks/media/openmax/oscl \
	frameworks/media/hal/include \
	frameworks/media/openmax/omx_base \
	frameworks/media/openmax/omx_base/inc \
	frameworks/media/openmax/api \
	frameworks/media/openmax/component/vrender_component/ \
	frameworks/media/openmax/component/vrec_component/

LOCAL_SHARED_LIBRARIES := liblog libdrm libomx liblog
LOCAL_ADDITIONAL_DEPENDENCIES := $(VIDEO_ADDITIONAL_DEPENDENCIES)
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	test/test_video_stream.c

LOCAL_MODULE:= test_video_stream
LOCAL_MODULE_TAGS := optional

# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := $(MEDIA_STRIP_MODULE)

#LOCAL_CFLAGS += -D__OS_LINUX
LOCAL_LDFLAGS += -lpthread

FILE_INCLUDES = $(shell find $(LOCAL_PATH)/ -type d)
LOCAL_C_INCLUDES += $(FILE_INCLUDES)
LOCAL_C_INCLUDES := \
	frameworks/media/openmax/api \
	frameworks/media/include

LOCAL_SHARED_LIBRARIES := \
	liblog \
	libdrm \
	libomx \
	liblog \
	libvideo

include $(BUILD_EXECUTABLE)

