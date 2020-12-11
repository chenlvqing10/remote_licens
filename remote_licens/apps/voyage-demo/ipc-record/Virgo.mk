LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES :=   \
	main.cpp

LOCAL_C_INCLUDES += \
	external/ipc/include

LOCAL_MODULE:= ipc-record
#LOCAL_MODULE_TAGS := optional

# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := false

LOCAL_LDFLAGS += -lpthread

# 依赖的静态库
LOCAL_STATIC_LIBRARIES :=

LOCAL_SHARED_LIBRARIES := \
	librtsp_cli \
	libonvif \
	libcurl

include $(BUILD_EXECUTABLE)

-include $(LOCAL_PATH)/test/res/res.mk
#######################################################################################
include $(call all-makefiles-under,$(LOCAL_PATH))