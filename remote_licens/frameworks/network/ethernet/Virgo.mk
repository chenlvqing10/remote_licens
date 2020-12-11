LOCAL_PATH:= $(call my-dir)

# shared library - libethernet.so
include $(CLEAR_VARS)

LOCAL_MODULE := libethernet

LOCAL_SRC_FILES := link_handler.c

LOCAL_C_INCLUDES := frameworks/network/net_utils/

LOCAL_LDFLAGS := -lpthread -lm

LOCAL_SHARED_LIBRARIES := liblog libnet_utils

include $(BUILD_SHARED_LIBRARY)