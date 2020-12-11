LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PREBUILT_LIBS := \
	libaacdemuxer.so \
	libamrdemuxer.so \
	libapedemuxer.so \
	libflacdemuxer.so \
	libg729demuxer.so \
	libmovdemuxer.so \
	libmp3demuxer.so \
	liboggdemuxer.so \
	libwavdemuxer.so \
	libmkvdemuxer.so \
	libtsdemuxer.so \
	libflvdemuxer.so \
	libmpgdemuxer.so \
	libavidemuxer.so \
	libh264demuxer.so \
	libassdemuxer.so \
	libsrtdemuxer.so \
	libascdemuxer.so \
	libjssdemuxer.so \
	liblrcdemuxer.so \
	libovrdemuxer.so \
	libsmidemuxer.so \
	libstldemuxer.so \
	libsubdemuxer.so \
	libttsdemuxer.so \
	libtxtdemuxer.so \
	libvsfdemuxer.so \
	libsubqueue.so


include $(BUILD_MULTI_PREBUILT)
