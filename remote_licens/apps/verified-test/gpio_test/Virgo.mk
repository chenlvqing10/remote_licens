LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES :=   \
	gpio_test.c

LOCAL_MODULE:= gpio_test
LOCAL_MODULE_TAGS := optional

# ע�ʹ��������ϣ���������Ϻ͵�����Ϣ
LOCAL_STRIP_MODULE := false

include $(BUILD_EXECUTABLE)
