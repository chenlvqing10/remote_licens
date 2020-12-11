LOCAL_PATH := $(call my-dir)
include $(LOCAL_PATH)/../config/config.mk

include $(CLEAR_VARS)

MY_FILES_PATH  := \
    $(LOCAL_PATH)/omx_base  \
    $(LOCAL_PATH)/oscl \
	$(LOCAL_PATH)/api

MY_FILES_SUFFIX := %.c

My_All_Files := $(foreach src_path,$(MY_FILES_PATH), $(shell find $(src_path) -type f \( ! -iname "*_eos_*" \)))
My_All_Files := $(My_All_Files:$(MY_CPP_PATH)/./%=$(MY_CPP_PATH)%)
MY_SRC_LIST  := $(filter $(MY_FILES_SUFFIX),$(My_All_Files))
MY_SRC_LIST  := $(MY_SRC_LIST:$(LOCAL_PATH)/%=%)
LOCAL_SRC_FILES := $(MY_SRC_LIST)

LOCAL_MODULE:= libomx
##LOCAL_MODULE_TAGS := optional

# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := $(MEDIA_STRIP_MODULE)

#LOCAL_CFLAGS += -D__OS_LINUX
LOCAL_CFLAGS += $(MEDIA_CFLAGS)
LOCAL_LDFLAGS += -lpthread -ltinyalsa -lasound -ldl
LOCAL_LDFLAGS += $(MEDIA_LDFLAGS)

LOCAL_C_INCLUDES := \
	external/libdrm \
	external/libdrm/include/drm \
	$(LOCAL_PATH)/../rot_neon \
	$(LOCAL_PATH)/../hal/include \
	$(LOCAL_PATH)/../include \
	$(LOCAL_PATH)/omx_base/oscl
FILE_INCLUDES = $(shell find $(LOCAL_PATH)/ -type d)
LOCAL_C_INCLUDES += $(FILE_INCLUDES)

LOCAL_WHOLE_STATIC_LIBRARIES = \
	libcamera \
	libdisplay

LOCAL_SHARED_LIBRARIES := \
	liblog \
	libdrm

LOCAL_ADDITIONAL_DEPENDENCIES := $(OMX_ADDITIONAL_DEPENDENCIES)

include $(BUILD_SHARED_LIBRARY)

#####################################################################################

include $(CLEAR_VARS)
LOCAL_SRC_FILES :=           \
    test/base_filter_component.c \
    test/file_reader.c           \
    test/file_writer.c           \
    test/omx_test_tunnel.c       \
    test/omx_test_untunnel.c     \
    test/vrec_vrender_test.c     \
    test/test_omx_comp.c         \
    test/rotate_test.c

LOCAL_MODULE:= omxtest
##LOCAL_MODULE_TAGS := optional
##LOCAL_MODULE_TAGS := eng

# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := $(MEDIA_STRIP_MODULE)

LOCAL_CFLAGS += $(MEDIA_CFLAGS)
LOCAL_LDFLAGS += -lpthread

LOCAL_C_INCLUDES := $(shell find $(LOCAL_PATH)/ -type d)
LOCAL_C_INCLUDES += \
	external/libdrm \
	external/libdrm/include/drm \
	$(LOCAL_PATH)/../hal/include \
	$(LOCAL_PATH)/../include

LOCAL_SHARED_LIBRARIES := \
	liblog \
	libdrm \
	libomx

include $(BUILD_EXECUTABLE)
#####################################################################################

include $(CLEAR_VARS)
LOCAL_SRC_FILES :=           \
    test/sdec_srender_test.c

LOCAL_MODULE:= sdec_srender_test
##LOCAL_MODULE_TAGS := optional

# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := $(MEDIA_STRIP_MODULE)

LOCAL_LDFLAGS += -lpthread

LOCAL_C_INCLUDES := $(shell find $(LOCAL_PATH)/ -type d)
LOCAL_C_INCLUDES += \
	external/libdrm \
	external/libdrm/include/drm \
	$(LOCAL_PATH)/../hal/include

LOCAL_SHARED_LIBRARIES := \
	liblog \
	libdrm \
	libomx \
	libomx_sdec	\
	libomx_srender

#####################################################################################

include $(CLEAR_VARS)
LOCAL_SRC_FILES :=           \
    test/arender_test.c

LOCAL_MODULE:= arender_test
##LOCAL_MODULE_TAGS := optional

# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := $(MEDIA_STRIP_MODULE)

LOCAL_LDFLAGS += -lpthread

LOCAL_C_INCLUDES := $(shell find $(LOCAL_PATH)/ -type d)
LOCAL_C_INCLUDES += \
	external/libdrm \
	external/libdrm/include/drm \
	$(LOCAL_PATH)/../hal/include

LOCAL_SHARED_LIBRARIES := \
	liblog \
	libdrm \
	libomx \


#####################################################################################

include $(CLEAR_VARS)
LOCAL_SRC_FILES :=           \
    test/file_writer.c           \
    test/arecorder_test.c

LOCAL_MODULE:= arec_test
##LOCAL_MODULE_TAGS := optional

# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := $(MEDIA_STRIP_MODULE)

LOCAL_LDFLAGS += -lpthread

LOCAL_C_INCLUDES := $(shell find $(LOCAL_PATH)/ -type d)
LOCAL_C_INCLUDES += \
	external/libdrm \
	external/libdrm/include/drm \
	$(LOCAL_PATH)/../hal/include

LOCAL_SHARED_LIBRARIES := \
	liblog \
	libdrm \
	libomx \

include $(BUILD_EXECUTABLE)


#####################################################################################

include $(CLEAR_VARS)
LOCAL_SRC_FILES :=           \
    test/video_recorder.c

LOCAL_MODULE:= video_recorder_test
##LOCAL_MODULE_TAGS := optional

# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := $(MEDIA_STRIP_MODULE)

LOCAL_CFLAGS += $(MEDIA_CFLAGS)
LOCAL_LDFLAGS += -lpthread

LOCAL_C_INCLUDES := $(shell find $(LOCAL_PATH)/ -type d)
LOCAL_C_INCLUDES += \
	external/libdrm \
	external/libdrm/include/drm \
	$(LOCAL_PATH)/../hal/include

LOCAL_SHARED_LIBRARIES := \
	liblog \
	libdrm \
	libomx \
	liblombo_malloc \
	libalc

include $(BUILD_EXECUTABLE)

#####################################################################################

include $(CLEAR_VARS)
LOCAL_SRC_FILES :=           \
    test/omx_dbg_cmdline.c

LOCAL_MODULE:= omx_dbg
##LOCAL_MODULE_TAGS := optional

# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := $(MEDIA_STRIP_MODULE)

LOCAL_CFLAGS += $(MEDIA_CFLAGS)
LOCAL_LDFLAGS += -lpthread

LOCAL_C_INCLUDES := $(shell find $(LOCAL_PATH)/ -type d)
LOCAL_C_INCLUDES += \
    $(LOCAL_PATH)/oscl \
    $(LOCAL_PATH)/../include \
    $(LOCAL_PATH)/../hal/include

LOCAL_SHARED_LIBRARIES := \
        liblog \
	libdrm \
        libomx

include $(BUILD_EXECUTABLE)


include $(call all-makefiles-under, $(LOCAL_PATH))
