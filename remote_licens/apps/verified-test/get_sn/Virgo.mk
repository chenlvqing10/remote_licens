LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES :=   \
	lombo_get_sn.c

LOCAL_MODULE:= lombo_get_sn
LOCAL_MODULE_TAGS := optional

# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := false

include $(BUILD_EXECUTABLE)

