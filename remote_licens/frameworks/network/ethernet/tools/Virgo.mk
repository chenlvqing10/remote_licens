LOCAL_PATH:= $(call my-dir)

# executable - try_delay
include $(CLEAR_VARS)

LOCAL_MODULE := try_delay

LOCAL_SRC_FILES := try_delay.c

LOCAL_LDFLAGS := -lpthread -lm

include $(BUILD_EXECUTABLE)

# executable - reg_check
include $(CLEAR_VARS)

LOCAL_MODULE := reg_check

LOCAL_SRC_FILES := reg_check.c

LOCAL_LDFLAGS := -lpthread -lm

include $(BUILD_EXECUTABLE)