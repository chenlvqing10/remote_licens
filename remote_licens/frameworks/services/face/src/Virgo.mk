LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

# 模块名称必须定义，要求唯一且不包含空格
LOCAL_MODULE := libface_svc

# optional: 模块不会安装到rootfs
# 实际使用时要注释此行
LOCAL_MODULE_TAGS := optional

# 源文件列表
LOCAL_SRC_FILES := \
	face_svc_conf.c \
	face_svc_main.c \
	face_svc_media.c \
	face_svc_nn.c \
	face_svc_isp.c \
	queue.c \
	common/image/bmp_dec.c \
	common/image/png_dec.c \
	common/image/recg_img.c \
	common/time/time_conv.c \
	common/file/file_ops.c \
	common/zbar/zbar_det.c \
	common/temp/temp_measure.c \
	common/temp/temp_measure_device_001.c \
	common/map/rgb2nir.cpp \
	common/map/rgb2nir_core.cpp \
	common/map/rgb2nir_util.cpp \
	common/map/rgb2nir_new.cpp \
	common/private/private.c \

# 头文件路径列表
#COMP_PATH := $(LOCAL_PATH)/../../../media/
#LOCAL_C_INCLUDES := $(shell find $(COMP_PATH)/ -type d)
LOCAL_C_INCLUDES += \
	frameworks/services/face/include \
	frameworks/nn/include \
	frameworks/nn/inc \
	frameworks/media/hal/include \
	frameworks/media/video \
	frameworks/media/audio \
	frameworks/media/recorder \
	frameworks/media/include \
	frameworks/media/jpeg \
	frameworks/media/jpeg/jpeg \
	frameworks/media/jpeg/jpeg/rescale \
	frameworks/media/cut \
	frameworks/media/flip \
	external/zbar/include \
	$(LOCAL_PATH)/common/include \

# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := false

# C编译器选项
LOCAL_CFLAGS := -Wall -Werror -g -O0

# C++编译器选项
LOCAL_CPPFLAGS := -g -O0

# 链接器选项
# 如果需要引用sysroot中的库，请在此指定，例如引用pthread和math
# LOCAL_LDFLAGS := -lpthread -lm
LOCAL_LDFLAGS := -lpthread -ldl -lpng \
	-lopencv_core \
	-lopencv_highgui \
	-lopencv_imgproc \
	-lopencv_imgcodecs \
	-lopencv_videoio \
	-lopencv_calib3d \
	-lopencv_features2d \
	-lopencv_flann \

# 依赖的静态库
LOCAL_STATIC_LIBRARIES :=

# 依赖的动态库
LOCAL_SHARED_LIBRARIES := liblombo_malloc libomx libvideo liblog \
	libdtbased_tk libnn_engine libopenvx libopenvx-nn libnn libnn-priv \
	liblombo_jpeg libcut libiniparser libflip libcut libzbar librecorder libaudio

include $(BUILD_SHARED_LIBRARY)

###################################################
include $(CLEAR_VARS)
# 模块名称必须定义，要求唯一且不包含空格
LOCAL_MODULE := libface_svc_test

# optional: 模块不会安装到rootfs
# 实际使用时要注释此行
LOCAL_MODULE_TAGS := optional

# 源文件列表
LOCAL_SRC_FILES := \
	face_svc_test.c \

# 头文件路径列表
LOCAL_C_INCLUDES += \
	frameworks/services/face/include \
	frameworks/nn/include \
	frameworks/nn/inc \
	frameworks/media/hal/include \
	frameworks/media/video \
	frameworks/media/audio \
	frameworks/media/recorder \
	frameworks/media/include \
	frameworks/media/jpeg \
	frameworks/media/jpeg/jpeg \
	frameworks/media/cut \
	frameworks/media/flip \
	external/zbar/include \
	$(LOCAL_PATH)/common/include \

# 注释此行如果你希望保留符合和调试信息
#LOCAL_STRIP_MODULE := false

# C编译器选项
LOCAL_CFLAGS := -Wall -Werror

# C++编译器选项
LOCAL_CPPFLAGS :=

# 链接器选项
# 如果需要引用sysroot中的库，请在此指定，例如引用pthread和math
# LOCAL_LDFLAGS := -lpthread -lm
LOCAL_LDFLAGS := -lpthread -ldl -lpng

# 依赖的静态库
LOCAL_STATIC_LIBRARIES :=

# 依赖的动态库
LOCAL_SHARED_LIBRARIES := liblombo_malloc libface_svc libiniparser \
		libdrm libomx liblombo_jpeg libvideo \
		libdtbased_tk libnn_engine libopenvx libopenvx-nn libnn-priv libnn liblog libsoft-nn libcut libxlsreader libxlsxwriter \
		libnet_utils libethernet libwifi_station libwpa_client libwifi_softap libflip libzbar librecorder libaudio

include $(BUILD_EXECUTABLE)





###################################################
include $(CLEAR_VARS)
# 模块名称必须定义，要求唯一且不包含空格
LOCAL_MODULE := nn_self_test

# optional: 模块不会安装到rootfs
# 实际使用时要注释此行
# LOCAL_MODULE_TAGS := optional

# 源文件列表
LOCAL_SRC_FILES := \
	test/face_svc_dec.c \
	test/face_svc_json.cpp \
	test/face_svc_event.c \
	test/face_svc_log.c \
	test/face_svc_sample.cpp \
	test/face_svc_nn_self_test.c 
	

FILE_INCLUDES = $(shell find frameworks/media/ -type d)
# 头文件路径列表
LOCAL_C_INCLUDES += $(FILE_INCLUDES)
LOCAL_C_INCLUDES += \
	frameworks/services/face/include \
	frameworks/nn/include \
	frameworks/nn/inc \
	external/zbar/include \
	$(LOCAL_PATH)/common/include \
	system/core/include \
	test/ 
	

# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := false

# C编译器选项
LOCAL_CFLAGS :=  -DFACE_SVC_NN_SELF_TEST -g -O0

# C++编译器选项
LOCAL_CPPFLAGS :=

# 链接器选项
# 如果需要引用sysroot中的库，请在此指定，例如引用pthread和math
# LOCAL_LDFLAGS := -lpthread -lm
LOCAL_LDFLAGS := -lpthread -ldl -lpng -lomx_demuxer -lomx 

# 依赖的静态库
LOCAL_STATIC_LIBRARIES := 

# 依赖的动态库
LOCAL_SHARED_LIBRARIES := liblombo_malloc libface_svc libiniparser \
		libdrm libomx liblombo_jpeg libvideo \
		libdtbased_tk libnn_engine libopenvx libopenvx-nn libnn-priv libnn liblog libsoft-nn libcut  \
		libflip libzbar librecorder libaudio \
		libdrm libomx liblog libvideo libomx_demuxer  libmediaplayer

include $(BUILD_EXECUTABLE)

###################################################
include $(CLEAR_VARS)
# 模块名称必须定义，要求唯一且不包含空格
LOCAL_MODULE := rgb2nir_new_test

# optional: 模块不会安装到rootfs
# 实际使用时要注释此行
LOCAL_MODULE_TAGS := optional

# 源文件列表
LOCAL_SRC_FILES := \
	rgb2nir_new_test.c \

# 头文件路径列表
LOCAL_C_INCLUDES += \
	frameworks/services/face/include \
	frameworks/nn/include \
	frameworks/nn/inc \
	frameworks/media/hal/include \
	frameworks/media/video \
	frameworks/media/audio \
	frameworks/media/recorder \
	frameworks/media/include \
	frameworks/media/jpeg \
	frameworks/media/jpeg/jpeg \
	frameworks/media/cut \
	frameworks/media/flip \
	external/zbar/include \
	$(LOCAL_PATH)/common/include \

# 注释此行如果你希望保留符合和调试信息
#LOCAL_STRIP_MODULE := false

# C编译器选项
LOCAL_CFLAGS := -Wall -Werror

# C++编译器选项
LOCAL_CPPFLAGS :=

# 链接器选项
# 如果需要引用sysroot中的库，请在此指定，例如引用pthread和math
# LOCAL_LDFLAGS := -lpthread -lm
LOCAL_LDFLAGS := -lpthread -ldl -lpng

# 依赖的静态库
LOCAL_STATIC_LIBRARIES :=

# 依赖的动态库
LOCAL_SHARED_LIBRARIES := liblombo_malloc libface_svc libiniparser \
		libdrm libomx liblombo_jpeg libvideo \
		libdtbased_tk libnn_engine libopenvx libopenvx-nn libnn-priv libnn liblog libsoft-nn libcut libliving libxlsreader libxlsxwriter libnirdt \
		libnet_utils libethernet libwifi_station libwpa_client libwifi_softap libflip libzbar librecorder libaudio

include $(BUILD_EXECUTABLE)
