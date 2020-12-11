LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := ipc-face-recg

#LOCAL_MODULE_TAGS := optional

CMD_SRC_PATH := $(LOCAL_PATH)/cmd

UI_FILES := $(shell find $(CMD_SRC_PATH)/.)
UI_FILES := $(UI_FILES:$(CMD_SRC_PATH)/./%=$(CMD_SRC_PATH)/%)
UI_SRC_FILES := $(filter %.cpp %.c,$(UI_FILES))
UI_SRC_FILES := $(UI_SRC_FILES:$(LOCAL_PATH)/%=%)

LOCAL_SRC_FILES += $(UI_SRC_FILES)

LOCAL_SRC_FILES += api/src/take_photo.c
LOCAL_SRC_FILES += http/http-api/src/config_db.c
LOCAL_SRC_FILES += http/http-api/src/face_db.c
LOCAL_SRC_FILES += http/http-api/src/lb_os_http.c
LOCAL_SRC_FILES += http/http-api/src/record_db.c
LOCAL_SRC_FILES += http/http-api/src/sys_http.c
LOCAL_SRC_FILES += http/json/src/cJSON.c
LOCAL_SRC_FILES += http/restful-api/src/base64.c
LOCAL_SRC_FILES += http/restful-api/src/buffer.c
LOCAL_SRC_FILES += http/restful-api/src/cloud_biz_api.c
LOCAL_SRC_FILES += http/restful-api/src/connect.c
LOCAL_SRC_FILES += http/restful-api/src/http.c
LOCAL_SRC_FILES += http/restful-api/src/sys_cfg.c
LOCAL_SRC_FILES += http/restful-api/src/sys_info.c
LOCAL_SRC_FILES += http/restful-api/src/web_biz_api.c
LOCAL_SRC_FILES += http/restful-api/src/web_sys_cfg.c
LOCAL_SRC_FILES += http/webserver/src/mongoose/mongoose.c
LOCAL_SRC_FILES += http/webserver/src/ws_http.c
LOCAL_SRC_FILES += http/http_main.c
LOCAL_SRC_FILES += http/parse.c

LOCAL_C_INCLUDES := \
	frameworks/services/face/include \
	frameworks/services/face/src \
	frameworks/media/jpeg/jpeg \
	frameworks/media/hal/include \
	frameworks/media/cut \
	frameworks/media/flip \
	frameworks/media/recorder \
	frameworks/media/include \
	frameworks/media/video \
	frameworks/network/wifi/wifi_station/lib \
	frameworks/network/net_utils \

UI_H_FILES := $(shell find $(CMD_SRC_PATH)/ -type d)
LOCAL_C_INCLUDES += $(UI_H_FILES)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/api/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/http
LOCAL_C_INCLUDES += $(LOCAL_PATH)/http/http-api/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/http/json/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/http/restful-api/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/http/webserver/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/http/webserver/src/mongoose

LOCAL_C_INCLUDES += $(shell find apps/voyage-demo/lib_face-recg/include -type d)
LOCAL_C_INCLUDES += $(shell find apps/voyage-demo/lib_onvif/inc -type d)

# 注释此行如果你希望保留符合和调试信息
#LOCAL_STRIP_MODULE := false

# C编译器选项
LOCAL_CFLAGS := -Wall -Werror -DWS_MG
#LOCAL_CFLAGS :=

# C++编译器选项
LOCAL_CPPFLAGS :=

# 链接器选项
# 如果需要引用sysroot中的库，请在此指定，例如引用pthread和math
# LOCAL_LDFLAGS := -lpthread -lm
LOCAL_LDFLAGS := -lpthread -lm -lts -lz -ljpeg -lpng -lfreetype -lxml2 -lsqlite3

# 依赖的静态库
LOCAL_STATIC_LIBRARIES :=

# 依赖的动态库
LOCAL_SHARED_LIBRARIES := liblombo_malloc libface_svc libiniparser libdrm \
				libomx liblombo_jpeg libvideo \
				libdtbased_tk libnn_engine libopenvx libopenvx-nn libnn-priv libnn libsoft-nn liblog libcut \
				libxlsreader libxlsxwriter librecorder libaudio libflip lib_face-recg \
				libnet_utils libethernet libwifi_station libwpa_client libwifi_softap libzbar lib_onvif_server

include $(BUILD_EXECUTABLE)
