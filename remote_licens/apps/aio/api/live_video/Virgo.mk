LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := rtsp_test


LOCAL_SRC_FILES:= \
	src/frame_queue.c \
	src/live_video.c \
	src/live_video_ctrl.c \
	src/live_server.cpp \
	src/mem_pool.c \
	src/osa.c \
	src/H264VideoServerMediaSubsession.cpp \
	src/H264VideoSource.cpp \
	test/main.c

FILE_INCLUDES += $(shell find external/live555/ -type d)
FILE_INCLUDES += $(shell find frameworks/media/ -type d)
FILE_INCLUDES += $(shell find $(LOCAL_PATH) -type d)


LOCAL_C_INCLUDES += $(FILE_INCLUDES)
LOCAL_C_INCLUDES += \
	src \
	include \
	./ \

# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := false


LOCAL_STATIC_LIBRARIES := libgroupsock libliveMedia \
	libBasicUsageEnvironment libUsageEnvironment 
LOCAL_SHARED_LIBRARIES := liblombo_malloc libface_svc libiniparser  \
		libdrm libomx liblombo_jpeg libvideo \
		libdtbased_tk libnn_engine libopenvx libopenvx-nn libnn-priv libnn liblog libsoft-nn libcut  \
		libflip libzbar librecorder libaudio \
		libdrm libomx liblog libvideo  \



LOCAL_CFLAGS := -Wall -D_LIVE_BUFFER_LOG_SUPPORT_ -g -O0

$(foreach _ld, $(LOCAL_STATIC_LIBRARIES), \
        $(eval LD_PATH += -L$(TARGET_OUT)/../obj/STATIC_LIBRARIES/$(_ld)_intermediates) )


LOCAL_LDFLAGS := $(LD_PATH) -lpthread -lgroupsock -lliveMedia \
	-lBasicUsageEnvironment -lUsageEnvironment 
	
## find out/target/n7v1/p1s/obj/ -name lib*.a | xargs -I file cp file out/target/n7v1/p1s/obj/lib

include $(BUILD_EXECUTABLE)
