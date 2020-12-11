LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PREBUILT_LIBS := \
	libaacdemuxer.so \
	libapedemuxer.so \
	libflacdemuxer.so \
	libg729demuxer.so \
	libmovdemuxer.so \
	libflvdemuxer.so \
	libmpgdemuxer.so \
	libmp3demuxer.so \
	liboggdemuxer.so \
	libwavdemuxer.so \
	libascdemuxer.so \
	libassdemuxer.so \
	libjssdemuxer.so \
	liblrcdemuxer.so \
	libovrdemuxer.so \
	libsmidemuxer.so \
	libsrtdemuxer.so \
	libstldemuxer.so \
	libsubdemuxer.so \
	libttsdemuxer.so \
	libtxtdemuxer.so \
	libvsfdemuxer.so \
	libsubqueue.so
include $(BUILD_MULTI_PREBUILT)
