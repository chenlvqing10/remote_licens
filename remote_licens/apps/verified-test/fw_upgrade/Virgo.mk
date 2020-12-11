LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE:= fw_upgrade
LOCAL_MODULE_TAGS := optional

SRC_FILES := $(shell find $(LOCAL_PATH)/.)
SRC_FILES := $(SRC_FILES:$(LOCAL_PATH)/./%=$(LOCAL_PATH)/%)
SRC_FILES := $(filter %.cpp %.c,$(SRC_FILES))
SRC_FILES := $(SRC_FILES:$(LOCAL_PATH)/%=%)

LOCAL_SRC_FILES := $(SRC_FILES)

LOCAL_C_INCLUDES := \
	external/libdrm \
	external/libdrm/include/drm \
	external/libdrm/tests

# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := false

LOCAL_CFLAGS := -Wall -Werror

LOCAL_LDFLAGS := -lpthread -lm -lts -lz -ljpeg -lpng -lfreetype

# 依赖的静态库
LOCAL_STATIC_LIBRARIES := libdrm_util libcamera libdisplay

LOCAL_SHARED_LIBRARIES := libdrm liblog liblombo_malloc

include $(BUILD_EXECUTABLE)
