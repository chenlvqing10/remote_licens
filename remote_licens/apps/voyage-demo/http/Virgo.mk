LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := ws_http

#LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := \
	biz_api.c \
	cJSON.c \
	http_main.c \
	mongoose.c \
	web_biz_api.c \
	ws_http.c

LOCAL_C_INCLUDES := 

# 注释此行如果你希望保留符合和调试信息
#LOCAL_STRIP_MODULE := false

# C编译器选项
LOCAL_CFLAGS := -Wall -Werror
#LOCAL_CFLAGS :=

# C++编译器选项
LOCAL_CPPFLAGS :=

# 链接器选项
# 如果需要引用sysroot中的库，请在此指定，例如引用pthread和math
# LOCAL_LDFLAGS := -lpthread -lm
LOCAL_LDFLAGS := -lpthread -lm -lts -lz -ljpeg -lpng -lfreetype

# 依赖的静态库
LOCAL_STATIC_LIBRARIES :=

# 依赖的动态库
LOCAL_SHARED_LIBRARIES := liblog

include $(BUILD_EXECUTABLE)
