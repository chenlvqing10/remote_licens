LOCAL_PATH :=$(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := sample_face_det

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := \
	sample_face_det.cpp

LOCAL_CFLAGS :=
LOCAL_LDFLAGS := -lpthread -lm

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/common \
	frameworks/nn/include

LOCAL_SHARED_LIBRARIES := liblombo_malloc libopenvx libopenvx-nn libnn liblog \
	libdtbased_tk libnn-priv libnn_engine

include $(BUILD_EXECUTABLE)

#################################################################################
include $(CLEAR_VARS)

LOCAL_MODULE := sample_det_batch

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := \
	sample_det_batch.cpp \
	common/tsemaphore.cpp

LOCAL_CFLAGS :=
LOCAL_LDFLAGS := -lpthread -lm

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/common \
	frameworks/nn/include

LOCAL_SHARED_LIBRARIES := liblombo_malloc libopenvx libopenvx-nn libnn liblog \
	libdtbased_tk libnn-priv libnn_engine

include $(BUILD_EXECUTABLE)

#################################################################################
include $(CLEAR_VARS)

LOCAL_MODULE := sample_rec

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := \
	sample_rec.cpp

LOCAL_CFLAGS :=
LOCAL_LDFLAGS := -lpthread -lm

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/common \
	frameworks/nn/include

LOCAL_SHARED_LIBRARIES := liblombo_malloc libopenvx libopenvx-nn libnn liblog \
	libdtbased_tk libnn-priv libnn_engine

include $(BUILD_EXECUTABLE)

#################################################################################
