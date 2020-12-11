LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := acc-test

#LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := \
	file_list.c \
	acc_svc.c \
	acc_svc_nn.c \
	main.c

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH) \
	frameworks/nn/include \
	frameworks/nn/inc \
	frameworks/media/jpeg/jpeg \
	frameworks/media/hal/include \
	frameworks/media/cut \
	frameworks/media/flip \


# 注释此行如果你希望保留符合和调试信息
#LOCAL_STRIP_MODULE := false

# C编译器选项
LOCAL_CFLAGS := -Wall 
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
LOCAL_SHARED_LIBRARIES :=libdrm liblombo_malloc libomx liblombo_jpeg libalc libvccore libie_jpg libid_jpg libvideo \
				libdtbased_tk libnn_engine libopenvx libopenvx-nn libnn-priv libnn libsoft-nn liblog libcut libxlsreader libxlsxwriter \
				libflip

include $(BUILD_EXECUTABLE)
