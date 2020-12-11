LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

# 模块名称必须定义，要求唯一且不包含空格
LOCAL_MODULE := file_http_download

# optional: 模块不会安装到rootfs
# 实际使用时要注释此行
LOCAL_MODULE_TAGS := optional

# 源文件列表
LOCAL_SRC_FILES := \
	file_http_download.c

# 头文件路径列表
LOCAL_C_INCLUDES := \
	system/core/prebuilts/lib/curl/include \

# 注释此行如果你希望保留符合和调试信息
#LOCAL_STRIP_MODULE := false

# C编译器选项
LOCAL_CFLAGS :=

# C++编译器选项
LOCAL_CPPFLAGS :=

# 链接器选项
# 如果需要引用sysroot中的库，请在此指定，例如引用pthread和math
# LOCAL_LDFLAGS := -lpthread -lm
LOCAL_LDFLAGS := -lpthread -lm -lcurl -lssl

# 依赖的静态库
LOCAL_STATIC_LIBRARIES :=

# 依赖的动态库
LOCAL_SHARED_LIBRARIES := \
	liblog \

include $(BUILD_EXECUTABLE)
