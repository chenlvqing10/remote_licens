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

#define LOG_TAG "LIVE_VIDEO"
#include <log/log.h>

#include "elang.h"
#include "video_stream.h"
#include "audio_stream.h"
#include "video_recorder.h"
#include "live_video.h"
#include "live_buffer.h"
#include "yuv_retangle_draw.h"
#include "file_ops.h"
#include "watermark_api.h"
#include "hotplug.h"
#include "config.h"

/* h264 enc main stream */
#define ENC_MAIN_STREAM_WIDTH      1920
#define ENC_MAIN_STREAM_HEIGHT     1080
/* h264 enc sec stream */
#define ENC_SEC_STREAM_WIDTH       1280
#define ENC_SEC_STREAM_HEIGHT      720

/* isp main channel, 1080P */
#define LIVE_VIDEO_WIDTH_DEFAULT           1920
#define LIVE_VIDEO_HEIGHT_DEFAULT          1080

#define LIVE_VIDEO_ROT       0
#define MAX_FACE_CNT         10
#define AV_STREAM_CALLBACK
#define INPUT_MODE           ENC_YUV420SP

typedef struct {
	int main_stream_enable;
	const char *video_dev;
	int video_width;
	int video_height;
	int face_video_width;
	int face_video_height;
	int main_recorder_save_file;
	int record_duration;
} live_video_param;

live_video_param g_param;

typedef struct _stream_recorder {
	void *astream;
	void *vstream;
	void *recorder;
} stream_recorder_t;

typedef struct recorder_para {
	int index;
	int watermark;
	int stop_flag;
	int preview_flag;
	int record_flag;
	rec_param_t rec_para;
	char *name_prefix;
	char *path;
	stream_recorder_t *recorder;
} recorder_para_t;

typedef struct stream_recorder_cb {
	void *disp;
	recorder_para_t param;
	struct sfifo_des_s *my_sfifo_des;
	sem_t sem;
} stream_recorder_cb_t;

static faces_t face_info;
static int video_max_len = 0;
static stream_recorder_t *g_main_recorder;
static stream_recorder_t *g_sec_recorder;
static pthread_mutex_t face_lock;

static int (*blending_handle)(face_t *face, frame_t *pframe);
static void *watermark_func(void *recoder);
static pthread_t g_main_wartermark_thread;
static pthread_t g_sec_wartermark_thread;

stream_recorder_cb_t g_sec_stream_cb = {
	.disp = NULL,
	.param = {
		.index = 1,
		.rec_para = {
			.audio_channels = 2,
			.audio_sample_width = 16,
			.audio_sample_rate = 48000,
#ifdef AV_STREAM_CALLBACK
			.aenc_format = AUDIO_ENCODER_AAC,	/* PCM */
			.file_fmt = REC_OUTPUT_FORMAT_RAW,	/* H264 RAW */
#else
			.aenc_format = AUDIO_ENCODER_AAC,	/* AAC */
			.file_fmt = REC_OUTPUT_FORMAT_MP4,	/* MP4 */
#endif
			.venc_format = VIDEO_ENCODER_H264,

			.bitrate = 3200000,
			.source_width = LIVE_VIDEO_WIDTH_DEFAULT,
			.source_height = LIVE_VIDEO_HEIGHT_DEFAULT,
			.width = ENC_SEC_STREAM_WIDTH,
			.height = ENC_SEC_STREAM_HEIGHT,
			.rotate = VDISP_ROTATE_NONE,
			.enc_rot = VDISP_ROTATE_NONE,
		},
		.name_prefix = "isp",
	},
};

stream_recorder_cb_t g_main_stream_cb = {
	.disp = NULL,
	.param = {
		.index = 1,
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

			.bitrate = 3200000,
			.source_width = LIVE_VIDEO_WIDTH_DEFAULT,
			.source_height = LIVE_VIDEO_HEIGHT_DEFAULT,
			.width = ENC_MAIN_STREAM_WIDTH,
			.height = ENC_MAIN_STREAM_HEIGHT,
			.rotate = VDISP_ROTATE_NONE,
			.enc_rot = VDISP_ROTATE_NONE,
		},
		.name_prefix = "isp",
	},
};

static int enc_main_stream_buf_handle(void *hdl, avstream_t *stream)
{
	stream_recorder_cb_t *cb = (stream_recorder_cb_t *)hdl;

	if (STREAM_TYPE_VIDEO == stream->type) {
		if (video_max_len < stream->data_size){
			video_max_len = stream->data_size;
			ALOGD("%s video_max_len=%d\n", __func__, video_max_len);
		}
	}

	/* ALOGD("%s\n", __func__); */

	sem_post(&cb->sem);

	return 0;
}

static int enc_sec_stream_buf_handle(void *hdl, avstream_t *stream)
{
	stream_recorder_cb_t *cb = (stream_recorder_cb_t *)hdl;

	if (STREAM_TYPE_VIDEO == stream->type) {
		if (video_max_len < stream->data_size){
			video_max_len = stream->data_size;
			ALOGD("%s video_max_len=%d\n", __func__, video_max_len);
		}

		save_data_to_buf(stream->data, stream->data_size, STREAM_TYPE_VIDEO);
	}
#ifdef RTSP_AUDIO
	else if (STREAM_TYPE_AUDIO == stream->type)
		save_data_to_buf(stream->data, stream->data_size, STREAM_TYPE_AUDIO);
#endif
	sem_post(&cb->sem);

	return 0;
}

/**
 * create_recorder - get a recorder handle.
 * This function will load all components recorder needs.
 *
 * Returns recorder handle on success, NULL otherwise..
 */
static void *create_stream_recorder(void *rgb_hdl)
{
	stream_recorder_t *hdl = malloc(sizeof(stream_recorder_t));
	if (hdl == NULL)
		return NULL;

	memset(hdl, 0, sizeof(stream_recorder_t));
	hdl->vstream = rgb_hdl;
	hdl->astream = audio_stream_creat();
	hdl->recorder = video_rec_creat();
	video_rec_set_video_stream(hdl->recorder, hdl->vstream);
	video_rec_set_audio_stream(hdl->recorder, hdl->astream);

	return hdl;
}

/**
 * release_stream_recorder - release resource recorder used.
 *
 * @hdl: recorder handle
 *
 * Returns 0 on success, -EERROR otherwise..
 */
static void release_stream_recorder(stream_recorder_t *recorder)
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

static int recorder_set_para(stream_recorder_t *recorder, rec_param_t *para)
{
	enc_para_t enc_para;
	vsrc_param_t vsrc_para;
	int ret;

	vsrc_para.frame_rate = para->frame_rate;
	vsrc_para.height = para->source_height;
	vsrc_para.width = para->source_width;
	ret = video_stream_set_src_para(recorder->vstream, &vsrc_para);
	if (ret != 0)
		return -1;

	enc_para.bitrate = para->bitrate;
	enc_para.width = para->width;
	enc_para.height = para->height;
	enc_para.fmt = para->file_fmt;
	enc_para.rotate = para->enc_rot;
	ret = video_rec_set_para(recorder->recorder, &enc_para);
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

void live_video_face_cb(faces_t *faces)
{
	int i = 0;

	pthread_mutex_lock(&face_lock);
	if (faces != NULL && faces->face_num > 0) {
		face_info.face_num = faces->face_num;
		for (i = 0; i < faces->face_num; i++)
			memcpy(&face_info.faces[i], &faces->faces[i], sizeof(face_t));
	} else {
		if (faces != NULL)
			face_info.face_num = faces->face_num;

	}
	pthread_mutex_unlock(&face_lock);
}

static int video_stream_buf_handle(void *app_data, frame_t *pframe)
{
	int i;

	if (pframe == NULL || pframe->info.video.addr[0] == NULL)
		return -1;

	pthread_mutex_lock(&face_lock);
	if (face_info.face_num > 0) {
		for (i = 0; i < face_info.face_num; i++) {

			float threshold = face_recg_threshold();
			if (face_info.faces[i].score > threshold)
				watermark_face_retangle_draw_by_color(&face_info.faces[i],
					pframe, g_param.face_video_width,
					g_param.face_video_height,
					INPUT_MODE, 0x00FF00);
			else
				watermark_face_retangle_draw(&face_info.faces[i], pframe,
					g_param.face_video_width,
					g_param.face_video_height,
					INPUT_MODE);

			if (blending_handle != NULL)
				blending_handle(&face_info.faces[i], pframe);
		}
	}
	pthread_mutex_unlock(&face_lock);
	return 0;
}

static void *live_video_start(const char *source, int width, int height, int rot)
{
	void *hdl;
	int ret;
	app_port_filter_t cb;
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
		ALOGE("stream create failed\n");
		return NULL;
	}

	ret = video_stream_set_camera_source(hdl, (char *)source);
	if (ret) {
		ALOGE("video_stream_set_camera_source failed\n");
		return NULL;
	}

	cb.buf_handle = video_stream_buf_handle;
	cb.app_data = NULL;
	cb.type = CB_VCAM_OUT;
	ret = video_stream_set_port_cb(hdl, &cb);
	if (ret) {
		ALOGE("video_stream_set_port_cb failed\n");
		return NULL;
	}

	ret = video_stream_set_camera_size(hdl, width, height);
	if (ret) {
		ALOGE("video_stream_set_camera_size failed\n");
		return NULL;
	}

	ret = video_stream_set_rotate(hdl, rotation);
	if (ret) {
		ALOGE("video_stream_set_rotate failed\n");
		return NULL;
	}

	ret = video_stream_start_camera(hdl);
	if (ret) {
		ALOGE("video_stream_start_camera failed\n");
		return NULL;
	}

	return hdl;
}

#define MAX_RECORD_FIELS 5
static char record_last_file[MAX_RECORD_FIELS][128];
static int record_last_file_index;

static void generate_filename(char *buf, size_t buflen, const char *prefix)
{
	time_t timep;
	struct tm *p;

	time(&timep);
	p = localtime(&timep);

	snprintf(buf, buflen, "%s_%d%02d%02d_%02d%02d%02d.mp4", prefix,
			p->tm_year + 1900, p->tm_mon + 1, p->tm_mday, p->tm_hour,
			p->tm_min, p->tm_sec);
}

int cb_get_next_file(void *hdl, char *next_file)
{
	char filename[128];
	generate_filename(filename, 128, "/mnt/usb/main_encoder");

	strcpy(next_file, filename);
	return 0;
}

int cb_file_closed(void *hdl, char *file_name)
{
	file_remove(
		record_last_file[(record_last_file_index + 1) % MAX_RECORD_FIELS]);

	strcpy(record_last_file[record_last_file_index], file_name);
	record_last_file_index++;
	if (record_last_file_index >= MAX_RECORD_FIELS)
		record_last_file_index = 0;
	return 0;
}

/* video enc for tf card transfer */

static int start_main_encoder(void *rgb_hdl)
{
	void *live_video_hdl;
	vsize_t size;
	recorder_para_t *para = &g_main_stream_cb.param;

	ALOGD("%s start\n", __func__);
	memset(&size, 0, sizeof(size));

	sem_init(&g_main_stream_cb.sem, 0, 0);

	/* use face service's rgb hdl for video encoder */
	if (rgb_hdl != NULL) {
		ALOGE("%s use face service's rgb hdl for video encoder\n", __func__);
		/* create rgb cam's recorder object */
		g_main_recorder = create_stream_recorder(rgb_hdl);
		if (g_main_recorder == NULL) {
			ALOGE("create g_main_recorder failed!\n");
			return -1;
		}
	} else {
		ALOGE("%s use isp main channel 1080P for video encoder\n", __func__);
		/* isp main channel, 1080P */
		live_video_hdl = live_video_start(g_param.video_dev,
						  g_param.video_width,
						  g_param.video_height,
						  LIVE_VIDEO_ROT);
		if (!live_video_hdl) {
			ALOGE("live_video_start failed\n");
			return -1;
		}

		/* create rgb cam's recorder object */
		g_main_recorder = create_stream_recorder(live_video_hdl);
		if (g_main_recorder == NULL) {
			ALOGE("create_stream_recorder failed!\n");
			return -1;
		}
	}

	para->recorder = g_main_recorder;

	/* set video/audio parameter */
	video_stream_set_camera_source(g_main_recorder->vstream,
			(char *) g_param.video_dev);
	recorder_set_para(g_main_recorder, &para->rec_para);

	if (g_param.main_recorder_save_file == 0) {
		/* set video/audio callback parameter */
		app_stream_cb_t cb;
		cb.app_data = &g_main_stream_cb;
		cb.buf_handle = enc_main_stream_buf_handle;
		video_rec_set_stream_cb(g_main_recorder->recorder, &cb);
		video_rec_start(g_main_recorder->recorder);
	} else {
		char filename[128];
		generate_filename(filename, 128, "/mnt/usb/main_encoder");
		video_rec_set_output_file(g_main_recorder->recorder, filename);
		video_rec_set_output_format(g_main_recorder->recorder,
				REC_OUTPUT_FORMAT_MP4);
		audio_stream_set_coding_type(g_main_recorder->astream,
				AUDIO_ENCODER_AAC);

		fix_duration_param_t fix_duration_param;
		fix_duration_param.file_duration = g_param.record_duration;
		fix_duration_param.cb_get_next_file = cb_get_next_file;
		fix_duration_param.cb_file_closed = cb_file_closed;
		record_last_file_index = 0;

		video_rec_set_fix_duration_param(g_main_recorder->recorder,
			g_main_recorder->recorder, &fix_duration_param);

		if (udisk_is_ok() == 0) {
			audio_stream_start(g_main_recorder->astream);
			video_rec_start(g_main_recorder->recorder);
		}
	}

	if (g_main_recorder != NULL && g_main_recorder->recorder != NULL) {
		watermark_init_source(g_main_recorder->recorder);
		pthread_create(&g_main_wartermark_thread, NULL, watermark_func,
				g_main_recorder->recorder);
	}
	return 0;
}

/* video enc for RTSP/RTP transfer */
static int start_sec_encoder(void *rgb_hdl)
{
	void *live_video_hdl;
	vsize_t size;
	recorder_para_t *para = &g_sec_stream_cb.param;

	ALOGD("%s start\n", __func__);
	memset(&size, 0, sizeof(size));

	/* Initialization of buffer for storing real-time audio and video data */
	g_sec_stream_cb.my_sfifo_des = video_buffer_init();
	if (g_sec_stream_cb.my_sfifo_des == NULL) {
		ALOGE("video_buffer_init failed!\n");
		return -1;
	}

	sem_init(&g_sec_stream_cb.sem, 0, 0);

	/* use face service's rgb hdl for video encoder */
	if (rgb_hdl != NULL) {
		ALOGE("%s use face service's rgb hdl for video encoder\n", __func__);
		/* create rgb cam's recorder object */
		g_sec_recorder = create_stream_recorder(rgb_hdl);
		if (g_sec_recorder == NULL) {
			ALOGE("create g_sec_recorder failed!\n");
			return -1;
		}
	} else {
		ALOGE("%s use isp main channel 1080P for video encoder\n", __func__);
		/* isp main channel, 1080P */
		live_video_hdl = live_video_start(g_param.video_dev,
			g_param.video_width, g_param.video_height,
			LIVE_VIDEO_ROT);
		if (!live_video_hdl) {
			ALOGE("live_video_start failed\n");
			return -1;
		}

		/* create rgb cam's recorder object */
		g_sec_recorder = create_stream_recorder(live_video_hdl);
		if (g_sec_recorder == NULL) {
			ALOGE("create_stream_recorder failed!\n");
			return -1;
		}
	}

	para->recorder = g_sec_recorder;

	/* set video/audio parameter */
	video_stream_set_camera_source(g_sec_recorder->vstream,
			(char *) g_param.video_dev);
	recorder_set_para(g_sec_recorder, &para->rec_para);

	/* set video/audio callback parameter */
	app_stream_cb_t cb;
	cb.app_data = &g_sec_stream_cb;
	cb.buf_handle = enc_sec_stream_buf_handle;

	video_rec_set_stream_cb(g_sec_recorder->recorder, &cb);
	audio_stream_start(g_sec_recorder->astream);
	video_rec_start(g_sec_recorder->recorder);

	if (g_sec_recorder != NULL && g_sec_recorder->recorder != NULL) {
		watermark_init_source(g_sec_recorder->recorder);
		pthread_create(&g_sec_wartermark_thread, NULL, watermark_func,
				g_sec_recorder->recorder);
	}

	return 0;
}

static void *watermark_func(void *recoder)
{
	while (1) {
		time_t time_ptr;
		time(&time_ptr);

		struct tm *tmp_ptr = localtime(&time_ptr);
		watermark_set_time_h_m_s(recoder,
				tmp_ptr->tm_hour * 10000 + tmp_ptr->tm_min * 100
						+ tmp_ptr->tm_sec, 16, 16);

		usleep(1000000);
	}

	return NULL;
}

static void live_video_init_param()
{
	g_param.main_stream_enable = config_get_int("live_video",
			"main_stream_enable", 1);
	g_param.video_dev = config_get_string("live_video", "video_dev",
			"/dev/video3");
	g_param.video_width = config_get_int("live_video", "video_width",
			LIVE_VIDEO_WIDTH_DEFAULT);
	g_param.video_height = config_get_int("live_video", "video_height",
			LIVE_VIDEO_HEIGHT_DEFAULT);
	g_param.face_video_width = config_get_int("live_video",
			"face_video_width", 1280);
	g_param.face_video_height = config_get_int("live_video",
			"face_video_height", 720);

	if (g_param.main_stream_enable) {
		g_param.main_recorder_save_file = config_get_int("live_video",
				"main_recorder_save_file", 1);
		g_param.record_duration = config_get_int("live_video",
				"record_duration", 300);

		g_main_stream_cb.param.rec_para.source_width = g_param.video_width;
		g_main_stream_cb.param.rec_para.source_height = g_param.video_height;
	} else
		g_param.main_recorder_save_file = 0;

	g_sec_stream_cb.param.rec_para.source_width = g_param.video_width;
	g_sec_stream_cb.param.rec_para.source_height = g_param.video_height;
}

int live_video_init(void *rgb_hdl)
{
	live_video_init_param();

	if (pthread_mutex_init(&face_lock, NULL)) {
		ALOGE("init face_lock failed");
		return -1;
	}

	/* max face num is 5 */
	face_info.faces = (face_t *)malloc(sizeof(face_t) * MAX_FACE_CNT);
	if (g_param.main_stream_enable)
		start_main_encoder(rgb_hdl);

	start_sec_encoder(rgb_hdl);

	return 0;
}

static int stop_main_encoder(void)
{
	pthread_cancel(g_main_wartermark_thread);
	pthread_join(g_main_wartermark_thread, NULL);

	/* stop video/audio */
	audio_stream_stop(g_main_recorder->astream);
	video_rec_stop(g_main_recorder->recorder);
	video_stream_stop_camera(g_main_recorder->vstream);
	release_stream_recorder(g_main_recorder);

	sem_post(&g_main_stream_cb.sem);
	sem_destroy(&g_main_stream_cb.sem);

	return 0;
}

static int stop_sec_encoder(void)
{
	pthread_cancel(g_sec_wartermark_thread);
	pthread_join(g_sec_wartermark_thread, NULL);

	/* stop video/audio */
	audio_stream_stop(g_sec_recorder->astream);
	video_rec_stop(g_sec_recorder->recorder);
	video_stream_stop_camera(g_sec_recorder->vstream);
	release_stream_recorder(g_sec_recorder);

	sem_post(&g_sec_stream_cb.sem);

	video_buffer_deinit();

	sem_destroy(&g_sec_stream_cb.sem);

	return 0;
}

int live_video_deinit(void)
{
	if (g_param.main_stream_enable)
		stop_main_encoder();

	stop_sec_encoder();

	free(face_info.faces);

	return 0;
}

void live_video_set_blending_callback(void *handle)
{
	blending_handle = handle;
}

int live_video_start_main_rec()
{
	if (g_param.main_stream_enable) {
		audio_stream_start(g_main_recorder->astream);
		return video_rec_start(g_main_recorder->recorder);
	} else
		return 0;
}

int live_video_stop_main_rec()
{
	if (g_param.main_stream_enable) {
		audio_stream_stop(g_main_recorder->astream);
		return video_rec_stop(g_main_recorder->recorder);
	} else
		return 0;

}
