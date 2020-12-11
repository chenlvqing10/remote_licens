LOCAL_PATH:= $(call my-dir)

# executable - net_dev_open_test
include $(CLEAR_VARS)

LOCAL_MODULE := net_dev_open_test

LOCAL_SRC_FILES := net_dev_open_test.c

LOCAL_C_INCLUDES := frameworks/network/net_utils/

LOCAL_LDFLAGS := -lpthread -lm

LOCAL_SHARED_LIBRARIES := liblog libnet_utils

include $(BUILD_EXECUTABLE)

# executable - mac_addr_test
include $(CLEAR_VARS)

LOCAL_MODULE := mac_addr_test

LOCAL_SRC_FILES := mac_addr_test.c

LOCAL_C_INCLUDES := frameworks/network/net_utils/

LOCAL_LDFLAGS := -lpthread -lm

LOCAL_SHARED_LIBRARIES := liblog libnet_utils

include $(BUILD_EXECUTABLE)

# executable - set_ip_test
include $(CLEAR_VARS)

LOCAL_MODULE := set_ip_test

LOCAL_SRC_FILES := set_ip_test.c

LOCAL_C_INCLUDES := frameworks/network/net_utils/

LOCAL_LDFLAGS := -lpthread -lm

LOCAL_SHARED_LIBRARIES := liblog libnet_utils

include $(BUILD_EXECUTABLE)

# executable - dhcp_test
include $(CLEAR_VARS)

LOCAL_MODULE := dhcp_test

LOCAL_SRC_FILES := dhcp_test.c

LOCAL_C_INCLUDES := frameworks/network/net_utils/

LOCAL_LDFLAGS := -lpthread -lm

LOCAL_SHARED_LIBRARIES := liblog libnet_utils

include $(BUILD_EXECUTABLE)

# executable - net_connect_test
include $(CLEAR_VARS)

LOCAL_MODULE := net_connect_test

LOCAL_SRC_FILES := net_connect_test.c

LOCAL_C_INCLUDES := frameworks/network/net_utils/

LOCAL_LDFLAGS := -lpthread -lm

LOCAL_SHARED_LIBRARIES := liblog libnet_utils

include $(BUILD_EXECUTABLE)

# executable - inet_connect_test
include $(CLEAR_VARS)

LOCAL_MODULE := inet_connect_test

LOCAL_SRC_FILES := inet_connect_test.c

LOCAL_C_INCLUDES := frameworks/network/net_utils/

LOCAL_LDFLAGS := -lpthread -lm

LOCAL_SHARED_LIBRARIES := liblog libnet_utils

include $(BUILD_EXECUTABLE)

# executable - inet_monitor_test
include $(CLEAR_VARS)

LOCAL_MODULE := inet_monitor_test

LOCAL_SRC_FILES := inet_monitor_test.c

LOCAL_C_INCLUDES := frameworks/network/net_utils/

LOCAL_LDFLAGS := -lpthread -lm

LOCAL_SHARED_LIBRARIES := liblog libnet_utils

include $(BUILD_EXECUTABLE)