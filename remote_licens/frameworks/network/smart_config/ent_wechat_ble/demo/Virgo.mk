LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := ent_wechat_test
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := \
	ent_wechat_test.c \
	system_private.c
LOCAL_C_INCLUDES := \
	frameworks/network/smart_config/ent_wechat_ble/include \
	frameworks/network/wifi/wifi_station/lib \
	frameworks/network/net_utils
LOCAL_CFLAGS :=
LOCAL_CPPFLAGS :=
LOCAL_LDFLAGS := -lpthread
LOCAL_SHARED_LIBRARIES := libent_wechat libgattimpl liblog libwifi_station libwpa_client libnet_utils 
include $(BUILD_EXECUTABLE)
