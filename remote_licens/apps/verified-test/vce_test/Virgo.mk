LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
  vce_recorder.c

LOCAL_C_INCLUDES := \
  $(LOCAL_PATH)/include \
  $(LOCAL_PATH)/../../../frameworks/media/hal/include \

LOCAL_SHARED_LIBRARIES := \
  libalc \
  libvccore \
  liblog

LOCAL_LDFLAGS += -lpthread

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= video_recorder
include $(BUILD_EXECUTABLE)