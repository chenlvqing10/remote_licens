LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := jpeg_enc_test_nv12.c

LOCAL_MODULE:= jpeg_test_nv12

LOCAL_MODULE_TAGS := optional

# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := false

LOCAL_LDFLAGS += -lpthread

#LOCAL_C_INCLUDES += $(shell find $(LOCAL_PATH)/ -type d)
LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../jpeg \
	frameworks/media/hal/include \

LOCAL_SHARED_LIBRARIES :=  libomx liblombo_jpeg liblog libdrm

include $(BUILD_EXECUTABLE)

###################################################################################

include $(CLEAR_VARS)
LOCAL_SRC_FILES := jpeg_enc_test.c

LOCAL_MODULE:= jpeg_test
LOCAL_MODULE_TAGS := optional

# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := $(MEDIA_STRIP_MODULE)

#LOCAL_LDFLAGS += -lpthread

LOCAL_C_INCLUDES += $(shell find $(LOCAL_PATH)/ -type d)
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../jpeg \
	frameworks/media/hal/include \
	frameworks/media/openmax/oscl \

LOCAL_SHARED_LIBRARIES :=  libomx liblombo_jpeg liblog libdrm

include $(BUILD_EXECUTABLE)

###################################################################################

include $(CLEAR_VARS)
LOCAL_SRC_FILES := jpeg_dec_test.c

LOCAL_MODULE:= jpeg_dec_test
LOCAL_MODULE_TAGS := optional

# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := $(MEDIA_STRIP_MODULE)

#LOCAL_LDFLAGS += -lpthread

LOCAL_C_INCLUDES += $(shell find $(LOCAL_PATH)/ -type d)
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../jpeg \
	frameworks/media/hal/include \

LOCAL_SHARED_LIBRARIES :=  libomx liblombo_jpeg liblog libdrm

include $(BUILD_EXECUTABLE)

###################################################################################

include $(CLEAR_VARS)
LOCAL_SRC_FILES := jpeg_dec_scale_test.c

LOCAL_MODULE:= jpeg_dec_scale_test
LOCAL_MODULE_TAGS := optional

# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := $(MEDIA_STRIP_MODULE)

#LOCAL_LDFLAGS += -lpthread

LOCAL_C_INCLUDES += $(shell find $(LOCAL_PATH)/ -type d)
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../jpeg \
	frameworks/media/hal/include \
	external/libdrm \
	frameworks/media/openmax/oscl \
	external/libdrm/include/drm

LOCAL_SHARED_LIBRARIES := liblombo_jpeg liblog libdrm libomx

include $(BUILD_EXECUTABLE)
