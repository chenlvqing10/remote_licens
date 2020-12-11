LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := lcd_tool

LOCAL_MODULE_TAGS := eng

LOCAL_SRC_FILES := \
	cJSON.c \
	disp_cjson.c \
	lcd_tool_drm.c \
	lcd_tool.c

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/include/

LOCAL_LDFLAGS := -lpthread -lm

LOCAL_STATIC_LIBRARIES := libdrm_util
LOCAL_SHARED_LIBRARIES := libdrm

include $(BUILD_EXECUTABLE)
