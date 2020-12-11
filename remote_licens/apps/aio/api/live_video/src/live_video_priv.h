#ifndef _LIVE_VIDEO_PRIV_H
#define _LIVE_VIDEO_PRIV_H

#include "live_video.h"
#include "video_recorder.h"

typedef struct live_video_recorder_para {
	int channel;
	int watermark;
	int stop_flag;
	int preview_flag;
	int record_flag;
	rec_param_t rec_para;
	char *name_prefix;
	char *source;
	char *path;
} live_video_recorder_para_t;

typedef struct live_video_stream_recorder {
	void *astream;
	void *vstream;
	void *recorder;
	live_video_recorder_para_t recorder_para;
	live_video_state_t state;
} live_video_stream_recorder_t;

extern live_video_stream_recorder_t recoders[];

#define live_video_get_recoder(ch) (&recoders[ch])
#define live_video_get_recoder_rec_para(ch) (&(recoders[ch].recorder_para.rec_para))


#define live_video_logd(fmt, arg...) \
	ALOG(LOG_DEBUG, LOG_TAG, "<[%s %04d]> " \
		fmt, __func__, __LINE__,  ##arg);

#define live_video_loge(fmt, arg...) \
			ALOG(LOG_ERROR, LOG_TAG, "<[%s %04d]> " \
				fmt, __func__, __LINE__,  ##arg);


extern int live_video_get_recoder_channels(void);
extern int live_video_reset(int channel);

#endif /* _LIVE_VIDEO_PRIV_H */


