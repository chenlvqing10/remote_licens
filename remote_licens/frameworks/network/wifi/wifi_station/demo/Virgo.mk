LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := sta_connect
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := \
	sta_connect.c
LOCAL_C_INCLUDES := \
	frameworks/network/wifi/wifi_station/lib \
	frameworks/network/net_utils
LOCAL_CFLAGS :=
LOCAL_CPPFLAGS :=
LOCAL_LDFLAGS :=
LOCAL_SHARED_LIBRARIES := libwifi_station libwpa_client libnet_utils liblog
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := sta_connect_with_bssid
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := \
	sta_connect_with_bssid.c
LOCAL_C_INCLUDES := \
	frameworks/network/wifi/wifi_station/lib \
	frameworks/network/net_utils
LOCAL_CFLAGS :=
LOCAL_CPPFLAGS :=
LOCAL_LDFLAGS :=
LOCAL_SHARED_LIBRARIES := libwifi_station libwpa_client libnet_utils liblog
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := sta_scan
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := \
	sta_scan.c
LOCAL_C_INCLUDES := \
	frameworks/network/wifi/wifi_station/lib
LOCAL_CFLAGS :=
LOCAL_CPPFLAGS :=
LOCAL_LDFLAGS :=
LOCAL_SHARED_LIBRARIES := libwifi_station libwpa_client liblog
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := sta_disconnect
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := \
	sta_disconnect.c
LOCAL_C_INCLUDES := \
	frameworks/network/wifi/wifi_station/lib
LOCAL_CFLAGS :=
LOCAL_CPPFLAGS :=
LOCAL_LDFLAGS :=
LOCAL_SHARED_LIBRARIES := libwifi_station libwpa_client liblog
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := sta_connect_with_id
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := \
	sta_connect_with_id.c
LOCAL_C_INCLUDES := \
	frameworks/network/wifi/wifi_station/lib \
	frameworks/network/net_utils
LOCAL_CFLAGS :=
LOCAL_CPPFLAGS :=
LOCAL_LDFLAGS :=
LOCAL_SHARED_LIBRARIES := libwifi_station libwpa_client libnet_utils liblog
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := sta_connect_stress_test
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := \
	sta_connect_stress_test.c
LOCAL_C_INCLUDES := \
	frameworks/network/wifi/wifi_station/lib \
	frameworks/network/net_utils
LOCAL_CFLAGS :=
LOCAL_CPPFLAGS :=
LOCAL_LDFLAGS :=
LOCAL_SHARED_LIBRARIES := libwifi_station libwpa_client liblog libnet_utils
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := sta_scan_stress_test
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := \
	sta_scan_stress_test.c
LOCAL_C_INCLUDES := \
	frameworks/network/wifi/wifi_station/lib \
LOCAL_CFLAGS :=
LOCAL_CPPFLAGS :=
LOCAL_LDFLAGS :=
LOCAL_SHARED_LIBRARIES := libwifi_station libwpa_client liblog
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := sta_switch_stress_test
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := \
	sta_switch_stress_test.c
LOCAL_C_INCLUDES := \
	frameworks/network/wifi/wifi_station/lib \
LOCAL_CFLAGS :=
LOCAL_CPPFLAGS :=
LOCAL_LDFLAGS :=
LOCAL_SHARED_LIBRARIES := libwifi_station libwpa_client liblog
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := sta_get_net_id
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := \
        sta_get_net_id.c
LOCAL_C_INCLUDES := \
        frameworks/network/wifi/wifi_station/lib \
LOCAL_CFLAGS :=
LOCAL_CPPFLAGS :=
LOCAL_LDFLAGS :=
LOCAL_SHARED_LIBRARIES := libwifi_station libwpa_client liblog
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := sta_disable_net_conf
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := \
        sta_disable_net_conf.c
LOCAL_C_INCLUDES := \
        frameworks/network/wifi/wifi_station/lib \
LOCAL_CFLAGS :=
LOCAL_CPPFLAGS :=
LOCAL_LDFLAGS :=
LOCAL_SHARED_LIBRARIES := libwifi_station libwpa_client liblog
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := sta_delete_net_conf
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := \
        sta_delete_net_conf.c
LOCAL_C_INCLUDES := \
        frameworks/network/wifi/wifi_station/lib \
LOCAL_CFLAGS :=
LOCAL_CPPFLAGS :=
LOCAL_LDFLAGS :=
LOCAL_SHARED_LIBRARIES := libwifi_station libwpa_client liblog
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := sta_delete_all_net_conf
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := \
        sta_delete_all_net_conf.c
LOCAL_C_INCLUDES := \
        frameworks/network/wifi/wifi_station/lib \
LOCAL_CFLAGS :=
LOCAL_CPPFLAGS :=
LOCAL_LDFLAGS :=
LOCAL_SHARED_LIBRARIES := libwifi_station libwpa_client liblog
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := sta_get_saved_config
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := \
        sta_get_saved_config.c
LOCAL_C_INCLUDES := \
        frameworks/network/wifi/wifi_station/lib \
LOCAL_CFLAGS :=
LOCAL_CPPFLAGS :=
LOCAL_LDFLAGS :=
LOCAL_SHARED_LIBRARIES := libwifi_station libwpa_client liblog
include $(BUILD_EXECUTABLE)
