#ifndef __LOMBO_DEC_COMMON_H__
#define __LOMBO_DEC_COMMON_H__

#include "demuxer_plugin.h"
#define DMX_MAX_EXSUB_NUM	(32)
#define DMX_SUBTITLE_STREAM_NUM		(2)
typedef struct omx_mediainfo {
	dmx_mediainfo_t mediainfo;
	int cur_video_idx;
	int cur_audio_idx;
	int cur_sub_idx;
	int video_supported_flag;
	int audio_supported_flag;
	int sub_supported_flag;
	int64_t start_time;
} omx_mediainfo_t;
typedef enum {
	SUB_STYLE_EMBEDED,
	SUB_STYLE_EXTERNAL,
	SUB_STYLE_MAX
} subtitle_style_e;
typedef struct exsub_info_tag {
	dmx_subinfo_t stream[DMX_MAX_EXSUB_NUM];
	OMX_S32 stream_num;/*total subtile stream number*/
} exsub_info_t;

#endif /* __LOMBO_DEC_COMMON_H__ */
