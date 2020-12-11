LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES :=   \
	video_preview.c \
	buffer.c \
	media.c \
	isp_api.c

LOCAL_MODULE:= video_preview
LOCAL_MODULE_TAGS := optional

# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := false

LOCAL_LDFLAGS += -lpthread

LOCAL_C_INCLUDES +=   \
		frameworks/media/hal/include \
		external/libdrm \
		external/libdrm/include/drm \
		external/libdrm/tests/util

# 依赖的静态库
LOCAL_STATIC_LIBRARIES := libdrm_util

LOCAL_SHARED_LIBRARIES := libdrm libomx liblog liblombo_malloc

include $(BUILD_EXECUTABLE)
