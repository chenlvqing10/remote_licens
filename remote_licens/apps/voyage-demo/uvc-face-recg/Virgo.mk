LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := uvc-face-recg

LOCAL_SRC_FILES := \
	uvc-face-recg.c \
	common/src/app_lang.c

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/common/inc \
	frameworks/media/audio/ \
	frameworks/media/hal/include/ \
	frameworks/media/include/ \
	frameworks/media/openmax/api/ \
	frameworks/media/openmax/omx_base/ \
	frameworks/media/openmax/omx_base/inc/ \
	frameworks/media/openmax/oscl/ \
	frameworks/media/recorder/ \
	frameworks/media/video/ \
	frameworks/media/jpeg/jpeg \
	frameworks/media/cut \
	frameworks/media/flip \
	frameworks/services/face/include \
	frameworks/services/face/src

LOCAL_C_INCLUDES += $(shell find apps/voyage-demo/uvc_comm/include -type d)
LOCAL_C_INCLUDES += $(shell find apps/voyage-demo/lib_face-recg/include -type d)

LOCAL_LDFLAGS := -lpthread -lm -lxml2 -lsqlite3

LOCAL_SHARED_LIBRARIES += \
	liblog \
	libdrm \
	libomx \
	librecorder \
	libvideo \
	libaudio \
	libcut \
	liblombo_malloc \
	libface_svc \
	libiniparser \
	libdtbased_tk \
	libnn_engine \
	libopenvx \
	libopenvx-nn \
	libnn-priv \
	libnn \
	libsoft-nn \
	liblombo_jpeg \
	libxlsreader \
	libxlsxwriter \
	libflip \
	libuvc_comm \
	lib_face-recg \
	libnet_utils \
	libwifi_station \
	libwpa_client \
	libethernet \
	libwifi_softap \
	libzbar

include $(BUILD_EXECUTABLE)
