LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := \
	isp_video.c \
	isp_api.c

LOCAL_MODULE:= isp_test
LOCAL_MODULE_TAGS := optional

# ע�ʹ��������ϣ���������Ϻ͵�����Ϣ
LOCAL_STRIP_MODULE := false

include $(BUILD_EXECUTABLE)