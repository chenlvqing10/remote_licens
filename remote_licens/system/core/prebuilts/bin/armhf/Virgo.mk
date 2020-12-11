LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

# 模块名称必须定义，要求唯一且不包含空格
# 预编译模块必须定义LOCAL_MODULE_CLASS，可取值
#   EXECUTABLES
#   SHARED_LIBRARIES
#   STATIC_LIBRARIES
#   ETC
LOCAL_MODULE := viss_isp
LOCAL_MODULE_CLASS := EXECUTABLES

# optional: 模块不会安装到rootfs
# 实际使用时要注释此行
#LOCAL_MODULE_TAGS := optional

# 源文件
LOCAL_SRC_FILES := $(LOCAL_MODULE)

# 安装目录，未定义则按照LOCAL_MODULE_CLASS定义确定
#LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)

include $(BUILD_PREBUILT)
