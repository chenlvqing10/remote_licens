LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := disp_set_parameter

LOCAL_MODULE_TAGS := eng

LOCAL_SRC_FILES := \
	lcd_tool_drm.c \
	disp_set_parameter.c

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/include/

LOCAL_LDFLAGS := -lpthread -lm

LOCAL_STATIC_LIBRARIES := libdrm_util
LOCAL_SHARED_LIBRARIES := libdrm

include $(BUILD_EXECUTABLE)
