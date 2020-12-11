LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

MY_FILES_PATH  := \
    $(LOCAL_PATH)/

MY_FILES_SUFFIX := %.c

My_All_Files := $(foreach src_path,$(MY_FILES_PATH), $(shell find $(src_path) -type f))
My_All_Files := $(My_All_Files:$(MY_CPP_PATH)/./%=$(MY_CPP_PATH)%)
MY_SRC_LIST  := $(filter $(MY_FILES_SUFFIX),$(My_All_Files))
MY_SRC_LIST  := $(MY_SRC_LIST:$(LOCAL_PATH)/%=%)
LOCAL_SRC_FILES := $(MY_SRC_LIST)

LOCAL_MODULE:= liblombo_jpeg
LOCAL_MODULE_TAGS := optional

# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := $(MEDIA_STRIP_MODULE)

#LOCAL_CFLAGS += -D__OS_LINUX
LOCAL_LDFLAGS += -lpthread -ldl

FILE_INCLUDES += $(shell find $(LOCAL_PATH)/ -type d)
LOCAL_C_INCLUDES += $(FILE_INCLUDES)
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/jpeg_enc \
	frameworks/media/hal/include \
	frameworks/media/openmax/oscl \
	external/libdrm \
	external/libdrm/include/drm

LOCAL_SHARED_LIBRARIES := libomx liblog libdrm

include $(BUILD_SHARED_LIBRARY)
