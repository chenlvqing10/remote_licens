LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

# 模块名称必须定义，要求唯一且不包含空格
LOCAL_MODULE := libobject_svc

# optional: 模块不会安装到rootfs
# 实际使用时要注释此行
LOCAL_MODULE_TAGS := optional

# 源文件列表
LOCAL_SRC_FILES := \
	object_svc_conf.c \
	object_svc_main.c \
	object_svc_media.c \
	object_svc_isp.c \
	queue.c

# 头文件路径列表
#COMP_PATH := $(LOCAL_PATH)/../../../media/
#LOCAL_C_INCLUDES := $(shell find $(COMP_PATH)/ -type d)
LOCAL_C_INCLUDES += \
	frameworks/services/object/include \
	frameworks/nn/include \
	frameworks/nn/inc \
	frameworks/media/hal/include \
	frameworks/media/video \
	frameworks/media/include \
	frameworks/media/jpeg \
	frameworks/media/cut

# 注释此行如果你希望保留符合和调试信息
#LOCAL_STRIP_MODULE := false

# C编译器选项
LOCAL_CFLAGS :=

# C++编译器选项
LOCAL_CPPFLAGS :=

# 链接器选项
# 如果需要引用sysroot中的库，请在此指定，例如引用pthread和math
# LOCAL_LDFLAGS := -lpthread -lm
LOCAL_LDFLAGS := -lpthread -ldl

# 依赖的静态库
LOCAL_STATIC_LIBRARIES :=

# 依赖的动态库
LOCAL_SHARED_LIBRARIES := libomx libvideo liblog \
	libdtbased_tk libnn_engine liblombo_malloc libopenvx libopenvx-nn libnn libnn-priv \
	liblombo_jpeg libcut libiniparser

include $(BUILD_SHARED_LIBRARY)

#######################################################################
include $(CLEAR_VARS)

LOCAL_MODULE := object_svc_test

LOCAL_SRC_FILES := object_svc_test.c

LOCAL_C_INCLUDES := \
	frameworks/nn/include \
	frameworks/media/jpeg

# 注释此行如果你希望保留符合和调试信息
#LOCAL_STRIP_MODULE := false

# C编译器选项
LOCAL_CFLAGS := -Wall -Werror

# C++编译器选项
#LOCAL_CPPFLAGS :=

# 链接器选项
# 如果需要引用sysroot中的库，请在此指定，例如引用pthread和math
# LOCAL_LDFLAGS := -lpthread -lm

# 依赖的静态库
#LOCAL_STATIC_LIBRARIES :=

# 依赖的动态库
LOCAL_SHARED_LIBRARIES :=  liblombo_jpeg \
				libdtbased_tk \
				libnn_engine \
				libopenvx \
				libopenvx-nn \
				libnn-priv \
				libnn liblog \
				liblombo_malloc \
				libsoft-nn \
				libdrm \
				libomx

include $(BUILD_EXECUTABLE)
