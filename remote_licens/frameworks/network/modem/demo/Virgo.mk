LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := modem_cmd_test
#LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := \
	modem-cmd-test.c
LOCAL_C_INCLUDES := \
	frameworks/network/modem/lib
LOCAL_CFLAGS :=
LOCAL_CPPFLAGS :=
LOCAL_LDFLAGS :=
LOCAL_SHARED_LIBRARIES := libmodem_at liblog
include $(BUILD_EXECUTABLE)

