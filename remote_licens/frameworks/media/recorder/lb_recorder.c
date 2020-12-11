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

#include <oscl.h>
#include <base_component.h>
#include "vrender_component.h"
#include "vrec_component.h"
#include "omx_api.h"
#include "lb_recorder.h"
#include <getopt.h>
#include "recorder_private.h"
#include "recorder_pano.h"

#ifndef __EOS__
#define RTM_EXPORT(func)
#endif

#ifdef __EOS__
#define ISP_DEV_NAME "isp"
#define ISP_CAP_DEV_NAME "isp_cap.0"
#define ISP_PREV_DEV_NAME "isp_prev.0"
#elif defined __linux__
#define ISP_DEV_NAME "isp"
#define ISP_CAP_DEV_NAME "/dev/video3"
#define ISP_PREV_DEV_NAME "/dev/video4"
#endif

typedef void *video_rec_handle;

typedef struct lb_recorder {
	pthread_mutex_t *lock;
	rec_param_t param;
	video_rec_handle video_rec;
	int is_isp;
	void *vprev_stream;
	void *venc_stream;
	astream_handle audio;
	int prev_rot;
} lb_recorder_t;

int _lb_rec_set_para(lb_recorder_t *recorder, rec_param_t *para)
{
	OSCL_TRACE("start");
	enc_para_t enc_para;
	vsrc_param_t vsrc_para;
	char *dev;
	int ret;

	oscl_param_check((recorder != NULL), -1, NULL);

	memcpy(&recorder->param, para, sizeof(rec_param_t));
	if (recorder->is_isp == 1) {
		dev = ISP_PREV_DEV_NAME;
		if (para->source_height >= 1080)
			dev = ISP_CAP_DEV_NAME;
		ret = video_stream_set_camera_source(recorder->venc_stream, dev);
		if (ret != 0) {
			recorder->is_isp = 0;
			dev = ISP_PREV_DEV_NAME;
			ret = video_stream_set_camera_source(recorder->venc_stream, dev);
		}
	}
	vsrc_para.frame_rate = para->frame_rate;
	vsrc_para.height = para->source_height;
	vsrc_para.width = para->source_width;
	ret = video_stream_set_src_para(recorder->venc_stream, &vsrc_para);
	oscl_param_check_exit((ret == 0), -1, NULL);

	recorder->prev_rot = para->rotate;

	enc_para.bitrate = para->bitrate;
	enc_para.width = para->width;
	enc_para.height = para->height;
	enc_para.fmt = para->file_fmt;
	enc_para.aenc_format = para->aenc_format;
	enc_para.venc_format = para->venc_format;
	enc_para.rotate = para->enc_rot;
	ret = video_rec_set_para(recorder->video_rec, &enc_para);
	oscl_param_check_exit((ret == 0), -1, NULL);

	ret = video_rec_set_enc_area(recorder->video_rec, &para->enc_rect);
	oscl_param_check_exit((ret == 0), -1, NULL);

	if (para->aenc_format)
		ret = audio_stream_set_coding_type(recorder->audio, para->aenc_format);
	if (para->audio_channels)
		ret = audio_stream_set_channels(recorder->audio, para->audio_channels);
	if (para->audio_sample_width) {
		ret = audio_stream_set_sample_width(recorder->audio,
			para->audio_sample_width);
	}
	if (para->audio_sample_rate) {
		ret = audio_stream_set_sample_rate(recorder->audio,
			para->audio_sample_rate);
	}

EXIT:
	OSCL_TRACE("end!");
	return ret;
}

int _lb_rec_get_para(lb_recorder_t *recorder, rec_param_t *rec_para)
{
	OSCL_TRACE("start");
	enc_para_t enc_para;
	vsrc_param_t vsrc_para;

	oscl_param_check((recorder != NULL), -1, NULL);

	memset(rec_para, 0, sizeof(rec_param_t));
	video_stream_get_src_para(recorder->venc_stream, &vsrc_para);
	rec_para->source_height = vsrc_para.height;
	rec_para->source_width = vsrc_para.width;
	rec_para->frame_rate = vsrc_para.frame_rate;

	rec_para->rotate = recorder->prev_rot;

	video_rec_get_para(recorder->video_rec, &enc_para);
	rec_para->bitrate = enc_para.bitrate;
	rec_para->width = enc_para.width;
	rec_para->height = enc_para.height;
	rec_para->file_fmt = enc_para.fmt;
	rec_para->enc_rot = enc_para.rotate;

	if (rec_para->audio_channels)
		rec_para->audio_channels = audio_stream_get_channels(recorder->audio);
	if (rec_para->audio_sample_width) {
		rec_para->audio_sample_width = audio_stream_get_sample_width(
			recorder->audio);
	}
	if (rec_para->audio_sample_rate) {
		rec_para->audio_sample_rate = audio_stream_get_sample_rate(
			recorder->audio);
	}
	if (rec_para->aenc_format)
		rec_para->aenc_format = audio_stream_get_coding_type(recorder->audio);

	OSCL_TRACE("end!");
	return 0;
}

int _lb_rec_set_vsrc_dev(lb_recorder_t *recorder, char *src)
{
	OSCL_TRACE("start");
	int ret;
	char *dev;

	oscl_param_check((recorder != NULL), -1, NULL);
	oscl_param_check((src != NULL), -1, NULL);

	dev = src;
	recorder->is_isp = 0;
	if (strcmp(src, ISP_DEV_NAME) == 0) {
		dev = ISP_PREV_DEV_NAME;
		recorder->is_isp = 1;
	}

	ret = video_stream_set_camera_source(recorder->vprev_stream, (char *)dev);
	ret = video_stream_set_camera_source(recorder->venc_stream, (char *)dev);

	OSCL_TRACE("end!");
	return ret;
}

/**
 * lb_recorder_creat - get a recorder handle.
 * This function will load all components recorder needs.
 *
 * Returns recorder handle on success, NULL otherwise..
 */
void *lb_recorder_creat(void)
{
	lb_recorder_t *recorder = NULL;
	int ret = 0;

	OSCL_TRACE("creat start");
	recorder = oscl_zalloc(sizeof(lb_recorder_t));
	oscl_param_check_exit(recorder != NULL, -1, NULL);

	recorder->lock = oscl_zalloc(sizeof(pthread_mutex_t));
	if (recorder->lock)
		pthread_mutex_init(recorder->lock, NULL);

	recorder->venc_stream = video_stream_create();
	recorder->vprev_stream = video_stream_create();
	recorder->audio = audio_stream_creat();

	recorder->video_rec = video_rec_creat();
	oscl_param_check_exit(recorder->video_rec != NULL, -1, NULL);

	video_rec_set_video_stream(recorder->video_rec, recorder->venc_stream);
	video_rec_set_audio_stream(recorder->video_rec, recorder->audio);

EXIT:
	if (ret != 0 && recorder != NULL) {
		lb_recorder_release(recorder);
		recorder = NULL;
	}
	OSCL_TRACE("recorder hdl:%p", recorder);
	return recorder;
}
RTM_EXPORT(lb_recorder_creat);

/**
 * lb_recorder_release - release resource recorder used.
 *
 * @hdl: recorder handle
 *
 * Returns 0 on success, -EERROR otherwise..
 */
void lb_recorder_release(void *hdl)
{
	lb_recorder_t *recorder;
	OSCL_TRACE("hdl:%p", hdl);
	recorder = (lb_recorder_t *)hdl;

	if (hdl == NULL)
		return;
	pthread_mutex_lock(recorder->lock);
	video_rec_release(recorder->video_rec);
	recorder->video_rec = NULL;

	audio_stream_release(recorder->audio);
	recorder->audio = NULL;

	if (recorder->vprev_stream)
		video_stream_release(recorder->vprev_stream);
	if (recorder->venc_stream)
		video_stream_release(recorder->venc_stream);

	recorder->vprev_stream = NULL;
	recorder->venc_stream = NULL;
	pthread_mutex_unlock(recorder->lock);

	if (recorder->lock) {
		pthread_mutex_destroy(recorder->lock);
		oscl_free(recorder->lock);
	}
	oscl_free(recorder);

	OSCL_TRACE("return");
}
RTM_EXPORT(lb_recorder_release);

/**
 * lb_recorder_ctrl - will send commands to a recorder
 *
 * @hdl: handle of recorder to be controled. recorder handle is get by lb_recorder_creat
 * @cmd: index of command
 * @para: pointer to application allocated structure to be used by the recorder.
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int lb_recorder_ctrl(void *hdl, int cmd, void *para)
{
	lb_recorder_t *recorder = (lb_recorder_t *)hdl;
	int ret = 0;

	OSCL_TRACE("hdl:%p, cmd:%s, para:%p", hdl, media_cmd_as_string(cmd), para);
	oscl_param_check(hdl != NULL, -1, NULL);

	pthread_mutex_lock(recorder->lock);
	switch (cmd) {
	case LB_REC_PREPARE:
		break;
	case LB_REC_SET_FIX_DURATION_PARA:
		ret = video_rec_set_fix_duration_param(recorder->video_rec, hdl, para);
		break;
	case LB_REC_SET_SRC_RES:
		ret = video_stream_set_res(recorder->vprev_stream, para);
		break;
	case LB_REC_PREVIEW:
		ret = video_stream_start_preview(recorder->vprev_stream);
		break;
	case LB_REC_STOP_PREVIEW:
		ret = video_stream_stop_preview(recorder->vprev_stream);
		video_stream_stop_camera(recorder->vprev_stream);
		break;
	case LB_REC_START:
		ret = video_rec_start(recorder->video_rec);
		break;
	case LB_REC_STOP:
		ret = video_rec_stop(recorder->video_rec);
		video_stream_stop_camera(recorder->venc_stream);
		break;
	case LB_REC_RESET:
		ret = video_rec_reset(recorder->video_rec);
		audio_stream_stop(recorder->audio);
		video_stream_stop_camera(recorder->vprev_stream);
		video_stream_stop_camera(recorder->venc_stream);
		break;
	case LB_REC_SET_OUTPUT_FILE:
		ret = video_rec_set_output_file(recorder->video_rec, (char *)para);
		break;
	case LB_REC_SET_ENC_PARA:
		ret = video_rec_set_para(recorder->video_rec, (enc_para_t *)para);
		break;
	case LB_REC_SET_OUTPUT_FORMAT:
		ret = video_rec_set_output_format(recorder->video_rec,
						  (output_format_t)para);
		break;
	case LB_REC_SET_ROTATE:
		recorder->prev_rot = (long)para;
		break;
	case LB_REC_SET_VIDEO_SOURCE:
		ret = _lb_rec_set_vsrc_dev(recorder, (char *)para);
		break;
	case LB_REC_SET_SINK:
		break;

	case LB_REC_SET_CB_ROT:
		video_stream_set_rotate(recorder->vprev_stream, (long)para);
		break;
	case LB_REC_SET_CB_FRAME:
		pthread_mutex_unlock(recorder->lock);
		ret = video_stream_set_frame_cb(recorder->vprev_stream, para);
		pthread_mutex_lock(recorder->lock);
		break;
	case LB_REC_FREE_FRAME:
		ret = video_stream_return_frame((frame_t *)para);
		break;
	case LB_REC_SET_LAYER_LEVEL:
		ret = video_stream_set_layer_level(recorder->vprev_stream,
					    (video_layer_level_t)para);
		break;
	case LB_REC_SET_DISP_PARA:
		ret = video_stream_set_disp_para(recorder->vprev_stream,
			para, recorder->prev_rot);
		break;
	case LB_REC_SET_DISP_SCALE_MODE:
		ret = video_stream_set_scale_mode(recorder->vprev_stream,
			(scale_mode_e)para);
		break;
	case LB_REC_SET_PARA:
		ret = _lb_rec_set_para(recorder, (rec_param_t *)para);
		break;
	case LB_REC_GET_PARA:
		ret = _lb_rec_get_para(recorder, (rec_param_t *)para);
		break;
	case LB_REC_GET_STATUS:
		ret = video_rec_get_status(recorder->video_rec, para);
		break;
	case LB_REC_GET_TIME:
		ret = video_rec_get_time(recorder->video_rec, (int *)para);
		break;
	case LB_REC_SET_WATERMARK_SOURCE:
		ret = video_rec_wm_set_bnp(recorder->video_rec, para);
		break;
	case LB_REC_SET_WATERMARK_INDEX:
		ret = video_rec_wm_set_mark(recorder->video_rec, para);
		break;
	case LB_REC_PANO_CREAT:
		ret = video_pano_creat(recorder->video_rec);
		break;
	case LB_REC_PANO_RELEASE:
		video_pano_release(recorder->video_rec);
		break;
	case LB_REC_PANO_START:
		ret = video_pano_start(recorder->video_rec);
		break;
	case LB_REC_PANO_STOP:
		ret = video_pano_stop(recorder->video_rec);
		break;
	case LB_REC_PANO_CALI_PROCESS:
		ret = video_pano_cali_process(recorder->video_rec, (cali_contex_t *)para);
		break;
	case LB_REC_PANO_SET_PREVIEW_SIZE:
		ret = video_pano_set_preview_size(recorder->video_rec, (vsize_t *)para);
		break;
	case LB_REC_PANO_SET_DISP_MODE:
		ret = video_pano_set_disp_mode(recorder->video_rec, (win_para_t *)para);
		break;
	case LB_REC_PANO_SET_CALI_PARA:
		ret = video_pano_set_cali_para(recorder->video_rec,
						(cali_param_t *)para);
		break;
	case LB_REC_PANO_SET_INIT_PARA:
		ret = video_pano_set_init_para(recorder->video_rec,
						(pano_param_t *)para);
		break;
	case LB_REC_PANO_SET_CALI_DATA:
		ret = video_pano_set_cali_data(recorder->video_rec,
						(cali_out_data_t *)para);
		break;
	case LB_REC_PANO_GET_CALI_DATA:
		ret = video_pano_get_cali_data(recorder->video_rec,
						(cali_out_data_t *)para);
		break;
	case LB_REC_PANO_SET_LAYER_LEVEL:
		ret = video_pano_set_layer_level(recorder->video_rec,
					    (video_layer_level_t)para);
		break;
	case LB_REC_TAKE_PICTURE:
		ret = video_rec_take_picture(recorder->video_rec, para);
		break;
	case LB_REC_SET_AUDIO_MUTE:
		ret = audio_stream_set_mute(recorder->audio, (long)para);
		break;
	case LB_REC_SET_STREAM_CB:
		video_rec_set_stream_cb(recorder->video_rec, (app_stream_cb_t *)para);
		break;
	case LB_REC_SET_PORT_FILTER:
		video_stream_set_port_cb(recorder->vprev_stream,
					(app_port_filter_t *)para);
		break;
	default:
		break;
	}

	pthread_mutex_unlock(recorder->lock);
	OSCL_TRACE("hdl:%p, cmd:%s, ret:%x", hdl, media_cmd_as_string(cmd), ret);
	return ret;
}
RTM_EXPORT(lb_recorder_ctrl);
