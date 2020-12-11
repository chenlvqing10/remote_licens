LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := system_pri_test

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := \
	system_private_test.c \
	../src/system_private.c

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../inc

LOCAL_STRIP_MODULE = false

LOCAL_LDFLAGS := -lpthread

LOCAL_SHARED_LIBRARIES := liblog

include $(BUILD_EXECUTABLE)
