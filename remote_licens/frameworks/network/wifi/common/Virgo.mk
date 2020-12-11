LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
ifdef BOARD_WIFI_MODULE
LOCAL_MODULE := S30wifi_module
LOCAL_MODULE_CLASS := ETC
# LOCAL_MODULE_TAGS := optional

ifeq ($(BOARD_WIFI_MODULE), rtl8189ftv)
LOCAL_SRC_FILES := S30wifi_rtl8189ftv
else ifeq ($(BOARD_WIFI_MODULE), rtl8189es)
LOCAL_SRC_FILES := S30wifi_rtl8189es
else ifeq ($(BOARD_WIFI_MODULE), rtl8189fs)
LOCAL_SRC_FILES := S30wifi_rtl8189fs
else ifeq ($(BOARD_WIFI_MODULE), rtl8723ds)
LOCAL_SRC_FILES := S30wifi_rtl8723ds
else ifeq ($(BOARD_WIFI_VENDOR), broadcom)
LOCAL_SRC_FILES := S30wifi_broadcom
endif

LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/init.d
include $(BUILD_PREBUILT)
endif
