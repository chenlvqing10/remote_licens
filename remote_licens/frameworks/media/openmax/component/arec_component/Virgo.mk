LOCAL_PATH := $(call my-dir)
include $(LOCAL_PATH)/../../../config/config.mk

include $(CLEAR_VARS)

MY_FILES_SUFFIX := %.c

My_All_Files := $(foreach src_path,$(LOCAL_PATH), $(shell find $(src_path) -type f \( ! -iname "*_eos_*" \)))
MY_SRC_LIST  := $(filter $(MY_FILES_SUFFIX),$(My_All_Files))
MY_SRC_LIST  := $(MY_SRC_LIST:$(LOCAL_PATH)/%=%)
LOCAL_SRC_FILES := $(MY_SRC_LIST)

LOCAL_MODULE:= libomx_arec
#LOCAL_MODULE_TAGS := eng

# 注释此行如果你希望保留符合和调试信息
#LOCAL_STRIP_MODULE := false

LOCAL_CFLAGS += $(MEDIA_CFLAGS)
LOCAL_LDFLAGS += -lpthread -ltinyalsa -lasound -ldl

FILE_INCLUDES = $(shell find $(LOCAL_PATH)/../../.. -type d)
LOCAL_C_INCLUDES += $(FILE_INCLUDES)

LOCAL_SHARED_LIBRARIES := liblog  libomx
LOCAL_STATIC_LIBRARIES += libecho

include $(BUILD_SHARED_LIBRARY)

#include $(CLEAR_VARS)
#LOCAL_MODULE:=libecho
#LOCAL_MODULE_TAGS := optional
#LOCAL_MODULE_CLASS := STATIC_LIBRARIES
#LOCAL_PREBUILT_LIBS  :=libecho.a
#include $(BUILD_MULTI_PREBUILT)

