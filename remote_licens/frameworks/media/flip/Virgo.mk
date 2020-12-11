LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := yuv_flip.c

LOCAL_MODULE:= libflip

LOCAL_MODULE_TAGS := optional

# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := false

#LOCAL_LDFLAGS += -lpthread

#LOCAL_C_INCLUDES += $(shell find $(LOCAL_PATH)/ -type d)
LOCAL_C_INCLUDES := \

LOCAL_SHARED_LIBRARIES := liblog

include $(BUILD_SHARED_LIBRARY)

###################################################################################

include $(CLEAR_VARS)

LOCAL_SRC_FILES := yuv_flip_test.c

LOCAL_MODULE:= yuv_flip_test

LOCAL_MODULE_TAGS := optional

# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := $(MEDIA_STRIP_MODULE)

#LOCAL_LDFLAGS += -lpthread

#LOCAL_C_INCLUDES += $(shell find $(LOCAL_PATH)/ -type d)
LOCAL_C_INCLUDES += \
	frameworks/media/cut

LOCAL_SHARED_LIBRARIES := liblog libflip

include $(BUILD_EXECUTABLE)

