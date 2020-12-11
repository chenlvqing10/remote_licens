LOCAL_PATH:= $(call my-dir)

# shared library - libnet_utils.so
include $(CLEAR_VARS)

LOCAL_MODULE := libnet_utils

LOCAL_SRC_FILES := net_utils.c ip_config.c inet_monitor.c

LOCAL_LDFLAGS := -lpthread -lm

LOCAL_SHARED_LIBRARIES := liblog

PRODUCT_COPY_FILES += $(LOCAL_PATH)/scripts/dns_config:rootfs/etc/init.d/dns_config
PRODUCT_COPY_FILES += $(LOCAL_PATH)/scripts/ip_drop:rootfs/etc/init.d/ip_drop

include $(BUILD_SHARED_LIBRARY)

# include all subdir Makefiles
include $(call all-makefiles-under, $(LOCAL_PATH))