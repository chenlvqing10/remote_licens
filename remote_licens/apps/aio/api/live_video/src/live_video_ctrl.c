#include <errno.h>
#include <signal.h>
#include <getopt.h>
#include <pthread.h>
#include "dirent.h"
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>

#define LOG_TAG "live_video"
#include <log/log.h>

#include "assert.h"
#include "live_server.h"
#include "video_stream.h"
#include "audio_stream.h"
#include "video_recorder.h"
#include "live_video.h"
#include "live_video_priv.h"

#if 0
static int live_video_serialize_para(int channel, 
											const live_video_para_t *para);
static int live_video_deserialize_para(int channel, 
											   live_video_para_t *para);
#endif



int live_video_set_para(int channel, 
								const live_video_para_t *para)
{
	/*live_video_para_t cur = {0};*/
	int ret = 0;
	enc_para_t enc_para = {0};
	live_video_logd("enter");

	if ((para->bitrate < 0) ||
		(para->frame_rate < 0) ||
		(para->width < 0) ||
		(para->height < 0) || 
		(channel < 0) || 
		(channel >= live_video_get_recoder_channels()) || 
		(para == NULL)){
			live_video_loge("wrong para");
			ret = -1;
		}else{
			live_video_logd("para: width=%d height=%d  frame_rate=%d bitrate=%d",
							para->width,
							para->height,
							para->frame_rate,
							para->bitrate);
			live_video_get_recoder_rec_para(channel)->width = para->width;
			live_video_get_recoder_rec_para(channel)->height = para->height;
			live_video_get_recoder_rec_para(channel)->frame_rate = para->frame_rate;
			live_video_get_recoder_rec_para(channel)->bitrate = para->bitrate;

			ret = live_video_reset(channel);
			if (!ret){
				live_video_logd("ok");
			}else{
				live_video_loge("failed");
			}
			(void)enc_para;
#if 0
			enc_para.bitrate = para->bitrate;
			enc_para.width = para->width;
			enc_para.height = para->height;
			enc_para.fmt = live_video_get_recoder_rec_para(channel)->venc_format;
			enc_para.rotate = live_video_get_recoder_rec_para(channel)->enc_rot;

			live_video_reset();
			ret = video_rec_set_para(live_video_get_recoder(channel)->recorder, 
								&enc_para);
			
			if (live_video_get_recoder(channel)->state == LIVE_VIDEO_RUNING){
				ret |= video_rec_stop(live_video_get_recoder(channel)->recorder);
				if (0 == ret){
					ret |= video_rec_start(live_video_get_recoder(channel)->recorder);
					if (0 == ret){
						//live_video_serialize_para(para);
						live_video_logd("start ok after change para");
					}else{
						live_video_loge("start failed after change para");
					}
				}else{
					live_video_loge("stop failed before change para");
				}
			}else{
				live_video_logd("video is not running, just change para");
			}
#endif
		}
		return ret;
}

int live_video_get_para(int channel,
								live_video_para_t *para)
{
	int ret = 0;
	if ((channel < 0) || 
		(channel >= live_video_get_recoder_channels()) || 
		(para == NULL)){
			live_video_loge("wrong para");
			ret = -1;
		}else{
			para->width = live_video_get_recoder_rec_para(channel)->width;
			para->height = live_video_get_recoder_rec_para(channel)->height;
			para->frame_rate = live_video_get_recoder_rec_para(channel)->frame_rate;
			para->bitrate = live_video_get_recoder_rec_para(channel)->bitrate;
		}
	return ret;
}
#if 0
static int live_video_serialize_para(int channel, 
											const live_video_para_t *para)
{
	char key[64] = {0};
	char value[256] = {0};
	int ret = 0;
	snprintf(key, 64, "live_video_%d", channel);
	snprintf(value, 256, "frame_rate:%d; width:%d; height:%d; bitrate:%d",
			para->frame_rate,
			para->width,
			para->height,
			para->bitrate);
	return ret;
}


static int live_video_deserialize_para(int channel, 
											   live_video_para_t *para)
{
	/*const char *token = 0;*/
	char value[256] = {0};
	int ret = 0;
	ret = sscanf(value, "frame_rate:%d; width:%d; height:%d; bitrate:%d",
		&para->frame_rate,
		&para->width,
		&para->height,
		&para->bitrate);
	if (ret > 0){

	}else{
		live_video_loge(" failed, error:%s",
			strerror(errno));
		ret = -1;
	}
	return ret;
}

#endif


