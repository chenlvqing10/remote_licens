LOCAL_PATH := $(call my-dir)
include $(LOCAL_PATH)/../../config/config.mk
MY_C_INCLUDES := $(shell find $(MEDIA_ROOT)/ -type d)

include $(CLEAR_VARS)
#LOCAL_SRC_FILES := lb_recorder_ipc.c lb_recorder_auto.c lb_recorder_test.c lb_recorder_debug.c lb_player_test.c watermark_pic.c
#LOCAL_SRC_FILES += ../../mediaplayer/test/omxplayer_test.c
#LOCAL_SRC_FILES += ../../openmax/component/demuxer_component/demuxer/media_probe.c
LOCAL_SRC_FILES := lb_recorder_ipc.c
LOCAL_MODULE := auto_lb
LOCAL_CFLAGS += -D_TEST_MEDIA_CLIENT
#LOCAL_MODULE_TAGS := eng
# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := $(MEDIA_STRIP_MODULE)
LOCAL_CFLAGS += $(MEDIA_CFLAGS)
LOCAL_LDFLAGS += -lpthread
LOCAL_C_INCLUDES := $(MY_C_INCLUDES)
LOCAL_SHARED_LIBRARIES := liblog libdrm libomx liblog librecorder libvideo libaudio libmediaplayer libmedia_test
include $(BUILD_EXECUTABLE)


include $(CLEAR_VARS)
LOCAL_SRC_FILES := lb_recorder_ipc.c
#LOCAL_SRC_FILES := lb_recorder_ipc.c lb_recorder_auto.c lb_recorder_test.c lb_recorder_debug.c lb_player_test.c watermark_pic.c
#LOCAL_SRC_FILES += ../../mediaplayer/test/omxplayer_test.c
#LOCAL_SRC_FILES += ../../openmax/component/demuxer_component/demuxer/media_probe.c
LOCAL_MODULE:= async_player
LOCAL_CFLAGS += -D_TEST_MEDIA_CLIENT
#LOCAL_MODULE_TAGS := eng
# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := $(MEDIA_STRIP_MODULE)
LOCAL_CFLAGS += $(MEDIA_CFLAGS)
LOCAL_LDFLAGS += -lpthread
LOCAL_C_INCLUDES := $(MY_C_INCLUDES)
LOCAL_SHARED_LIBRARIES := liblog libdrm libomx liblog librecorder libvideo libaudio libmediaplayer libmedia_test
include $(BUILD_EXECUTABLE)


include $(CLEAR_VARS)
#LOCAL_SRC_FILES := lb_recorder_ipc.c lb_recorder_auto.c lb_recorder_test.c lb_recorder_debug.c lb_player_test.c watermark_pic.c
#LOCAL_SRC_FILES += ../../mediaplayer/test/omxplayer_test.c
#LOCAL_SRC_FILES += ../../openmax/component/demuxer_component/demuxer/media_probe.c
LOCAL_SRC_FILES := lb_recorder_ipc.c
LOCAL_MODULE:= recorder_show_components
LOCAL_CFLAGS += -D_TEST_MEDIA_CLIENT
#LOCAL_MODULE_TAGS := eng
# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := $(MEDIA_STRIP_MODULE)
LOCAL_LDFLAGS += -lpthread
LOCAL_CFLAGS += $(MEDIA_CFLAGS)
LOCAL_C_INCLUDES := $(MY_C_INCLUDES)
LOCAL_SHARED_LIBRARIES := liblog libdrm libomx liblog librecorder libvideo libaudio libmediaplayer libmedia_test
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
#LOCAL_SRC_FILES := lb_recorder_ipc.c lb_recorder_auto.c lb_recorder_test.c lb_recorder_debug.c lb_player_test.c watermark_pic.c
#LOCAL_SRC_FILES += ../../mediaplayer/test/omxplayer_test.c
#LOCAL_SRC_FILES += ../../openmax/component/demuxer_component/demuxer/media_probe.c
LOCAL_SRC_FILES := lb_recorder_ipc.c
LOCAL_MODULE:= lbrecorder
LOCAL_CFLAGS += -D_TEST_MEDIA_CLIENT
#LOCAL_MODULE_TAGS := eng
# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := $(MEDIA_STRIP_MODULE)
#LOCAL_CFLAGS += -D__OS_LINUX
LOCAL_CFLAGS += $(MEDIA_CFLAGS)
LOCAL_LDFLAGS += -lpthread
LOCAL_C_INCLUDES := $(MY_C_INCLUDES)
LOCAL_SHARED_LIBRARIES := liblog libdrm libomx liblog librecorder libvideo libaudio libmediaplayer libmedia_test
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
#LOCAL_SRC_FILES := lb_recorder_ipc.c lb_recorder_auto.c lb_recorder_test.c lb_recorder_debug.c lb_player_test.c watermark_pic.c
LOCAL_SRC_FILES := lb_recorder_ipc.c
#LOCAL_SRC_FILES += ../../mediaplayer/test/omxplayer_test.c
#LOCAL_SRC_FILES += ../../openmax/component/demuxer_component/demuxer/media_probe.c
LOCAL_MODULE:= media_server
LOCAL_CFLAGS += $(MEDIA_CFLAGS)
#LOCAL_MODULE_TAGS := eng
# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := $(MEDIA_STRIP_MODULE)
LOCAL_CFLAGS += -D_TEST_MEDIA_SERVER
LOCAL_CFLAGS += $(MEDIA_CFLAGS)
LOCAL_LDFLAGS += -lpthread
LOCAL_C_INCLUDES := $(MY_C_INCLUDES)
LOCAL_SHARED_LIBRARIES := liblog libdrm libomx liblog librecorder libvideo libaudio libmediaplayer libmedia_test
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := stream_cb_test.c
LOCAL_MODULE:= stream_cb_test
LOCAL_MODULE_TAGS := optional
# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := $(MEDIA_STRIP_MODULE)
LOCAL_CFLAGS += $(MEDIA_CFLAGS)
LOCAL_LDFLAGS += -lpthread
LOCAL_C_INCLUDES += $(MY_C_INCLUDES)
LOCAL_SHARED_LIBRARIES := liblog libdrm libomx liblog librecorder libvideo libaudio
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := stream_cb_test.c
LOCAL_MODULE:= stream_cb_mjpeg
LOCAL_MODULE_TAGS := optional
# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := $(MEDIA_STRIP_MODULE)
LOCAL_CFLAGS += -DSTREAM_CB_MJPEG
LOCAL_CFLAGS += $(MEDIA_CFLAGS)
LOCAL_LDFLAGS += -lpthread
LOCAL_C_INCLUDES += $(MY_C_INCLUDES)
LOCAL_SHARED_LIBRARIES := liblog libdrm libomx liblog librecorder libvideo libaudio
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_SRC_FILES += ./omxrec_test.c
LOCAL_SRC_FILES += lb_recorder_auto.c lb_recorder_test.c lb_recorder_debug.c lb_player_test.c watermark_pic.c
LOCAL_SRC_FILES += ../../mediaplayer/test/omxplayer_test.c
LOCAL_SRC_FILES += ../../openmax/component/demuxer_component/demuxer/media_probe.c
LOCAL_MODULE := libmedia_test
#LOCAL_MODULE_TAGS := eng
# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := $(MEDIA_STRIP_MODULE)
LOCAL_CFLAGS += $(MEDIA_CFLAGS)
LOCAL_LDFLAGS += -lpthread
LOCAL_C_INCLUDES := $(MY_C_INCLUDES)
LOCAL_SHARED_LIBRARIES := libdrm libomx liblog librecorder libvideo libaudio libmediaplayer
include $(BUILD_SHARED_LIBRARY)
