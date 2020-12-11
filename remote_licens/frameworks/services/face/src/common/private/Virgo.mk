LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := private_test

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := \
	private.c \
	private_test.c \

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/. \
	frameworks/services/face/include \
	frameworks/services/face/src/common/include \

LOCAL_STRIP_MODULE = false

LOCAL_LDFLAGS := -lpthread

LOCAL_SHARED_LIBRARIES := liblog

include $(BUILD_EXECUTABLE)
