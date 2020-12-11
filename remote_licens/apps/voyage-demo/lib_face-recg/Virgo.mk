LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := lib_face-recg

#LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := \
	common/file/file_ops.c \
	common/db/db_persistence.c \
	common/db/sqlite_middle.c \
	common/queue/ee_queue.c \
	common/excel/excel_file_read.c \
	common/excel/excel_file_write.c \
	common/excel/excel_person_public.c \
	common/time/time_conv.c \
	common/string/char_conv.c \
	common/string/char_ops.c \
	common/string/app_check_input.c \
	common/storage/hotplug.c \
	common/storage/storage.c \
	common/image/bmp_dec.c \
	common/image/png_dec.c \
	common/elang/elang.c \
	common/threadpool/threadpool.c \
	api/db/src/cloud_platform.c \
	api/db/src/device_factory_default.c \
	api/db/src/device_hw_param.c \
	api/db/src/device_net_param.c \
	api/db/src/device_status_info.c \
	api/db/src/device_system_param.c \
	api/db/src/external_import_pic.c \
	api/db/src/face_recognition_param.c \
	api/db/src/manager_info.c \
	api/db/src/person_info.c \
	api/db/src/punch_record.c \
	api/db/src/db.c \
	api/db/src/db_config.c \
	api/network/src/net_wifi.c \
	api/network/src/net_wired.c \
	api/hal/display/src/display.c \
	api/hal/audio/src/lombo_sound_play.c \
	api/hal/audio/src/lombo_sound_cap.c \
	api/hal/audio/src/lombo_sound_mixer.c \
	api/hal/audio/src/mixer.c \
	api/hal/audio/src/pcm.c \
	api/hal/device/src/cpu_info.c \
	api/hal/device/src/mem_info.c \
	api/hal/gpio/src/gpio.c \
	api/hal/light/src/light.c \
	api/face/src/face.c \
	api/recognition/src/recognition.c \
	api/management/person/src/person.c \
	api/management/person/src/recg_img.c \
	api/management/import/src/person_import.c \
	api/management/lookup/src/person_lookup.c \
	api/management/reg_check/src/person_reg_check.c \
	api/management/register/src/person_register.c \
	api/record/src/camera_record.c \
	api/record/src/record_export.c \
	api/record/src/record_lookup.c \
	api/record/src/record_auto_delete.c \
	api/rtsp/common/src/live_buffer.c \
	api/rtsp/video/src/live_video.c \
	api/rtsp/live/src/live_server.cpp \
	api/rtsp/live/src/ADTSAudioServerMediaSubsession.cpp \
	api/rtsp/live/src/ADTSAudioSource.cpp \
	api/rtsp/live/src/H264VideoServerMediaSubsession.cpp \
	api/rtsp/live/src/H264VideoSource.cpp \
	api/system/src/system_multi_lang.c \
	api/system/src/system_factory.c \
	api/system/src/system_screen.c \
	api/system/src/system_time.c \
	api/system/src/system_admin.c \
	api/system/src/system_develop_sel.c \
	api/system/src/system_info.c \
	api/system/src/system_volume.c \
	api/system/src/system_upgrade.c \
	api/system/src/system_private.c \
	api/config/src/config.c \
	api/microwave/src/microwave.c \
	api/watermark/common/src/watermark_pic.c \
	api/watermark/common/src/yuv_retangle_draw.c \
	api/watermark/common/src/yuv_blending.c \
	api/watermark/watermark_api.c \

LOCAL_C_INCLUDES := \
	frameworks/network/wifi/wifi_softap/lib \
	frameworks/network/wifi/wifi_station/lib \
	frameworks/network/net_utils/ \
	frameworks/network/ethernet/ \
	frameworks/services/face/include \
	frameworks/services/face/src \
	frameworks/media/jpeg/jpeg \
	frameworks/media/hal/include \
	frameworks/media/cut \
	frameworks/media/flip \
	frameworks/media/recorder \
	frameworks/media/include \
	frameworks/media/video \
	frameworks/media/audio \
	frameworks/media/openmax/oscl \
	frameworks/media/openmax/omx_base/inc \
	external/live555/liveMedia/include \
	external/live555/groupsock/include \
	external/live555/UsageEnvironment/include \
	external/live555/BasicUsageEnvironment/include \
	external/live555/mediaServer \

LOCAL_C_INCLUDES += $(shell find $(LOCAL_PATH)/include -type d)

# 注释此行如果你希望保留符合和调试信息
#LOCAL_STRIP_MODULE := false

# C编译器选项
LOCAL_CFLAGS := -Wall -Werror
#LOCAL_CFLAGS += -DRTSP_AUDIO

# C++编译器选项
LOCAL_CPPFLAGS :=

# 链接器选项
# 如果需要引用sysroot中的库，请在此指定，例如引用pthread和math
# LOCAL_LDFLAGS := -lpthread -lm
LOCAL_LDFLAGS := -lpthread -lm -lts -lz -ljpeg -lpng -lfreetype -lxml2 -lsqlite3

# 依赖的静态库
LOCAL_STATIC_LIBRARIES := \
	libliveMedia \
	libBasicUsageEnvironment \
	libUsageEnvironment \
	libgroupsock

# 依赖的动态库
LOCAL_SHARED_LIBRARIES :=  liblombo_malloc libface_svc libiniparser \
				libdrm libomx liblombo_jpeg libvideo \
				libdtbased_tk libnn_engine libopenvx libopenvx-nn libnn-priv libnn libsoft-nn liblog libcut libxlsreader libxlsxwriter \
				libnet_utils libethernet libwifi_station libwpa_client libwifi_softap libflip librecorder libaudio libzbar

include $(BUILD_SHARED_LIBRARY)
