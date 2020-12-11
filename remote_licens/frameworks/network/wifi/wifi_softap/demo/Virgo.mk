LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := softap_on
#LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := \
	softap_on.c
LOCAL_C_INCLUDES := \
	frameworks/network/wifi/wifi_softap/lib
LOCAL_CFLAGS :=
LOCAL_CPPFLAGS :=
LOCAL_LDFLAGS :=
LOCAL_SHARED_LIBRARIES := libwifi_softap liblog
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := softap_on_stress_test
#LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := \
	softap_on_stress_test.c
LOCAL_C_INCLUDES := \
	frameworks/network/wifi/wifi_softap/lib
LOCAL_CFLAGS :=
LOCAL_CPPFLAGS :=
LOCAL_LDFLAGS :=
LOCAL_SHARED_LIBRARIES := libwifi_softap liblog
include $(BUILD_EXECUTABLE)
