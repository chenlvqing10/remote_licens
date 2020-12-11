LOCAL_PATH := $(my-dir)
include $(CLEAR_VARS)

libiniparser_sources := \
	src/iniparser.c \
	src/dictionary.c

include $(CLEAR_VARS)
LOCAL_MODULE := libiniparser
LOCAL_SRC_FILES := $(libiniparser_sources)
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libiniparser
LOCAL_SRC_FILES := $(libiniparser_sources)
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := iniexample
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := \
	example/iniexample.c
LOCAL_SHARED_LIBRARIES := libiniparser
include $(BUILD_EXECUTABLE)
