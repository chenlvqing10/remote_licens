LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES :=   \
	video_capture.c \
	formation.c \

	ifeq ($(ver),dis)
		LOCAL_SRC_FILES :=   \
			video_capture.c \
			formation.c \
			drm_ops.c
	endif

LOCAL_C_INCLUDES +=   \
		frameworks/media/hal/include \

	ifeq ($(ver),dis)
		LOCAL_C_INCLUDES +=   \
			frameworks/media/hal/include \
			external/libdrm              \
			external/libdrm/include/drm \
			external/libdrm/tests/util
	endif

LOCAL_MODULE:= video_capture
	ifeq ($(ver),dis)
		LOCAL_MODULE:= video_capture_dis
	endif

LOCAL_MODULE_TAGS := optional

LOCAL_STRIP_MODULE := false

LOCAL_SHARED_LIBRARIES := liblog liblombo_malloc
LOCAL_STATIC_LIBRARIES := libcamera

	ifeq ($(ver),dis)
		LOCAL_SHARED_LIBRARIES := libdrm libomx liblog liblombo_malloc
		LOCAL_STATIC_LIBRARIES := libdrm_util libcamera
		LOCAL_CFLAGS := -DDISPLAY
	endif
include $(BUILD_EXECUTABLE)
