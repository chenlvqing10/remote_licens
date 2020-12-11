LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := libdisplay

LOCAL_STRIP_MODULE := false

LOCAL_SRC_FILES := \
	common.c \
	common-drm.c \
	common-rot.c \
	common-modeset.c \
	common-drawing.c \
	common-scale.c

LOCAL_C_INCLUDES := \
	external/libdrm \
	external/libdrm/include/drm \
	$(LOCAL_PATH)/../include

#LOCAL_LDFLAGS := -lrt -lpthread
LOCAL_SHARED_LIBRARIES := libdrm liblog

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_SRC_FILES :=           \
	testpat.c

LOCAL_MODULE:= testpat
#LOCAL_MODULE_TAGS := optional

# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := false

LOCAL_LDFLAGS += -lrt -lpthread

LOCAL_C_INCLUDES += \
	external/libdrm \
	external/libdrm/include/drm \
	$(LOCAL_PATH)/../include

LOCAL_SHARED_LIBRARIES := libdrm liblog
LOCAL_STATIC_LIBRARIES := libdisplay

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_SRC_FILES :=           \
	db.c

LOCAL_MODULE:= db
#LOCAL_MODULE_TAGS := optional

# 注释此行如果你希望保留符合和调试信息
LOCAL_STRIP_MODULE := false

LOCAL_LDFLAGS += -lrt -lpthread

LOCAL_C_INCLUDES += \
	external/libdrm \
	external/libdrm/include/drm \
	$(LOCAL_PATH)/../include

LOCAL_SHARED_LIBRARIES := libdrm liblog
LOCAL_STATIC_LIBRARIES := libdisplay

include $(BUILD_EXECUTABLE)
