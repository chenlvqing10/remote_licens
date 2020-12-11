LOCAL_PATH := $(call my-dir)
include $(LOCAL_PATH)/../config/config.mk

MY_C_INCLUDES := $(shell find $(MEDIA_ROOT)/ -type d)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := lb_recorder.c omx_video_recorder.c recorder_take_picture.c recorder_pano.c watermark/watermark.c
LOCAL_MODULE:= librecorder
#LOCAL_MODULE_TAGS := eng
# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := $(MEDIA_STRIP_MODULE)
#LOCAL_CFLAGS += -D__OS_LINUX
LOCAL_CFLAGS += $(MEDIA_CFLAGS)
LOCAL_LDFLAGS += -lpthread -ldl
LOCAL_C_INCLUDES := $(MY_C_INCLUDES)
LOCAL_SHARED_LIBRARIES := liblog libdrm libomx liblog libvideo libaudio
LOCAL_ADDITIONAL_DEPENDENCIES := \
	$(RECORDER_ADDITIONAL_DEPENDENCIES)
include $(BUILD_SHARED_LIBRARY)

include $(call all-makefiles-under, $(LOCAL_PATH))
