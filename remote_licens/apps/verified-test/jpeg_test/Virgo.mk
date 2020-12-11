LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := jpeg_enc_test_nv12.c

LOCAL_MODULE:= jpeg_enc_test_nv12

LOCAL_MODULE_TAGS := optional

# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := false

LOCAL_LDFLAGS += -lpthread

#LOCAL_C_INCLUDES += $(shell find $(LOCAL_PATH)/ -type d)
LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../../../frameworks/media/jpeg/jpeg \
	$(LOCAL_PATH)/../../../frameworks/media/hal/include \

LOCAL_SHARED_LIBRARIES :=  libomx liblombo_jpeg liblog libdrm

include $(BUILD_EXECUTABLE)

###################################################################################

include $(CLEAR_VARS)
LOCAL_SRC_FILES := jpeg_enc_test.c

LOCAL_MODULE:= jpeg_enc_test
LOCAL_MODULE_TAGS := optional

# 注释此行如果你希望保留符合和调试信息
#LOCAL_STRIP_MODULE := false

#LOCAL_LDFLAGS += -lpthread

LOCAL_C_INCLUDES += $(shell find $(LOCAL_PATH)/ -type d)
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../../../frameworks/media/jpeg/jpeg \
	$(LOCAL_PATH)/../../../frameworks/media/hal/include \

LOCAL_SHARED_LIBRARIES :=  libomx liblombo_jpeg liblog libdrm liblombo_malloc

include $(BUILD_EXECUTABLE)
