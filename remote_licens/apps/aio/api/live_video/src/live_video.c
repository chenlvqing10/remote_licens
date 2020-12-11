/*
 * live_video.c - Standard functionality for recorder.
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
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

//#include "elang.h"
#include "assert.h"
#include "live_server.h"
#include "video_stream.h"
#include "audio_stream.h"
#include "video_recorder.h"
#include "live_video.h"
#include "live_video_priv.h"
#include "frame_queue.h"

//#include "file_ops.h"
//#include "hotplug.h"

/* isp sub channel, 720P, /dev/video4 */
#define FACE_VIDEO_WIDTH	1280
#define FACE_VIDEO_HEIGHT	720

/* isp sub channel, 1080P */
#define LIVE_VIDEO_SUB_DEV	"/dev/video3" /* isp sub channel */
#define LIVE_VIDEO_SUB_BITRATE	1600000
#define LIVE_VIDEO_SUB_WIDTH	1280
#define LIVE_VIDEO_SUB_HEIGHT	800

/* h264 enc main stream */
#define ENC_MAIN_STREAM_WIDTH	1920
#define ENC_MAIN_STREAM_HEIGHT	1080

/* h264 enc sec stream */
#define ENC_SEC_STREAM_WIDTH	800
#define ENC_SEC_STREAM_HEIGHT	1408

#define LIVE_VIDEO_ROT		0
#define MAX_FACE_CNT		5
#define AV_STREAM_CALLBACK
#define INPUT_MODE		ENC_YUV420SP

#define MAIN_RECORDER_SAVE_FILE	0
#define RECORD_DURATION		300




//static faces_t face_info;
static int video_max_len;


live_video_stream_recorder_t recoders[] = {
    {
	.state = LIVE_VIDEO_DEINIT,
	.astream = NULL,
	.vstream = NULL,
	.recorder = NULL,
	.recorder_para = {
			.channel = 0,
			.source = LIVE_VIDEO_SUB_DEV,
			.rec_para = {
				.audio_channels = 2,
				.audio_sample_width = 16,
				.audio_sample_rate = 48000,
	      #ifdef AV_STREAM_CALLBACK
				.aenc_format = AUDIO_ENCODER_PCM,	/* PCM */
				.file_fmt = REC_OUTPUT_FORMAT_RAW,	/* H264 RAW */
	      #else
				.aenc_format = AUDIO_ENCODER_AAC,	/* AAC */
				.file_fmt = REC_OUTPUT_FORMAT_MP4,	/* MP4 */
	      #endif
				.venc_format = VIDEO_ENCODER_H264,

				.bitrate = LIVE_VIDEO_SUB_BITRATE,	/* 3200000 */
				.frame_rate = 25,
				.source_width = LIVE_VIDEO_SUB_WIDTH,
				.source_height = LIVE_VIDEO_SUB_HEIGHT,
				//.width = 1280,
				//.height = 800,
				.width = 800,
				.height = 1280,

				.rotate = VDISP_ROTATE_NONE,
				.enc_rot = VDISP_ROTATE_90,
			},
			.name_prefix = "isp",
		},
    },
    {
	.state = LIVE_VIDEO_DEINIT,
	.astream = NULL,
	.vstream = NULL,
	.recorder = NULL,
	.recorder_para = {
			.channel = 1,
			.source = LIVE_VIDEO_SUB_DEV,
			.rec_para = {
				.audio_channels = 2,
				.audio_sample_width = 16,
				.audio_sample_rate = 48000,
	      #ifdef AV_STREAM_CALLBACK
				.aenc_format = AUDIO_ENCODER_PCM,	/* PCM */
				.file_fmt = REC_OUTPUT_FORMAT_RAW,	/* H264 RAW */
	      #else
				.aenc_format = AUDIO_ENCODER_AAC,	/* AAC */
				.file_fmt = REC_OUTPUT_FORMAT_MP4,	/* MP4 */
	      #endif

				.venc_format = VIDEO_ENCODER_H264,

				.bitrate = LIVE_VIDEO_SUB_BITRATE/2,	/* 3200000 */
				.frame_rate = 25,
				.source_width = LIVE_VIDEO_SUB_WIDTH,
				.source_height = LIVE_VIDEO_SUB_HEIGHT,
				//.width = 768,
				//.height = 480,
				.width = 480,
				.height = 768,

				.rotate = VDISP_ROTATE_NONE,
				.enc_rot = VDISP_ROTATE_90,
			},
			.name_prefix = "isp",
		},
    },
};

typedef int (*enc_buf_handle_t)(void *hdl, avstream_t *stream);

static int live_video_enc_sec_stream_buf_handle(void *hdl, avstream_t *stream)
{
  live_video_stream_recorder_t *rec = (live_video_stream_recorder_t *)hdl;
  frame_dsc_s FrameDsc = {0};
  if (STREAM_TYPE_VIDEO == stream->type) {
    if (video_max_len < stream->data_size) {
	  video_max_len = stream->data_size;
	  live_video_logd(" video_max_len=%d\n", video_max_len);
    }

	/*live_video_logd("new packet");*/
    FrameDsc.pbyData = stream->data;
    FrameDsc.dwDataLen = stream->data_size;
    if (SYS_OK == frame_queue_push_node(rec->recorder_para.channel, &FrameDsc, 0)){
		/*live_video_logd("save %p size=%d ch=%d ok",
	      stream->data,
	      stream->data_size,
	      rec->recorder_para.channel);*/
    }
  }
  return 0;
}

#if 0
/**
 * create_recorder - get a recorder handle.
 * This function will load all components recorder needs.
 *
 * Returns recorder handle on success, NULL otherwise..
 */
static void *live_video_create_stream_recorder(void *rgb_hdl)
{
	live_video_stream_recorder_t *hdl = malloc(sizeof(live_video_stream_recorder_t));
	if (hdl == NULL)
		return NULL;

	memset(hdl, 0, sizeof(live_video_stream_recorder_t));
	hdl->vstream = rgb_hdl;
	hdl->astream = audio_stream_creat();
	hdl->recorder = video_rec_creat();
	video_rec_set_video_stream(hdl->recorder, hdl->vstream);
	video_rec_set_audio_stream(hdl->recorder, hdl->astream);

	return hdl;
}
#endif

/**
 * live_video_release_stream_recorder - release resource recorder used.
 *
 * @hdl: recorder handle
 *
 * Returns 0 on success, -EERROR otherwise..
 */
static void live_video_release_stream_recorder(live_video_stream_recorder_t *recorder)
{
	if (recorder->vstream != NULL) {
		video_stream_release(recorder->vstream);
		recorder->vstream = NULL;
	}

	if (recorder->astream != NULL) {
		audio_stream_release(recorder->astream);
		recorder->astream = NULL;
	}

	if (recorder->recorder != NULL) {
		video_rec_release(recorder->recorder);
		recorder->recorder = NULL;
	}
}




static int live_video_recorder_set_para(live_video_stream_recorder_t *recorder, rec_param_t *para)
{
	enc_para_t enc_para;
	vsrc_param_t vsrc_para;
	int ret;


	if (/*(0 != para->frame_rate)
	    &&*/ (0 != para->source_height)
	    && (0 != para->source_width)){
	    vsrc_para.frame_rate = para->frame_rate;
	    vsrc_para.height = para->source_height;
	    vsrc_para.width = para->source_width;
	    ret = video_stream_set_src_para(recorder->vstream, &vsrc_para);

	    live_video_logd("video_stream_set_src_para %s\n",
		    (!ret)?"ok":"error");
	    if (ret != 0)
		    return -1;
	}


	enc_para.bitrate = para->bitrate;
	enc_para.width = para->width;
	enc_para.height = para->height;
	enc_para.fmt = para->file_fmt;
	enc_para.rotate = para->enc_rot;
	ret = video_rec_set_para(recorder->recorder, &enc_para);

	live_video_logd("video_rec_set_para %s\n",
		(!ret)?"ok":"error");
	if (ret != 0)
		return -1;

	if (para->audio_channels)
		ret = audio_stream_set_channels(recorder->astream, para->audio_channels);
	if (para->audio_sample_width) {
		ret = audio_stream_set_sample_width(recorder->astream,
			para->audio_sample_width);
	}
	if (para->audio_sample_rate) {
		ret = audio_stream_set_sample_rate(recorder->astream,
			para->audio_sample_rate);
	}
	if (para->aenc_format)
		ret = audio_stream_set_coding_type(recorder->astream, para->aenc_format);

	return ret;
}





static void *live_video_start(char *source, int width, int height, int rot)
{
	void *hdl;
	int ret;
	vdisp_rotate_mode_e rotation;

	switch (rot) {
	case 90:
		rotation = VDISP_ROTATE_90;
		break;
	case 270:
		rotation = VDISP_ROTATE_270;
		break;
	default:
		rotation = VDISP_ROTATE_NONE;
		break;
	}

	hdl = video_stream_create();
	if (!hdl) {
		live_video_loge("stream create failed\n");
		return NULL;
	}

	ret = video_stream_set_camera_source(hdl, source);
	if (ret) {
		live_video_loge("video_stream_set_camera_source failed\n");
		return NULL;
	}

	ret = video_stream_set_camera_size(hdl, width, height);
	if (ret) {
		live_video_loge("video_stream_set_camera_size failed\n");
		return NULL;
	}

	ret = video_stream_set_rotate(hdl, rotation);
	if (ret) {
		live_video_loge("video_stream_set_rotate failed\n");
		return NULL;
	}

	ret = video_stream_start_camera(hdl);
	if (ret) {
		live_video_loge("video_stream_start_camera failed\n");
		return NULL;
	}else{
	    live_video_logd("video_stream_start_camera success\n");
	}

	return hdl;
}


static void live_video_set_as_config(void *config, rec_param_t *p_rec_param)
{
/*  vdisp_rotate_mode_e rot_mode = VDISP_ROTATE_NONE;
	if (config != NULL) {

		p_rec_param->bitrate = config->live_stream_bitrate;
		p_rec_param->source_width = config->live_stream_width;
		p_rec_param->source_height = config->live_stream_height;
		p_rec_param->width = config->live_stream_encode_width;
		p_rec_param->height = config->live_stream_encode_height;
		p_rec_param->enc_rot = rot_mode;

		live_video_logd("[%s:%d]Config Bitrate=%d, source_width=%d, source_height=%d \
			encode_width=%d, encode_height=%d, encode_rot=%d",
			__func__, __LINE__,
			p_rec_param->bitrate, p_rec_param->source_width,
			p_rec_param->source_height, p_rec_param->width,
			p_rec_param->height, p_rec_param->enc_rot);
	}*/
}

/* video enc for RTSP/RTP transfer */
static int live_video_setup_recoder(live_video_stream_recorder_t *rec , void *config, enc_buf_handle_t handle_cb)
{
	live_video_logd(" start\n");
	/* Initialization of buffer for storing real-time audio and video data */
	(void)config;
	int ret = 0;
	/* use face service's rgb hdl for video encoder */
	if (rec->vstream != NULL) {
		live_video_loge("don't need to start a new vstream");

		rec->astream = audio_stream_creat();
		rec->recorder = video_rec_creat();

		video_rec_set_video_stream(rec->recorder, rec->vstream);
		video_rec_set_audio_stream(rec->recorder, rec->astream);

		if (rec->recorder  == NULL) {
			live_video_loge("create recorder failed!");
			return -1;
		}
	} else {
		/* isp main channel, 1080P */
/*		rec->vstream = live_video_start(config->live_stream_device,
						config->live_stream_width,
						config->live_stream_height,
						0);*/
		rec->vstream = live_video_start(rec->recorder_para.source,
						rec->recorder_para.rec_para.source_width,
						rec->recorder_para.rec_para.source_height,
						0);
		if (!rec->vstream) {
			live_video_loge("live_video_start failed\n");
			return -1;
		}

		/* create rgb cam's recorder object */
		//live_video_create_stream_recorder(rec->vstream);
		rec->astream = audio_stream_creat();
		rec->recorder = video_rec_creat();


		video_rec_set_video_stream(rec->recorder, rec->vstream);
		video_rec_set_audio_stream(rec->recorder, rec->astream);


		if (rec->recorder == NULL) {
			live_video_loge("create recorder failed!\n");
			return -1;
		}
	}


	/* Modify the default value of record according to the configuration */
	//live_video_set_as_config(config, &(rec->recorder_para.rec_para));
	//rec->config = config;


	ret = video_stream_set_camera_source(rec->vstream, rec->recorder_para.source);
	if (ret) {
		live_video_loge("video_stream_set_camera_source failed\n");
		return -1;
	}
	/* set video/audio parameter */
	live_video_recorder_set_para(rec, &(rec->recorder_para.rec_para));

	/* set video/audio callback parameter */
	app_stream_cb_t cb;
	cb.app_data = rec;
	cb.buf_handle = handle_cb;

	video_rec_set_stream_cb(rec->recorder, &cb);
	//video_rec_start(g_sec_recorder->recorder);
	rec->state = LIVE_VIDEO_INIT;
	return 0;
}

static int live_video_start_recoder(live_video_stream_recorder_t *rec)
{
  rec->state = LIVE_VIDEO_RUNING;
  return video_rec_start(rec->recorder);
}

int live_video_reset(int channel)
{
	int i = 0;
	int encoder_ret = 0;
	int ret = 0;
	i = channel;
	if (channel < live_video_get_recoder_channels() ){
		live_video_state_t old_state;
		old_state =recoders[i].state;
		if (old_state == LIVE_VIDEO_RUNING){
			ret = video_rec_stop(recoders[i].recorder);

			live_video_logd("video_rec_stop %s\n",
				(!ret)?"ok":"error");

		}



		live_video_release_stream_recorder(&recoders[i]);

	    encoder_ret = live_video_setup_recoder(&recoders[i], NULL, live_video_enc_sec_stream_buf_handle);
	    live_video_loge("live_video_setup_recoder %s\n",
		    (!encoder_ret)?"ok":"error");
		ret |= encoder_ret;

		if (old_state == LIVE_VIDEO_RUNING){
			encoder_ret = live_video_start_recoder(&recoders[i]);
			live_video_loge("live_video_start_recoder %s\n",
				(!encoder_ret)?"ok":"error");
		}


		ret |= encoder_ret;

	}else{
		live_video_loge("para error: channel=%d max=%d",
			channel,
			live_video_get_recoder_channels());

	}
	return ret;
}

int live_video_get_recoder_channels(void)
{
	return (sizeof(recoders)/sizeof(recoders[0]));
}



int live_video_get_data_from_buf(int channel, unsigned char *buffer, unsigned int *size)
{

	frame_dsc_s FrameDsc;
	FrameDsc.pbyData = buffer;

	if (SYS_OK == frame_queue_get_node_frame(channel, &FrameDsc,  FRAME_QUEUE_NO_WAIT)){
	    *size = FrameDsc.dwDataLen;
	    /*live_video_logd("get %p size=%d ok", buffer, *size);*/


	}
	return 0;
}



/**
 * live_video_init - Initialize the live video stream.
 * @rgb_hdl Video stream handle(Note: The value can be NULL).
 *
 * Returns 0  if success, other if failure.
 */
int live_video_init(void *rgb_hdl)
{
	int rtsp_ret = -1;
	int encoder_ret = -1;
	int i = 0;
	//video_hdl = 0;

	frame_queue_init();

	(void)rgb_hdl;
	/*TODO make compiler happy*/
	live_video_set_as_config(0, 0);

	for (i = 0; i < sizeof(recoders)/sizeof(recoders[0]); i++){

	    encoder_ret = live_video_setup_recoder(&recoders[i], NULL, live_video_enc_sec_stream_buf_handle);

	    live_video_loge("live_video_setup_recoder %s\n",
		    (!encoder_ret)?"ok":"error");
	}

	for (i = 0; i < sizeof(recoders)/sizeof(recoders[0]); i++){
	    encoder_ret = live_video_start_recoder(&recoders[i]);
	    live_video_loge("live_video_start_recoder %s\n",
		    (!encoder_ret)?"ok":"error");
	}

	{
	      /* Initialize the live555 rtsp service */
	      rtsp_ret = rtsp_server_init();
	      live_video_loge("Start rtsp server %s\n",
		      (!rtsp_ret)?"ok":"error");
	      return rtsp_ret;
	}

	return 0;
}

int live_video_deinit(void)
{
  int i = 0;
  rtsp_server_deinit();

  for (i = 0; i < sizeof(recoders)/sizeof(recoders[0]); i++){
      live_video_release_stream_recorder(&recoders[i]);
      recoders[i].state = LIVE_VIDEO_DEINIT;
  }

  return 0;
}


int live_video_pause(int channel)
{
  int i = 0;
  int ret = 0;
  i = channel;

  if (channel < live_video_get_recoder_channels() ){

  	live_video_logd("channel=%d recorder=%p",
		 channel,
		 recoders[i].recorder);
	if (recoders[i].state == LIVE_VIDEO_RUNING){
      ret |= video_rec_stop(recoders[i].recorder);
      recoders[i].state = LIVE_VIDEO_PAUSE;
	}


  }else{
  	live_video_loge("para error: channel=%d max=%d",
		channel,
		live_video_get_recoder_channels());
	ret = -1;
  }
  return ret;
}


int live_video_resume(int channel)
{
  int i = 0;
  int ret = 0;
  i = channel;
  if (channel < live_video_get_recoder_channels() ){

	  live_video_logd("channel=%d recorder=%p",
		   channel,
		   recoders[i].recorder);

	if (recoders[i].state == LIVE_VIDEO_PAUSE){
      ret |= video_rec_start(recoders[i].recorder);
      recoders[i].state = LIVE_VIDEO_RUNING;
	}

  }else{
  	live_video_loge("para error: channel=%d max=%d",
		channel,
		live_video_get_recoder_channels());
	ret = -1;
  }
  return ret;
}

live_video_state_t live_get_video_state(int channel)
{
  live_video_state_t ret = 0;
  if (channel < live_video_get_recoder_channels() ){
	  ret = recoders[channel].state;
  }else{
  	live_video_loge("para error: channel=%d max=%d",
		channel,
		live_video_get_recoder_channels());
	ret = -1;
  }
  return ret;
}

