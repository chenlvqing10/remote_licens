#ifndef __VIDEO_ENCODE_PRIV_H__
#define __VIDEO_ENCODE_PRIV_H__

#ifdef __ANDROID__
#include "android_hw_api.h"
#include "lombo_util.h"

#define UTIL_DEV            "/dev/lb_util"
#define MUXER_PACKET_SIZE 32
typedef struct win_rect {
	int   x;
	int   y;
	int   width;
	int   height;
} win_rect_t;

#else

#include "lombo_muxer.h"
#include "media_common.h"
#define MUXER_PACKET_SIZE sizeof(muxer_vpacket_t)

#endif

#endif
