LOCAL_PATH:= $(call my-dir)
include $(LOCAL_PATH)/../config/config.mk
include $(CLEAR_VARS)

LOCAL_MODULE := libsw_rot

#LOCAL_MODULE_TAGS := option
LOCAL_STRIP_MODULE := $(MEDIA_STRIP_MODULE)

LOCAL_CFLAGS += $(MEDIA_CFLAGS)
LOCAL_CFLAGS += -mfpu=neon

LOCAL_SRC_FILES := \
	rotation_sw.c \
	rotation_neon.c \
	rot_sw.c

LOCAL_C_INCLUDES :=

LOCAL_STRIP_MODULE := false

#LOCAL_CFLAGS += -Wall -Werror
#LOCAL_CFLAGS += -O0 -g3

LOCAL_LDFLAGS := -lpthread
LOCAL_SHARED_LIBRARIES := liblog

ifeq ($(TARGET_CHIP), n7v1)
include $(BUILD_SHARED_LIBRARY)
endif
