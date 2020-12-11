LOCAL_PATH:= $(call my-dir)

# executable - link_handler_test
include $(CLEAR_VARS)

LOCAL_MODULE := link_handler_test

LOCAL_SRC_FILES := link_handler_test.c

LOCAL_C_INCLUDES := frameworks/network/ethernet/ frameworks/network/net_utils/

LOCAL_LDFLAGS := -lpthread -lm

LOCAL_SHARED_LIBRARIES := liblog libnet_utils libethernet

include $(BUILD_EXECUTABLE)