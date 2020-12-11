/*
 * omx_video_recorder.c - Standard functionality for video stream recorder.
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
#define DBG_LEVEL		DBG_WARNING
#include <oscl.h>
#include <base_component.h>
#include "media_common.h"
#include "vdisp.h"
#include "video_recorder.h"
#include "vrender_component.h"
#include "vrec_component.h"
#include "venc_component.h"
#include "omx_api.h"
#include "recorder_private.h"
#include "omx_vendor_lb.h"
#include "vsrc_camera.h"
#include <lb_omx_as_string.h>
#ifdef ENABLE_PANO
#include "pano_component.h"
#endif
#include "av_media_type.h"
#include <muxer_component.h>

#define ENCODE_VIDEO
#define ENCODE_AUDIO

#define __ALIGN(x, a) (((x) + (a) - 1) & ~((a) - 1))

static signed long long _vstream_get_time(void *handle)
{
	void *camera = video_stream_get_camera(handle);
	return vsrc_camera_get_time(camera);
}

static OMX_ERRORTYPE vrec_untunnel_event_handler(
	OMX_HANDLETYPE comp_hdl,
	OMX_PTR app_data,
	OMX_EVENTTYPE event,
	OMX_U32 data1,
	OMX_U32 data2,
	OMX_PTR event_data)
{
	OMX_ERRORTYPE ret;
	comp_info_t *al_comp = app_data;
	video_recorder_t *video_rec;
	OSCL_TRACE("comp:%p, app_data:%p, event:%x, data1:%x, data2:%x, event data:%p",
		comp_hdl, app_data, event, data1, data2, event_data);

	oscl_param_check((comp_hdl != NULL && app_data != NULL),
		OMX_ErrorBadParameter, NULL);
	al_comp = (comp_info_t *)app_data;
	video_rec = (video_recorder_t *)al_comp->priv_data;

	OSCL_LOGI("%s event:%s", al_comp->name, omx_evt_as_string(event));
	ret = untunnel_event_handler(comp_hdl, app_data, event, data1, data2, event_data);

	if (event == OMX_Eventneednewoutputfilename) {
		al_muxer_private_t *p_cb = &video_rec->mux_priv;
		ret = OMX_ErrorNotImplemented;
		if (p_cb && p_cb->app_data && p_cb->fix_duration_param.cb_get_next_file)
			ret = p_cb->fix_duration_param.cb_get_next_file(p_cb->app_data,
				event_data);
	}
	if (event == OMX_Eventfileclosed && al_comp->priv_data) {
		al_muxer_private_t *p_cb = &video_rec->mux_priv;
		OSCL_LOGI("OMX_Eventfileclosed");
		if (p_cb && p_cb->app_data && p_cb->fix_duration_param.cb_file_closed)
			p_cb->fix_duration_param.cb_file_closed(p_cb->app_data,
				event_data);
	}
	if (event == OMX_Event_update_ref && app_data) {
		muxer_sync_data_t *sync_data = event_data;
		ret = 0;
		sync_data->atime = -1;
		sync_data->vtime = -1;
		if (video_rec->audio)
			sync_data->atime = audio_stream_get_time(video_rec->audio);
		if (video_rec->vstream)
			sync_data->vtime = _vstream_get_time(video_rec->vstream);
	}
	return ret;
}

OMX_CALLBACKTYPE muxer_common_callbacks = {
	.EventHandler = vrec_untunnel_event_handler,
	.EmptyBufferDone = untunnel_empty_buffer_done,
	.FillBufferDone = untunnel_fill_buffer_done,
};

/**
 * _get_config_venc - get default config for video encoder.
 *
 * @venc_info: video encoder handle
 *
 */
static void _get_config_venc(__venc_info_t *venc_info)
{
	venc_info->venc_format = VIDEO_ENCODER_H264;
	venc_info->role = "video_encode.h264";
	venc_info->out_width = 1280;
	venc_info->out_height = 720;
	venc_info->bitrate = 6000000;
	venc_info->quality = 99;
	venc_info->vout->buf_size = ENCODER_BUF_SIZE;
	venc_info->vout->nbuffer = ENCODER_DEFAULT_BUF_NUM;
}

static int _set_venc_fmt(video_recorder_t *video_rec, video_encoder_t fmt)
{
	switch (fmt) {
	case VIDEO_ENCODER_MJPEG:
		video_rec->venc_info.venc_format = VIDEO_ENCODER_MJPEG;
		video_rec->venc_info.role = "video_encode.jpeg";
		break;
	case VIDEO_ENCODER_H265:
		video_rec->venc_info.venc_format = VIDEO_ENCODER_H265;
		video_rec->venc_info.role = "video_decoder.hevc";
		break;
	case VIDEO_ENCODER_H264:
	default:
		video_rec->venc_info.venc_format = VIDEO_ENCODER_H264;
		video_rec->venc_info.role = "video_encode.avc";
		break;
	}
	return 0;
}

/**
 * _get_config_venc - get default config for muxer.
 *
 * @venc_info: video muxer info structure
 *
 */
static void _get_config_mux(mux_info_t *mux_info)
{
	mux_info->fmt = REC_OUTPUT_FORMAT_MP4;
}

static int _set_muxer_fmt(video_recorder_t *video_rec, output_format_t fmt)
{
	int ret = -1;
	if (fmt == REC_OUTPUT_FORMAT_MP4) {
		video_rec->mux_info.fmt = REC_OUTPUT_FORMAT_MP4;
		ret = 0;
	} else if (fmt == REC_OUTPUT_FORMAT_MOV) {
		video_rec->mux_info.fmt = REC_OUTPUT_FORMAT_MOV;
		ret = 0;
	} else if (fmt == REC_OUTPUT_FORMAT_RAW) {
		video_rec->mux_info.fmt = REC_OUTPUT_FORMAT_RAW;
		ret = 0;
	} else if (fmt == REC_OUTPUT_FORMAT_TS) {
		video_rec->mux_info.fmt = REC_OUTPUT_FORMAT_TS;
		ret = 0;
	}
	return ret;
}

static int _get_muxer_type(output_format_t fmt)
{
	int type = AV_MUXER_TYPE_MOV;

	if (fmt == REC_OUTPUT_FORMAT_RAW)
		type = AV_MUXER_TYPE_RAW;
	else if (fmt == REC_OUTPUT_FORMAT_MP4)
		type = AV_MUXER_TYPE_MOV;
	else if (fmt == REC_OUTPUT_FORMAT_MOV)
		type = AV_MUXER_TYPE_MOV;
	else if (fmt == REC_OUTPUT_FORMAT_TS)
		type = AV_MUXER_TYPE_TS;
	else
		type = AV_MUXER_TYPE_MOV;

	return type;
}

static port_info_t *_vstream_getport(void *handle, vdisp_rotate_mode_e mode)
{
	void *camera = video_stream_get_camera(handle);
	return vsrc_camera_getport(camera, mode);
}

static int _vstream_putport(void *handle, port_info_t *port)
{
	void *camera = video_stream_get_camera(handle);
	return vsrc_camera_putport(camera, port);
}

static int _vstream_enable_port(void *handle, port_info_t *port)
{
	void *camera = video_stream_get_camera(handle);
	return vsrc_camera_enable_port(camera, port);
}

static int _vstream_disable_port(void *handle, port_info_t *port)
{
	void *camera = video_stream_get_camera(handle);
	return vsrc_camera_disable_port(camera, port);
}

static int _vstream_get_portinfo(void *handle,
	port_info_t *port, OMX_PARAM_PORTDEFINITIONTYPE *info)
{
	void *camera = video_stream_get_camera(handle);
	return vsrc_camera_getpara_portinfo(camera, port, info);
}

static int _rec_vstream_prepare(video_recorder_t *video_rec)
{
	OMX_PARAM_PORTDEFINITIONTYPE port_def;
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	__venc_info_t *venc_info;
	int height, width;
	OMX_U32 cap_offset[4] = {0};
	OMX_PARAM_COMPONENTROLETYPE venc_role;

	oscl_param_check(video_rec != NULL, -1, NULL);
	oscl_param_check(video_rec->src_venc == NULL, -1, NULL);

	venc_info = &video_rec->venc_info;
	/* set venc format (video encoder role) */
	if (venc_info->role) {
		venc_role.nSize = sizeof(OMX_PARAM_COMPONENTROLETYPE);
		venc_role.nVersion.nVersion = OMX_VERSION;
		strcpy((char *)venc_role.cRole, venc_info->role);
		ret = OMX_SetParameter(venc_info->al_comp.cmp_hdl,
				OMX_IndexParamStandardComponentRole, &venc_role);
		oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);
	}
	if (venc_info->venc_format == VIDEO_ENCODER_MJPEG)
		OMX_SetParameter(venc_info->al_comp.cmp_hdl,
			omx_index_lombo_venc_mjpeg_quality, &venc_info->quality);

	/* get a video source port */
	if (video_rec->venc_info.rotate)
		video_stream_set_rotate(video_rec->vstream, video_rec->venc_info.rotate);
	video_rec->src_venc = _vstream_getport(video_rec->vstream,
			video_rec->venc_info.rotate);
	oscl_param_check_exit(video_rec->src_venc != NULL, -1, NULL);

	/* get para from camera, and set it to encoder component */
	_vstream_get_portinfo(video_rec->vstream, video_rec->src_venc, &port_def);
	height = port_def.format.video.nFrameHeight;
	width = port_def.format.video.nFrameWidth;
	calc_frame_size(port_def.format.video.eColorFormat, width, height,
		&cap_offset[1], port_def.nBufferAlignment);
	OSCL_LOGI("w-h:%d-%d, offset:%d, %d, %d",
		width, height, cap_offset[0], cap_offset[1], cap_offset[2]);
	ret = OMX_SetParameter(venc_info->al_comp.cmp_hdl,
			omx_index_lombo_capture_plan_offsets, cap_offset);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);

	ret = OMX_SetParameter(venc_info->al_comp.cmp_hdl,
			omx_index_lombo_venc_rect, &venc_info->venc_rect);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);

	/* set encoder component output port para */
	port_def.nPortIndex = venc_info->vout->index;
	ret = OMX_GetParameter(venc_info->al_comp.cmp_hdl,
			OMX_IndexParamPortDefinition, &port_def);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);
	port_def.format.video.nFrameWidth = venc_info->out_width;
	port_def.format.video.nFrameHeight = venc_info->out_height;
	port_def.format.video.nBitrate = venc_info->bitrate;
	port_def.nBufferCountActual = venc_info->vout->nbuffer;
	ret = OMX_SetParameter(venc_info->al_comp.cmp_hdl,
			OMX_IndexParamPortDefinition, &port_def);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);

	int type = _get_muxer_type(video_rec->mux_info.fmt);
	ret = OMX_SetParameter(video_rec->mux_info.al_comp.cmp_hdl,
			omx_index_lombo_set_muxer_format, &type);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);

	/* connect video source with video encoder */
	ret = untunnel_setup_ports(video_rec->src_venc, video_rec->venc_info.vin);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);

	/* connect video encoder with muxer */
	ret = untunnel_setup_ports(video_rec->venc_info.vout,
			video_rec->mux_info.vin);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);

EXIT:
	return ret;

}


static int _rec_astream_prepare(video_recorder_t *video_rec)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	OMX_AUDIO_CODINGTYPE coding_type;
	OMX_AUDIO_PARAM_ADPCMTYPE adpcm_type;
	OMX_AUDIO_PARAM_PCMMODETYPE pcm_type;
	OMX_AUDIO_PARAM_AACPROFILETYPE aac_type;
	OMX_AUDIO_PARAM_G729TYPE g729_type;

	OSCL_TRACE("start");
	oscl_param_check(video_rec != NULL, -1, NULL);
	oscl_param_check(video_rec->src_amux != NULL, -1, NULL);

	/* connect audio encoder with muxer */
	ret = untunnel_setup_ports(video_rec->src_amux,
			video_rec->mux_info.ain);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);

	coding_type = audio_stream_get_coding_type(video_rec->audio);
	switch (coding_type) {
	case AUDIO_ENCODER_ADPCM:
		memset(&adpcm_type, 0, sizeof(OMX_AUDIO_PARAM_ADPCMTYPE));
		adpcm_type.nChannels = audio_stream_get_channels(video_rec->audio);
		adpcm_type.nBitsPerSample = audio_stream_get_sample_width(
				video_rec->audio);
		adpcm_type.nSampleRate = audio_stream_get_sample_rate(video_rec->audio);
		adpcm_type.nPortIndex = video_rec->mux_info.ain->index;
		ret = OMX_SetParameter(video_rec->mux_info.al_comp.cmp_hdl,
				OMX_IndexParamAudioAdpcm,
				&adpcm_type);
		break;

	case AUDIO_ENCODER_AAC:
		memset(&aac_type, 0, sizeof(OMX_AUDIO_PARAM_AACPROFILETYPE));
		aac_type.nChannels = audio_stream_get_channels(video_rec->audio);
		aac_type.nSampleRate = audio_stream_get_sample_rate(video_rec->audio);
		aac_type.nBitRate = 128000;
		aac_type.nPortIndex = video_rec->mux_info.ain->index;

		if (video_rec->mux_info.fmt == REC_OUTPUT_FORMAT_AUDIO_AAC)
			aac_type.eAACStreamFormat = OMX_AUDIO_AACStreamFormatMP4ADTS;
		else
			aac_type.eAACStreamFormat = OMX_AUDIO_AACStreamFormatRAW;
		ret = OMX_SetParameter(video_rec->mux_info.al_comp.cmp_hdl,
				OMX_IndexParamAudioAac,
				&aac_type);
		break;

	case AUDIO_ENCODER_G729:
		memset(&g729_type, 0, sizeof(OMX_AUDIO_PARAM_G729TYPE));
		g729_type.nChannels = audio_stream_get_channels(video_rec->audio);
		ret = OMX_SetParameter(video_rec->mux_info.al_comp.cmp_hdl,
				OMX_IndexParamAudioG729,
				&g729_type);
		break;

	case AUDIO_ENCODER_PCM:
	default:
		memset(&pcm_type, 0, sizeof(pcm_type));
		pcm_type.nChannels = audio_stream_get_channels(video_rec->audio);
		pcm_type.nBitPerSample = audio_stream_get_sample_width(video_rec->audio);
		pcm_type.nSamplingRate = audio_stream_get_sample_rate(video_rec->audio);
		pcm_type.nPortIndex = video_rec->mux_info.ain->index;
		ret = OMX_SetParameter(video_rec->mux_info.al_comp.cmp_hdl,
				OMX_IndexParamAudioPcm,
				&pcm_type);
		break;
	}

EXIT:
	OSCL_TRACE("end");
	return ret;

}

static void _set_prio(video_recorder_t *video_rec)
{
	OMX_PRIORITYMGMTTYPE priority;

	audio_stream_set_prio(video_rec->audio, LB_RECORDER_AUDIO_PRIO);
	priority.nVersion.nVersion = OMX_VERSION;
	priority.nGroupPriority = LB_RECORDER_MUXER_PRIO;
	OMX_SetParameter(video_rec->mux_info.al_comp.cmp_hdl,
		OMX_IndexParamPriorityMgmt, &priority);

	priority.nGroupPriority = LB_RECORDER_VIDEO_PRIO;
	OMX_SetParameter(video_rec->venc_info.al_comp.cmp_hdl,
		OMX_IndexParamPriorityMgmt, &priority);

}

void _avsync_before_start(video_recorder_t *video_rec)
{
	int nretry = 500;
	OMX_TIME_CONFIG_TIMESTAMPTYPE start_time;

	/* both video stream and audio stream is exist needs to be sync */
	if (video_rec->audio == NULL || video_rec->vstream == NULL)
		nretry = 0;

	while (nretry--) {
		video_rec->aref_time = audio_stream_get_time(video_rec->audio);
		video_rec->vref_time = _vstream_get_time(video_rec->vstream);
		if (video_rec->aref_time >= 0 && video_rec->vref_time >= 0)
			break;
		if (nretry % 20 == 0)
			OSCL_LOGE("get reftime fail(%lld, %lld), retry!",
				video_rec->aref_time, video_rec->vref_time);
		oscl_mdelay(20);
	}
	OSCL_LOGE("get reftime:%lld, %lld", video_rec->aref_time, video_rec->vref_time);
	if (video_rec->aref_time < 0)
		video_rec->aref_time = 0;
	if (video_rec->vref_time < 0)
		video_rec->vref_time = 0;
	start_time.nPortIndex = video_rec->mux_info.ain->index;
	start_time.nTimestamp = video_rec->aref_time;
	OMX_SetConfig(video_rec->mux_info.al_comp.cmp_hdl,
		omx_index_lombo_config_cur_time, &start_time);

	start_time.nPortIndex = video_rec->mux_info.vin->index;
	start_time.nTimestamp = video_rec->vref_time;
	OMX_SetConfig(video_rec->mux_info.al_comp.cmp_hdl,
		omx_index_lombo_config_cur_time, &start_time);

	start_time.nPortIndex = video_rec->venc_info.vin->index;
	start_time.nTimestamp = video_rec->vref_time;
	OMX_SetConfig(video_rec->venc_info.al_comp.cmp_hdl,
		omx_index_lombo_config_cur_time, &start_time);
}

int _set_watermark(video_recorder_t *video_rec)
{
	long i;
	int nmark;
	numeral_output_t *mark;
	wm_data_t wm_data;
	OMX_STATETYPE cur_state = OMX_StateInvalid;

	OMX_GetState(video_rec->venc_info.al_comp.cmp_hdl, &cur_state);
	if (cur_state == OMX_StateInvalid || cur_state == OMX_StateLoaded) {
		OSCL_LOGE("set watermark to encoder while encoder not init!");
		goto EXIT;
	}

	nmark = watermark_get_markarray(video_rec->watermark_private, &mark);
	if (nmark == 0 || mark == NULL)
		goto EXIT;
	for (i = 0; i < nmark; i++) {
		if (mark[i].numeral_picture.picture_size != 0) {
			memset(&wm_data, 0, sizeof(wm_data));
			wm_data.wm_pic.blending_width = mark[i].numeral_picture.width;
			wm_data.wm_pic.blending_height = mark[i].numeral_picture.height;
			wm_data.wm_pic.blending_stride = mark[i].numeral_picture.stride;
			wm_data.wm_pic.blending_x_pos = mark[i].start_x_pos;
			wm_data.wm_pic.blending_y_pos = mark[i].start_y_pos;
			wm_data.wm_pic.vir_addr = mark[i].numeral_picture.data;
			wm_data.wm_pic.phy_addr = mark[i].numeral_picture.phy_addr;
			wm_data.wm_pic.colorspace = mark[i].colorspace;
			wm_data.index = mark[i].blending_area_index;
			OMX_SetParameter(video_rec->venc_info.al_comp.cmp_hdl,
				omx_index_lombo_blending_picture_indexs,
				&mark[i]);
		} else
			OMX_SetParameter(video_rec->venc_info.al_comp.cmp_hdl,
				omx_index_lombo_disable_blending_picture,
				(OMX_PTR)i);
	}
	watermark_put_markarray(video_rec->watermark_private, mark);
EXIT:
	return 0;

}

int _rec_stop(video_recorder_t *video_rec)
{
	int ret = 0;

	OSCL_TRACE("_rec_stop:%p", video_rec);
	oscl_param_check_exit((video_rec->state == REC_STATE_RECORD), -1, NULL);

	if (video_rec->vstream) {
		_vstream_disable_port(video_rec->vstream, video_rec->src_venc);
		component_setstate(&video_rec->venc_info.al_comp, OMX_StateIdle);
	}
	if (video_rec->audio)
		audio_stream_disable_port(video_rec->audio, video_rec->src_amux);

	/* stop encoder and mux compnent */
	component_setstate(&video_rec->mux_info.al_comp, OMX_StateIdle);

	if (video_rec->vstream) {
		/* disconnect video splitter with video encoder */
		untunnel_unset_ports(video_rec->src_venc, video_rec->venc_info.vin);
		/* disconnect video enc with muxer */
		untunnel_unset_ports(video_rec->venc_info.vout, video_rec->mux_info.vin);
		_vstream_putport(video_rec->vstream, video_rec->src_venc);

		component_setstate(&video_rec->venc_info.al_comp, OMX_StateLoaded);
		video_rec->src_venc = NULL;
	}

	if (video_rec->audio) {
		/* disconnect audio enc with muxer */
		untunnel_unset_ports(video_rec->src_amux, video_rec->mux_info.ain);
		audio_stream_stop_enc(video_rec->audio, video_rec->src_amux);
		video_rec->src_amux = NULL;
		audio_stream_stop(video_rec->audio);
	}

	/* stop encoder and mux compnent */
	component_setstate(&video_rec->mux_info.al_comp, OMX_StateLoaded);

	video_rec->state = REC_STATE_INIT;
EXIT:
	OSCL_TRACE("end! ret:%x", ret);
	return ret;
}

int video_rec_prepare(void *hdl)
{
	return 0;
}

/**
 * video_rec_set_fix_duration_param - set fix duration param.
 *
 * @video_rec: video recorder handle
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int video_rec_set_fix_duration_param(void *hdl,
	void *contex, fix_duration_param_t *param)
{
	al_muxer_private_t *muxer_private;
	video_recorder_t *video_rec = hdl;
	int ret = 0;

	oscl_param_check((video_rec != NULL), -1, NULL);
	oscl_param_check((param != NULL), -1, NULL);

	pthread_mutex_lock(&video_rec->lock);
	muxer_private = &video_rec->mux_priv;
	memcpy(&muxer_private->fix_duration_param, param, sizeof(fix_duration_param_t));
	muxer_private->app_data = contex;

	OMX_SetParameter(video_rec->mux_info.al_comp.cmp_hdl,
		omx_index_vendor_set_max_duration,
		&(muxer_private->fix_duration_param.file_duration));

	pthread_mutex_unlock(&video_rec->lock);

	return ret;
}

/**
 * video_rec_stop_preview - stop preview.
 *
 * @video_rec: video recorder handle
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int video_rec_reset(void *hdl)
{
	int ret = 0;
	video_recorder_t *video_rec = hdl;

	OSCL_TRACE("start");
	OSCL_LOGE("");
	/* current state is record, stop recorder first */
	if (video_rec->state == REC_STATE_RECORD) {
		OSCL_LOGE("");
		video_rec_stop(video_rec);
	}

	video_rec->state = REC_STATE_INIT;
	return ret;
}

/**
 * video_rec_start - start recording.
 *
 * @video_rec: video recorder handle
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int video_rec_start(void *hdl)
{
	int ret = 0;
	video_recorder_t *video_rec = hdl;

	OSCL_TRACE("start");
	OSCL_LOGI("video_rec->state:%d", video_rec->state);

	pthread_mutex_lock(&video_rec->lock);
	oscl_param_check_exit((video_rec->state != REC_STATE_RECORD), -1, NULL);
	_set_prio(video_rec);

	/* check output filename is set */
#if 0
	oscl_param_check_exit(video_rec->mux_info.filename != NULL, ret, NULL);
	ret = OMX_SetParameter(video_rec->mux_info.al_comp.cmp_hdl,
			omx_index_vendor_output_filename,
			video_rec->mux_info.filename);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);
#else
	if (video_rec->mux_info.filename != NULL) {
		ret = OMX_SetParameter(video_rec->mux_info.al_comp.cmp_hdl,
				omx_index_vendor_output_filename,
				video_rec->mux_info.filename);
		oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);
	}
#endif

	if (video_rec->vstream) {
		video_stream_start_camera(video_rec->vstream);
		ret = _rec_vstream_prepare(video_rec);
		oscl_param_check_exit(ret == 0, ret, NULL);
		component_setstate(&video_rec->venc_info.al_comp, OMX_StateIdle);
		_set_watermark(video_rec);
	}

	if (video_rec->audio) {
		audio_stream_start(video_rec->audio);
		video_rec->src_amux = audio_stream_start_enc(video_rec->audio);
		oscl_param_check_exit(video_rec->src_amux != NULL, -1, NULL);
		_rec_astream_prepare(video_rec);
	}

	/* change to idle state for recorder components.
	 * muxer must set to idle after video/audio encoder prepared,
	 * becase of set port definition must be in load state */
	component_setstate(&video_rec->mux_info.al_comp, OMX_StateIdle);

	/* excute encoder and mux compnent */
	component_setstate(&video_rec->mux_info.al_comp, OMX_StateExecuting);
	if (video_rec->vstream) {
		component_setstate(&video_rec->venc_info.al_comp, OMX_StateExecuting);
		/* enable encoder port of video rec */
		ret = _vstream_enable_port(video_rec->vstream, video_rec->src_venc);
		oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);
	}
	if (video_rec->audio) {
		ret = audio_stream_enable_port(video_rec->audio, video_rec->src_amux);
		oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);
	}

	_avsync_before_start(video_rec);

	video_rec->state = REC_STATE_RECORD;
EXIT:
	if (ret != OMX_ErrorNone)
		video_rec->state = REC_STATE_ERR;
	OSCL_TRACE("video_rec->state:%x", video_rec->state);
	pthread_mutex_unlock(&video_rec->lock);
	OSCL_TRACE("end! ret:%x", ret);
	return ret;
}


/**
 * video_rec_stop - stop recording.
 *
 * @video_rec: video recorder handle
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int video_rec_stop(void *hdl)
{
	int ret = 0;
	video_recorder_t *video_rec = hdl;

	OSCL_TRACE("start video_rec->state:%x", video_rec->state);
	oscl_param_check((video_rec != NULL), -1, NULL);

	pthread_mutex_lock(&video_rec->lock);
	oscl_param_check_exit((video_rec->state == REC_STATE_RECORD), 0, NULL);
	ret = _rec_stop(hdl);

EXIT:
	pthread_mutex_unlock(&video_rec->lock);
	return ret;
}


/**
 * video_rec_set_output_file - set output file.
 *
 * @video_rec: video recorder handle
 * @filename: output filename
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int video_rec_set_output_file(void *hdl, const char *filename)
{
	int ret = 0;
	video_recorder_t *video_rec = hdl;

	OSCL_TRACE("start");
	OSCL_TRACE("filename:%s", filename);

	pthread_mutex_lock(&video_rec->lock);
	if (video_rec->mux_info.filename != NULL)
		oscl_free(video_rec->mux_info.filename);
	video_rec->mux_info.filename = oscl_strdup(filename);
	oscl_param_check_exit(video_rec->mux_info.filename != NULL,
		OMX_ErrorInsufficientResources, NULL);
	pthread_mutex_unlock(&video_rec->lock);

EXIT:
	OSCL_TRACE("end! ret:%x", ret);
	return ret;
}

/**
 * video_rec_set_output_format - set output format.
 *
 * @video_rec: video recorder handle
 * @fmt: output format
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int video_rec_set_output_format(void *hdl, output_format_t fmt)
{
	int ret;
	video_recorder_t *video_rec = hdl;
	OSCL_TRACE("start");

	oscl_param_check((video_rec != NULL), -1, NULL);

	pthread_mutex_lock(&video_rec->lock);
	ret = _set_muxer_fmt(video_rec, fmt);
	pthread_mutex_unlock(&video_rec->lock);

	OSCL_TRACE("end!");
	return ret;
}


/**
 * video_rec_set_video_stream - set video stream to recorder.
 *
 * @video_rec: video recorder handle
 * @vstream: vstream handle
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int video_rec_set_video_stream(void *hdl, void *vstream)
{
	int ret = 0;
	video_recorder_t *video_rec = hdl;
	oscl_param_check((video_rec != NULL), -1, NULL);

	pthread_mutex_lock(&video_rec->lock);
	oscl_param_check_exit((video_rec->state == REC_STATE_INIT), -1, NULL);
	if (video_rec->vstream) {
		video_stream_release(video_rec->vstream);
		video_rec->vstream = NULL;
	}

	if (vstream) {
		video_rec->vstream = vstream;
		video_stream_inc_refcnt(vstream);
	}
	OSCL_LOGE("vstream:%p, astream:%p", video_rec->vstream, video_rec->audio);
EXIT:
	pthread_mutex_unlock(&video_rec->lock);
	return ret;
}

/**
 * video_rec_set_audio_stream - set audio stream to recorder.
 *
 * @video_rec: video recorder handle
 * @vstream: audio handle
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int video_rec_set_audio_stream(void *hdl, void *astream)
{
	int ret = 0;
	video_recorder_t *video_rec = hdl;
	oscl_param_check((video_rec != NULL), -1, NULL);
	oscl_param_check((video_rec->state == REC_STATE_INIT), -1, NULL);

	pthread_mutex_lock(&video_rec->lock);
	if (video_rec->audio) {
		audio_stream_release(video_rec->audio);
		video_rec->audio = NULL;
	}

	if (astream) {
		video_rec->audio = astream;
		audio_stream_inc_refcnt(astream);
	}
	OSCL_LOGE("vstream:%p, astream:%p", video_rec->vstream, video_rec->audio);
	pthread_mutex_unlock(&video_rec->lock);
	return ret;
}

/**
 * video_rec_set_enc_area - set encoding area.
 *
 * @hdl: video recorder handle
 * @area: rect in source frame. x, y, width and height should be multiple of 16.
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int video_rec_set_enc_area(void *hdl, win_rect_t *area)
{
	video_recorder_t *video_rec = hdl;
	OSCL_TRACE("start");
	oscl_param_check((video_rec != NULL), -1, NULL);
	oscl_param_check((area != NULL), -1, NULL);

	pthread_mutex_lock(&video_rec->lock);
	if (((area->x % 16) != 0) || ((area->y % 16) != 0) ||
		((area->width % 16) != 0) || ((area->height % 16) != 0)) {
		area->x = __ALIGN(area->x, 16);
		area->y = __ALIGN(area->y, 16);
		area->width = __ALIGN(area->width, 16);
		area->height = __ALIGN(area->height, 16);
		OSCL_LOGE("warning! enc rec x/y/width/height is not 16X");
	}
	memcpy(&video_rec->venc_info.venc_rect, area, sizeof(win_rect_t));
	pthread_mutex_unlock(&video_rec->lock);

	OSCL_TRACE("end!");
	return 0;
}


/**
 * video_rec_set_para - set encoding para.
 *
 * @video_rec: video recorder handle
 * @filename: output filename
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int video_rec_set_para(void *hdl, enc_para_t *para)
{
	video_recorder_t *video_rec = hdl;
	OSCL_TRACE("start");
	oscl_param_check((video_rec != NULL), -1, NULL);
	oscl_param_check((para != NULL), -1, NULL);

	pthread_mutex_lock(&video_rec->lock);
	if (para->height)
		video_rec->venc_info.out_height = para->height;
	if (para->width)
		video_rec->venc_info.out_width = para->width;
	if (para->bitrate)
		video_rec->venc_info.bitrate = para->bitrate;
	if (para->fmt)
		_set_muxer_fmt(video_rec, para->fmt);
	if (para->quality)
		video_rec->venc_info.quality = para->quality;
	if (para->venc_format)
		_set_venc_fmt(video_rec, para->venc_format);
	video_rec->venc_info.rotate = para->rotate;
	OSCL_LOGI("preview enc rot:%d", video_rec->venc_info.rotate);
	pthread_mutex_unlock(&video_rec->lock);

	OSCL_TRACE("end!");
	return 0;
}

/**
 * video_rec_get_para - set current para of recorder.
 *
 * @video_rec: video recorder handle
 * @fmt: output format
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int video_rec_get_para(void *hdl, enc_para_t *rec_para)
{
	video_recorder_t *video_rec = hdl;
	OSCL_TRACE("start");

	oscl_param_check((video_rec != NULL), -1, NULL);
	oscl_param_check((rec_para != NULL), -1, NULL);

	pthread_mutex_lock(&video_rec->lock);
	rec_para->height = video_rec->venc_info.out_height;
	rec_para->width = video_rec->venc_info.out_width;
	rec_para->bitrate = video_rec->venc_info.bitrate;
	rec_para->rotate = video_rec->venc_info.rotate;
	rec_para->fmt = video_rec->mux_info.fmt;
	rec_para->venc_format = video_rec->venc_info.venc_format;
	pthread_mutex_unlock(&video_rec->lock);

	OSCL_TRACE("end!");
	return 0;
}


/**
 * video_rec_get_time - get current recoder time.
 *
 * @video_rec: video recorder handle
 * @time: output time in second. If recorder is not in recording state, *time will be 0;
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int video_rec_get_time(void *hdl, int *time)
{
	int ret = 0;
	video_recorder_t *video_rec = hdl;
	OMX_TIME_CONFIG_TIMESTAMPTYPE cur_time;

	oscl_param_check((video_rec != NULL), -1, NULL);
	oscl_param_check((time != NULL), -1, NULL);

	pthread_mutex_lock(&video_rec->lock);
	*time = 0;
	cur_time.nPortIndex = video_rec->mux_info.vin->index;
	ret = OMX_GetConfig(video_rec->mux_info.al_comp.cmp_hdl,
			omx_index_lombo_config_cur_time, &cur_time);
	if (ret == 0)
		*time = cur_time.nTimestamp / 1000;
	pthread_mutex_unlock(&video_rec->lock);
	return 0;
}

/**
 * video_rec_get_stat - set current status of recorder.
 *
 * @video_rec: video recorder handle
 * @fmt: output format
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int video_rec_get_status(void *hdl, int *status)
{
	video_recorder_t *video_rec = hdl;
	oscl_param_check((video_rec != NULL), -1, NULL);
	oscl_param_check((status != NULL), -1, NULL);

	switch (video_rec->state) {
	case REC_STATE_INIT:
		*status = RECORDER_STATE_INIT;
		break;
	case REC_STATE_RECORD:
		*status = RECORDER_STATE_RECORD;
		break;
	default:
		*status = RECORDER_STATE_ERR;
		break;
	}
	return 0;
}

/**
 * video_rec_wm_set_bnp - set watermark source pic.
 *
 * @video_rec: video recorder handle
 * @bnp: source picture struct
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int video_rec_wm_set_bnp(void *hdl, numeral_input_t *bnp)
{
	int ret;
	video_recorder_t *video_rec = hdl;
	oscl_param_check((video_rec != NULL), -1, NULL);
	oscl_param_check((bnp != NULL), -1, NULL);

	pthread_mutex_lock(&video_rec->lock);
	ret = watermark_set_bnp(video_rec->watermark_private, bnp);
	pthread_mutex_unlock(&video_rec->lock);
	return ret;
}

/**
 * video_rec_wm_set_mark - set watermark watermark pic index.
 *
 * @hdl: video recorder handle
 * @mk: watermark
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int video_rec_wm_set_mark(void *hdl, numeral_picture_index_t *mk)
{
	int ret = 0;
	numeral_output_t *watermark;
	long index;
	wm_data_t wm_data;
	video_recorder_t *video_rec = hdl;
	oscl_param_check((video_rec != NULL), -1, NULL);

	pthread_mutex_lock(&video_rec->lock);
	watermark = watermark_set_mark(video_rec->watermark_private, mk);
	/* if not encoding, just save in watermark and exit. watermark will set to
	 * encoder befor ecoder change to state executing*/
	if (video_rec->venc_info.al_comp.state != OMX_StateExecuting)
		goto EXIT;

	memset(&wm_data, 0, sizeof(wm_data));
	wm_data.wm_pic.blending_width = watermark->numeral_picture.width;
	wm_data.wm_pic.blending_height = watermark->numeral_picture.height;
	wm_data.wm_pic.blending_stride = watermark->numeral_picture.stride;
	wm_data.wm_pic.blending_x_pos = watermark->start_x_pos;
	wm_data.wm_pic.blending_y_pos = watermark->start_y_pos;
	wm_data.wm_pic.vir_addr = watermark->numeral_picture.data;
	wm_data.wm_pic.phy_addr = watermark->numeral_picture.phy_addr;
	wm_data.wm_pic.colorspace = watermark->colorspace;
	wm_data.index = watermark->blending_area_index;

	/* set to encoder */
	index = MAX_WATERMARK_NUM;
	if (mk == NULL)
		index = -1;
	else if (mk->total_index_num == 0 || watermark == NULL)
		index = mk->blending_area_index;
	if (index != MAX_WATERMARK_NUM)
		ret = OMX_SetParameter(video_rec->venc_info.al_comp.cmp_hdl,
				omx_index_lombo_disable_blending_picture, (OMX_PTR)index);
	else {
		ret = OMX_SetParameter(video_rec->venc_info.al_comp.cmp_hdl,
				omx_index_lombo_blending_picture_indexs,
				&wm_data);
	}
EXIT:
	pthread_mutex_unlock(&video_rec->lock);
	return ret;
}

int video_rec_take_picture(void *hdl, char *file)
{
	video_recorder_t *video_rec = hdl;
	int ret = 0;

	OSCL_TRACE("start");
	oscl_param_check(video_rec, -1, NULL);
	oscl_param_check(video_rec->pic_private, -1, NULL);

	pthread_mutex_lock(&video_rec->lock);
	ret = take_pic_start(video_rec, file);
	pthread_mutex_unlock(&video_rec->lock);

	return ret;
}

/**
 * video_rec_creat - get a video_rec handle.
 * This function will load all components video_rec needs.
 *
 * Returns video_rec handle on success, NULL otherwise..
 */
void *video_rec_creat(void)
{
	video_recorder_t *video_rec;
	int index;
	int ret;

	OSCL_TRACE("creat start");
	video_rec = oscl_zalloc(sizeof(video_recorder_t));
	oscl_param_check_exit(video_rec != NULL, -1, NULL);
	pthread_mutex_init(&video_rec->lock, NULL);

	/* init video encoder component */
	ret = component_init(&video_rec->venc_info.al_comp,
			"OMX.LB.VIDEO.ENCODECOMPONENT",
			&untunnel_common_callbacks);
	oscl_param_check_exit(ret == 0, ret, NULL);
	index = component_get_port_index(&video_rec->venc_info.al_comp, OMX_DirInput,
			OMX_PortDomainVideo);
	oscl_param_check_exit(index >= 0, -1, NULL);
	video_rec->venc_info.vin = &video_rec->venc_info.al_comp.port_info[index];
	index = component_get_port_index(&video_rec->venc_info.al_comp, OMX_DirOutput,
			OMX_PortDomainVideo);
	oscl_param_check_exit(index >= 0, -1, NULL);
	video_rec->venc_info.vout = &video_rec->venc_info.al_comp.port_info[index];

	/* init video mux component */
	ret = component_init(&video_rec->mux_info.al_comp, "OMX.LB.SINK.MUXER",
			&muxer_common_callbacks);
	oscl_param_check_exit(ret == 0, ret, NULL);
	video_rec->mux_info.al_comp.priv_data = video_rec;
	video_rec->mux_info.vin = &video_rec->mux_info.al_comp.port_info[0];
	index = component_get_port_index(&video_rec->mux_info.al_comp, OMX_DirInput,
			OMX_PortDomainVideo);
	if (index >= 0)
		video_rec->mux_info.vin = &video_rec->mux_info.al_comp.port_info[index];
	index = component_get_port_index(&video_rec->mux_info.al_comp, OMX_DirInput,
			OMX_PortDomainAudio);
	if (index >= 0)
		video_rec->mux_info.ain = &video_rec->mux_info.al_comp.port_info[index];

	/* git initial config for components */
	_get_config_venc(&video_rec->venc_info);
	_get_config_mux(&video_rec->mux_info);
	take_pic_init(video_rec);
	video_rec->watermark_private = watermark_init();

	video_rec->state = REC_STATE_INIT;
EXIT:
	if (ret != 0) {
		video_rec_release(video_rec);
		video_rec = NULL;
	}
	OSCL_TRACE("recorder hdl:%p", video_rec);
	return video_rec;
}

/**
 * video_rec_release - release resource recorder used.
 *
 * @hdl: recorder handle
 *
 * Returns 0 on success, -EERROR otherwise..
 */
void video_rec_release(void *hdl)
{
	video_recorder_t *video_rec;
	OSCL_TRACE("hdl:%p", hdl);
	video_rec = (video_recorder_t *)hdl;

	if (video_rec == NULL)
		return;

	pthread_mutex_lock(&video_rec->lock);
	if (video_rec->state == REC_STATE_RECORD) {
		OSCL_LOGE("");
		_rec_stop(video_rec);
	}
	if (video_rec->audio) {
		audio_stream_release(video_rec->audio);
		video_rec->audio = NULL;
	}
	if (video_rec->vstream) {
		video_stream_release(video_rec->vstream);
		video_rec->vstream = NULL;
	}

	watermark_deinit(video_rec->watermark_private);
	take_pic_deinit(video_rec);

	component_deinit(&video_rec->venc_info.al_comp);
	component_deinit(&video_rec->mux_info.al_comp);

	if (video_rec->mux_info.filename != NULL)
		oscl_free(video_rec->mux_info.filename);

	pthread_mutex_unlock(&video_rec->lock);
	pthread_mutex_destroy(&video_rec->lock);
	memset(video_rec, 0, sizeof(video_recorder_t));
	oscl_free(video_rec);
	OSCL_TRACE("exit");
}

static int stream_callback(void *hdl, int type, uint8_t *data, int size,
	int64_t timestamp)
{
	avstream_t stream;
	app_stream_cb_t *cb = (app_stream_cb_t *)hdl;

	oscl_param_check(cb != NULL, -1, NULL);
	oscl_param_check(data != NULL, -1, NULL);

	if (cb->buf_handle == NULL) {
		OSCL_LOGW("stream cb not set yet!");
		return -1;
	}

	if (type == AV_MEDIA_TYPE_VIDEO)
		stream.type = STREAM_TYPE_VIDEO;
	else
		stream.type = STREAM_TYPE_AUDIO;
	stream.data = data;
	stream.data_size = size;
	stream.timestamp = timestamp;
	return cb->buf_handle(cb->app_data, &stream);
}

int video_rec_set_stream_cb(void *hdl, app_stream_cb_t *cb)
{
	video_recorder_t *video_rec = (video_recorder_t *)hdl;
	external_stream_writer_t writer;

	oscl_param_check(video_rec != NULL, -1, NULL);

	video_rec->stream_cb = *cb;

	writer.app_data = &video_rec->stream_cb;
	writer.write = stream_callback;
	OMX_SetParameter(video_rec->mux_info.al_comp.cmp_hdl,
		omx_index_lombo_set_stream_callback,
		&writer);

	return 0;
}

int video_rec_collision_enable(void *hdl, long collision_cache_time)
{
	int ret;
	video_recorder_t *video_rec = (video_recorder_t *)hdl;

	oscl_param_check(video_rec != NULL, -1, NULL);

	ret = OMX_SetParameter(video_rec->mux_info.al_comp.cmp_hdl,
		omx_index_collision_enable,
		(void *)collision_cache_time);
	return ret;
}

int video_rec_collision_disable(void *hdl)
{
	int ret;
	video_recorder_t *video_rec = (video_recorder_t *)hdl;

	oscl_param_check(video_rec != NULL, -1, NULL);

	ret = OMX_SetParameter(video_rec->mux_info.al_comp.cmp_hdl,
		omx_index_collision_disable,
		NULL);
	return ret;
}

int video_rec_collision_start(void *hdl, colli_file_cfg_t *cfg)
{
	int ret;
	video_recorder_t *video_rec = (video_recorder_t *)hdl;
	colli_muxfile_cfg_t colli_mux_cfg;

	oscl_param_check(video_rec != NULL, -1, NULL);
	oscl_param_check(cfg != NULL, -1, NULL);
	oscl_param_check((cfg->filename != NULL || cfg->cb.buf_handle != NULL), -1, NULL);

	video_rec->stream_cb = cfg->cb;

	colli_mux_cfg.restart = 0;
	colli_mux_cfg.cache_time = cfg->cache_time * 1000;
	colli_mux_cfg.total_time = cfg->total_time * 1000;
	colli_mux_cfg.filename = cfg->filename;
	colli_mux_cfg.ext_writer.app_data = &video_rec->colli_cb;
	colli_mux_cfg.ext_writer.write = stream_callback;
	ret = OMX_SetParameter(video_rec->mux_info.al_comp.cmp_hdl,
		omx_index_collision_start,
		&colli_mux_cfg);
	return ret;
}

