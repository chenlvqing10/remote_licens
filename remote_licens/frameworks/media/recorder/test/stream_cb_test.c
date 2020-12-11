/*
 * lb_recorder.c - Standard functionality for recorder.
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

#define DBG_LEVEL		DBG_VERBOSE
#define LOG_TAG			"stream_cb_test"

#include <signal.h>
#include <errno.h>
#include <oscl.h>
#include <base_component.h>
#include "omx_test.h"
#include "vrender_component.h"
#include "vrec_component.h"
#include "omx_api.h"
#include "lb_recorder.h"
#include <getopt.h>
#include "recorder_private.h"
#include "dirent.h"
#include "lb_omx_core.h"


#define OUT_FMT_MP4          0
#define OUT_FMT_TS           1
#define OUT_FMT_CALLBACK     2

#define AV_STREAM_OUT_FMT    OUT_FMT_MP4

#ifdef STREAM_CB_MJPEG
#undef AV_STREAM_OUT_FMT
#define AV_STREAM_OUT_FMT    OUT_FMT_CALLBACK
#endif
#if (AV_STREAM_OUT_FMT == OUT_FMT_TS)
#define OUTPUT_FILE_NAME     "/mnt/nfs/t_v.ts"
#else
#define OUTPUT_FILE_NAME     "/mnt/nfs/t_v.mp4"
#endif
typedef struct queue_elem_t {
	void *data;
	struct queue_elem_t *next;
} queue_elem_t;

typedef struct queue_t {
	queue_elem_t *head, *tail;
	pthread_mutex_t lock;
	int count;
} queue_t;

typedef struct stream_cb_test {
	void *disp;
	auto_test_para_t param;
	queue_t *queue;
	sem_t sem;
	int proc_thd_exit;
} stream_cb_test_t;

stream_cb_test_t g_stream_cb = {
	.disp = NULL,
	.param = {
		.index = 1,
		.source = FRONT_INPUT_DEVICE_NAME,
		.rec_para = {
			.audio_channels = 2,
			.audio_sample_width = 16,
			.audio_sample_rate = 48000,
#if (AV_STREAM_OUT_FMT == OUT_FMT_CALLBACK)
			.aenc_format = AUDIO_ENCODER_PCM,
			.file_fmt = REC_OUTPUT_FORMAT_RAW,
#elif (AV_STREAM_OUT_FMT == OUT_FMT_TS)
			.aenc_format = AUDIO_ENCODER_AAC_MP4ADTS,
			.file_fmt = REC_OUTPUT_FORMAT_TS,
#else
			.aenc_format = AUDIO_ENCODER_AAC,
			.file_fmt = REC_OUTPUT_FORMAT_MP4,
#endif
			.venc_format = VIDEO_ENCODER_H264,
			.bitrate = 14000000,
			.source_width = 1920,
			.source_height = 1080,
			.width = 1080,
			.height = 1920,
			.rotate = VDISP_ROTATE_90,
			.enc_rot = VDISP_ROTATE_90,
		},
		.name_prefix = "isp",
	},
};

static int g_test_end_flag = 1;

static void signal_handler(int sig)
{
	OSCL_LOGI("test end");
	g_test_end_flag = 0;
}

#if (AV_STREAM_OUT_FMT == OUT_FMT_CALLBACK)

#define ID_RIFF 0x46464952
#define ID_WAVE 0x45564157
#define ID_FMT  0x20746d66
#define ID_DATA 0x61746164
#define FORMAT_PCM 1

typedef struct wav_header {
	uint32_t riff_id;
	uint32_t riff_sz;
	uint32_t riff_fmt;
	uint32_t fmt_id;
	uint32_t fmt_sz;
	uint16_t audio_format;
	uint16_t num_channels;
	uint32_t sample_rate;
	uint32_t byte_rate;
	uint16_t block_align;
	uint16_t bits_per_sample;
	uint32_t data_id;
	uint32_t data_sz;
} wav_header_t;

static void set_wav_header(wav_header_t *header, int ch, int bits, int rate)
{
	header->riff_id = ID_RIFF;
	header->riff_sz = 0;
	header->riff_fmt = ID_WAVE;
	header->fmt_id = ID_FMT;
	header->fmt_sz = 16;
	header->audio_format = FORMAT_PCM;
	header->num_channels = ch;
	header->sample_rate = rate;
	header->bits_per_sample = bits;
	header->byte_rate = (header->bits_per_sample / 8) * ch * rate;
	header->block_align = ch * (header->bits_per_sample / 8);
	header->data_id = ID_DATA;
}

static int queue_push(queue_t *queue, void *data)
{
	queue_elem_t *elem;

	if (queue == NULL)
		return -1;
	elem = malloc(sizeof(queue_elem_t));
	if (elem == NULL)
		return -1;
	elem->data = data;
	elem->next = NULL;
	pthread_mutex_lock(&queue->lock);
	if (queue->tail == NULL) {
		queue->head = queue->tail = elem;
	} else {
		queue->tail->next = elem;
		queue->tail = elem;
	}
	queue->count++;
	pthread_mutex_unlock(&queue->lock);
	return 0;
}

static void *queue_pop(queue_t *queue)
{
	void *data = NULL;
	queue_elem_t *elem;

	if (queue == NULL)
		return NULL;

	pthread_mutex_lock(&queue->lock);
	if (queue->head == NULL) {
		pthread_mutex_unlock(&queue->lock);
		return NULL;
	}
	elem = queue->head;
	queue->head = elem->next;
	if (queue->head == NULL)
		queue->tail = NULL;
	queue->count--;
	pthread_mutex_unlock(&queue->lock);
	data = elem->data;
	free(elem);

	return data;
}

static queue_t *queue_init(void)
{
	queue_t *queue = malloc(sizeof(queue_t));
	if (queue == NULL)
		return NULL;
	queue->head = queue->tail = NULL;
	queue->count = 0;
	pthread_mutex_init(&queue->lock, NULL);
	return queue;
}

static void queue_deinit(queue_t *queue)
{
	if (queue == NULL)
		return;
	pthread_mutex_destroy(&queue->lock);
	free(queue);
}

static int stream_buf_handle(void *hdl, avstream_t *stream)
{
	stream_cb_test_t *cb = (stream_cb_test_t *)hdl;
	avstream_t *s = malloc(sizeof(avstream_t));

	if (s == NULL)
		return -1;

	*s = *stream;
	s->data = malloc(s->data_size);
	if (s->data == NULL) {
		free(s);
		return -1;
	}
	memcpy(s->data, stream->data, s->data_size);
	queue_push(cb->queue, s);
	sem_post(&cb->sem);
	return 0;
}

#define RAW_H264_PATH	"/mnt/usb/raw.h264"
#define RAW_WAV_PATH	"/mnt/usb/raw.wav"

static void *data_proc_thread(void *arg)
{
	stream_cb_test_t *cb = (stream_cb_test_t *)arg;
	int a_fd, v_fd;
	int pcm_size = 0;

	prctl(PR_SET_NAME, "stream_cb_test_thread");
	v_fd = open(RAW_H264_PATH, O_RDWR|O_CREAT|O_TRUNC, 0666);
	if (v_fd < 0)
		OSCL_LOGE("open file error(%s)!", strerror(errno));

	a_fd = open(RAW_WAV_PATH, O_RDWR|O_CREAT|O_TRUNC, 0666);
	if (a_fd < 0)
		OSCL_LOGE("open file error(%s)!", strerror(errno));
	else
		lseek(a_fd, sizeof(wav_header_t), SEEK_SET);

	while (!cb->proc_thd_exit) {
		sem_wait(&cb->sem);
		avstream_t *stream = queue_pop(cb->queue);
		if (stream == NULL)
			continue;
#if 0
		uint8_t *p = stream->data;
		OSCL_LOGI("%s: size %d, ts %lld, data(%02x %02x %02x %02x %02x %02x)",
			(stream->type == STREAM_TYPE_VIDEO) ? "video" : "audio",
			stream->data_size, stream->timestamp,
			p[0], p[1], p[2], p[3], p[4], p[5]);
#endif
		if (v_fd >= 0 && stream->type == STREAM_TYPE_VIDEO) {
			int ret;
			ret = write(v_fd, stream->data, stream->data_size);
			if (ret != stream->data_size) {
				OSCL_LOGE("write data error(%d!=%d, %s)!",
					ret, stream->data_size, strerror(errno));
			}
		}
		if (a_fd >= 0 && stream->type == STREAM_TYPE_AUDIO) {
			int ret;
			ret = write(a_fd, stream->data, stream->data_size);
			if (ret != stream->data_size) {
				OSCL_LOGE("write data error(%d!=%d, %s)!",
					ret, stream->data_size, strerror(errno));
			}
			pcm_size += stream->data_size;
		}
		free(stream->data);
		free(stream);
	}

	if (v_fd >= 0)
		close(v_fd);

	if (a_fd >= 0) {
		wav_header_t header;
		set_wav_header(&header, cb->param.rec_para.audio_channels,
			cb->param.rec_para.audio_sample_width,
			cb->param.rec_para.audio_sample_rate);
		header.data_sz = pcm_size;
		header.riff_sz = sizeof(wav_header_t) + pcm_size - 8;
		lseek(a_fd, 0, SEEK_SET);
		write(a_fd, &header, sizeof(wav_header_t));
		close(a_fd);
	}

	return NULL;
}
#endif

#ifndef STREAM_CB_MJPEG
int main(int argc, char *argv[])
{
	disp_size_t disp_size;
	vsize_t size;
	void *recorder;
	auto_test_para_t *para = &g_stream_cb.param;
	int ret;

	OSCL_LOGI("start");
	memset(&size, 0, sizeof(size));

#if (AV_STREAM_OUT_FMT == OUT_FMT_CALLBACK)
	pthread_t thread;
	g_stream_cb.queue = queue_init();
	if (g_stream_cb.queue == NULL) {
		OSCL_LOGE("queue_init failed!");
		return -1;
	}
	sem_init(&g_stream_cb.sem, 0, 0);
	g_stream_cb.proc_thd_exit = 0;
	pthread_create(&thread, NULL, data_proc_thread, &g_stream_cb);
#endif

	g_stream_cb.disp = oscl_open_disp_engine();
	if (g_stream_cb.disp == NULL) {
		OSCL_LOGE("oscl_open_disp_engine failed!");
		return -1;
	}
	ret = oscl_disp_get_screen_size(g_stream_cb.disp, &disp_size);
	if (ret != 0) {
		OSCL_LOGE("oscl_disp_get_screen_size failed(ret=%d)!", ret);
		return -1;
	}

	para->disp_para.rect.x = 0;
	para->disp_para.rect.y = 0;
	para->disp_para.mode = VDISP_WINDOW_FULL_SCREEN_VIDEO_RATIO;
	para->disp_para.rect.width = disp_size.width;
	para->disp_para.rect.height = disp_size.height;
	OSCL_LOGI("disp_para (%d %d %d %d)!",
		para->disp_para.rect.x,
		para->disp_para.rect.y,
		para->disp_para.rect.width,
		para->disp_para.rect.height);

	recorder = lb_recorder_creat();
	if (recorder == NULL) {
		OSCL_LOGE("lb_recorder_creat failed!");
		return -1;
	}
	para->recorder = recorder;

	lb_recorder_ctrl(recorder, LB_REC_SET_VIDEO_SOURCE, para->source);
	lb_recorder_ctrl(recorder, LB_REC_SET_PARA, &para->rec_para);
	lb_recorder_ctrl(recorder, LB_REC_SET_DISP_PARA, &para->disp_para);
#if (AV_STREAM_OUT_FMT == OUT_FMT_CALLBACK)
	app_stream_cb_t cb;
	cb.app_data = &g_stream_cb;
	cb.buf_handle = stream_buf_handle;
	lb_recorder_ctrl(recorder, LB_REC_SET_STREAM_CB, &cb);
#else
	lb_recorder_ctrl(recorder, LB_REC_SET_OUTPUT_FILE, OUTPUT_FILE_NAME);
#endif
	lb_recorder_ctrl(recorder, LB_REC_PREPARE, 0);
	lb_recorder_ctrl(recorder, LB_REC_PREVIEW, 0);
	lb_recorder_ctrl(recorder, LB_REC_START, 0);

	g_test_end_flag = 1;
	signal(SIGINT, signal_handler);

	while (g_test_end_flag)
		oscl_mdelay(1000);

	lb_recorder_ctrl(recorder, LB_REC_STOP_PREVIEW, 0);
	lb_recorder_ctrl(recorder, LB_REC_STOP, 0);
	lb_recorder_release(recorder);
	oscl_close_disp_engine(g_stream_cb.disp);

#if (AV_STREAM_OUT_FMT == OUT_FMT_CALLBACK)
	g_stream_cb.proc_thd_exit = 1;
	sem_post(&g_stream_cb.sem);
	pthread_join(thread, NULL);

	avstream_t *s;
	while ((s = queue_pop(g_stream_cb.queue)) != NULL) {
		OSCL_LOGW("queue count %d!", g_stream_cb.queue->count);
		free(s->data);
		free(s);
	}
	queue_deinit(g_stream_cb.queue);
	sem_destroy(&g_stream_cb.sem);
#endif

	OSCL_LOGI("finish");

	return 0;
}

#else
int main(int argc, char *argv[])
{
	void *vstream;
	void *video_rec;
	int ret;
	vsrc_param_t src_para;
	enc_para_t enc_para;

#if (AV_STREAM_OUT_FMT == OUT_FMT_CALLBACK)
	app_stream_cb_t cb;
	pthread_t thread;
	g_stream_cb.queue = queue_init();
	if (g_stream_cb.queue == NULL) {
		OSCL_LOGE("queue_init failed!");
		return -1;
	}
	sem_init(&g_stream_cb.sem, 0, 0);
	g_stream_cb.proc_thd_exit = 0;
	pthread_create(&thread, NULL, data_proc_thread, &g_stream_cb);
#endif
	OSCL_LOGI("start");

	vstream = video_stream_create();
	if (!vstream) {
		printf("stream create failed\n");
		return -1;
	}
	video_rec = video_rec_creat();
	if (!video_rec) {
		printf("video_rec create failed\n");
		return -1;
	}
	video_rec_set_video_stream(video_rec, vstream);

	ret = video_stream_set_camera_source(vstream, FRONT_INPUT_DEVICE_NAME);
	if (ret) {
		printf("video_stream_set_camera_source failed\n");
		return -1;
	}

	memset(&src_para, 0, sizeof(src_para));
	src_para.width = 1920;
	src_para.height = 1080;
	ret = video_stream_set_src_para(vstream, &src_para);
	if (ret) {
		printf("video_stream_set_camera_size failed\n");
		return -1;
	}

	memset(&enc_para, 0, sizeof(enc_para));
	enc_para.bitrate = 16000000;
	enc_para.width = 1920;
	enc_para.height = 1080;
	enc_para.venc_format = VIDEO_ENCODER_MJPEG;
	enc_para.fmt = REC_OUTPUT_FORMAT_RAW;
	enc_para.quality = 99;
	ret = video_rec_set_para(video_rec, &enc_para);
	if (ret) {
		printf("video_rec_set_para failed\n");
		return -1;
	}

#if (AV_STREAM_OUT_FMT == OUT_FMT_CALLBACK)
	cb.app_data = &g_stream_cb;
	cb.buf_handle = stream_buf_handle;
	video_rec_set_stream_cb(video_rec, &cb);
#else
	ret = video_rec_set_output_file(video_rec, OUTPUT_FILE_NAME);
#endif
	ret = video_rec_start(video_rec);

	g_test_end_flag = 1;
	signal(SIGINT, signal_handler);

	while (g_test_end_flag)
		oscl_mdelay(1000);

	video_rec_stop(video_rec);
	video_rec_release(video_rec);
	video_stream_release(vstream);

#if (AV_STREAM_OUT_FMT == OUT_FMT_CALLBACK)
	g_stream_cb.proc_thd_exit = 1;
	sem_post(&g_stream_cb.sem);
	pthread_join(thread, NULL);

	avstream_t *s;
	while ((s = queue_pop(g_stream_cb.queue)) != NULL) {
		OSCL_LOGW("queue count %d!", g_stream_cb.queue->count);
		free(s->data);
		free(s);
	}
	queue_deinit(g_stream_cb.queue);
	sem_destroy(&g_stream_cb.sem);
#endif

	OSCL_LOGI("finish");

	return 0;
}
#endif

