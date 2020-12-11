LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := liblombo_malloc

LOCAL_STRIP_MODULE := false

ifeq ($(BOARD_KERNEL_SRC), linux-4.19)
	LOCAL_SRC_FILES += 4.19/lombo_malloc.c
else
	LOCAL_SRC_FILES += 3.10/lombo_malloc.c
endif

LOCAL_SRC_FILES += lombo_cycle_buf.c

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/inc \
	$(LOCAL_PATH)/../../openmax/oscl/ \
	$(LOCAL_PATH)/../include

LOCAL_LDFLAGS := -lpthread
LOCAL_SHARED_LIBRARIES := liblog

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_SRC_FILES :=           \
	lombo_malloc_test.c

LOCAL_MODULE:= lombo_malloc_test
LOCAL_MODULE_TAGS := optional

# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := false

LOCAL_LDFLAGS += -lpthread

LOCAL_C_INCLUDES += $(LOCAL_PATH)/inc \
	$(LOCAL_PATH)/../../openmax/oscl/ \
		$(LOCAL_PATH)/../include

LOCAL_SHARED_LIBRARIES := liblog liblombo_malloc

include $(BUILD_EXECUTABLE)
