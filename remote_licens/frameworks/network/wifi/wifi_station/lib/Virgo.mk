LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libwifi_station
LOCAL_MODULE_TAGS := optional
ifeq ($(BOARD_WIFI_VENDOR), realtek)
LOCAL_CFLAGS += -DREALTEK_MODULE
endif
LOCAL_SRC_FILES := \
	wifi_common.c \
	net_conf_parser.c \
	wifi_monitor.c \
	wifi_station.c \
	state.c
LOCAL_C_INCLUDES :=
#LOCAL_STRIP_MODULE := false
LOCAL_CFLAGS +=
LOCAL_CPPFLAGS :=
LOCAL_LDFLAGS := -lpthread
LOCAL_STATIC_LIBRARIES :=
LOCAL_SHARED_LIBRARIES := libwpa_client liblog
include $(BUILD_SHARED_LIBRARY)
