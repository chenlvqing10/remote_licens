LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := \
	isp_video.c \
	isp_api.c

LOCAL_MODULE:= isp_test
LOCAL_MODULE_TAGS := optional

# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := false

include $(BUILD_EXECUTABLE)