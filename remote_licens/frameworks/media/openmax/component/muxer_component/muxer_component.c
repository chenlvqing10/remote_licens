/*
 * muxer_component.c - Standard functionality for the media encodec component.
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
#define LOG_TAG			"muxer_component"

#include <oscl.h>
#include <base_component.h>
#include "muxer_com.h"
#include "muxer_common.h"
#include "av_media_type.h"
#include "av_media_type.h"
#include "muxer_handle.h"
#include "muxer_private.h"
#include "muxer_collision.h"
#include "video_recorder.h"
#include "lombo_muxer.h"

#define FILE_NAME_MAX_LENTH 512

#define SINK_INPUT_DEFAULT_SIZE (16 * 1024)
#define BLKSIZE 1024
#define OMX_VIDEO_BUF_SIZE (1024 * 1024)
#define IS_CONFIG_BUF(buffer) (buffer->nFlags & OMX_BUFFERFLAG_CODECCONFIG)
#define IS_ENDING_BUF(buffer) (buffer->nFlags & OMX_BUFFERFLAG_ENDOFFRAME)

static int _get_filename(MuxerContext *context)
{
	int ret;
	char *filename_cb = NULL;
	lb_omx_component_t *component = (lb_omx_component_t *)context->lb_comp;

	oscl_param_check((context != NULL), OMX_ErrorBadParameter, NULL);
	oscl_param_check((context->id != AV_MUXER_TYPE_INVALID), 0, NULL);
	oscl_param_check((context->id != AV_MUXER_TYPE_RAW), 0, NULL);

	filename_cb = oscl_zalloc(FILE_NAME_MAX_LENTH);
	oscl_param_check(filename_cb != NULL, -1, NULL);

	ret = ((component->callbacks.EventHandler))(
		&component->base_comp,
		component->callback_data,
		OMX_Eventneednewoutputfilename,
		0, 0, filename_cb);
	if (ret != 0 || filename_cb[0] == 0) {
		oscl_free(filename_cb);
		filename_cb = NULL;
	}
	if (context->filename) {
		OSCL_LOGE("context filename is not null when getting next file");
		oscl_free(context->filename);
	}
	context->filename = filename_cb;

	return 0;
}

static int _update_ref(MuxerContext *context)
{
	int ret;
	muxer_sync_data_t sync_data;
	lb_omx_component_t *component = (lb_omx_component_t *)context->lb_comp;
	OMX_TICKS audio_time;
	OMX_TICKS video_time;
	int sync_time = 0;

	oscl_param_check((context != NULL), OMX_ErrorBadParameter, NULL);

	memset(&sync_data, 0, sizeof(muxer_sync_data_t));
	ret = ((component->callbacks.EventHandler))(
		&component->base_comp,
		component->callback_data,
		OMX_Event_update_ref,
		0, 0, &sync_data);
	if (sync_data.atime > 0 && sync_data.vtime > 0) {
		video_time = sync_data.vtime - context->ref_time[VIDEO_INDEX];
		audio_time = sync_data.atime - context->ref_time[AUDIO_INDEX];
		audio_time = AUDPTS_TO_TIME(context, audio_time);
		sync_time = audio_time - video_time;
	}
	if (abs(sync_time) > 100) {
		OSCL_LOGI("%p muxer resync, ori:%lld,%lld, sync:%lld,%lld, time:%d",
			context,
			context->ref_time[VIDEO_INDEX], context->ref_time[AUDIO_INDEX],
			sync_data.vtime, sync_data.atime, sync_time);
		context->ref_time[VIDEO_INDEX] -= sync_time / 2;
		if (context->vbuffer) {
			context->intime[VIDEO_INDEX] = context->vbuffer->nTimeStamp
				- context->ref_time[VIDEO_INDEX];
		}
		OSCL_LOGI("%p muxer resync:%lld,%lld", context,
			context->ref_time[VIDEO_INDEX],
			context->ref_time[AUDIO_INDEX]);
	}

	return ret;
}

static void _muxer_print_dbg(lb_omx_component_t *component)
{
	MuxerContext *context = NULL;
	context = component->component_private;

	OSCL_LOGE("=context:%p, coll:%p==", context, context->colli_info);
	log_muxer_context(context);
}

static int _check_stop_muxer(lb_omx_component_t *component,
	OMX_BUFFERHEADERTYPE *buf)
{
	int64_t cur_time = 0;
	MuxerContext *context;
	int ret = 0;

	context = component->component_private;

	oscl_param_check(buf != NULL, 0, NULL);
	if (context->stream_cfg.exist[VIDEO_INDEX] && buf->nInputPortIndex != VIDEO_INDEX)
		goto EXIT;
	if (context->start_ref < 0 || context->file_total_time <= 0)
		goto EXIT;

	/* recording time > total time set by apps, stop muxer */
	cur_time = buf->nTimeStamp - context->start_ref;
	if (buf->nInputPortIndex == AUDIO_INDEX)
		cur_time = AUDPTS_TO_TIME(context, cur_time);
	/* get next filename before file finished, because it may take much time */
	if (cur_time >= (context->file_total_time - 500) && (context->filename == NULL))
		context->ctrl_flag |= CONTEXT_FILENAME_MASK;

	/* only stop muxer with video key frame if video stream existing, */
	if (cur_time >= (context->file_total_time - 100) &&
		(buf->nInputPortIndex == AUDIO_INDEX ||
		(buf->nFlags & OMX_BUFFERFLAG_SYNCFRAME))) {
		OSCL_LOGI("%s file_time:%d, starttime:%lld, cur:%lld, stamp:%lld",
			context->mux_hdl->filename,
			context->file_total_time, context->start_ref,
			cur_time, buf->nTimeStamp);
		ret = 1;
		context->start_ref += context->file_total_time;
	}

EXIT:
	return ret;
}


static int _start_new_muxer(MuxerContext *context,
	OMX_BUFFERHEADERTYPE *buf)
{
	if (context->id == AV_MUXER_TYPE_INVALID)
		goto EXIT;

	/* start muxer with video key frame if video stream existing, */
	if (context->stream_cfg.exist[VIDEO_INDEX]) {
		if (buf->nInputPortIndex != VIDEO_INDEX ||
		    (buf->nFlags & OMX_BUFFERFLAG_SYNCFRAME) == 0)
			goto EXIT;
	}

	if (context->filename == NULL && context->id != AV_MUXER_TYPE_RAW)
		_get_filename(context);

	context->mux_hdl = start_muxer_handle(&context->mux_hdl_mgr,
		&context->stream_cfg, context->id,
		context->filename, &context->ext_writer);
	if (context->mux_hdl && context->filename != NULL) {
		oscl_free(context->filename);
		context->filename = NULL;
	}
	if (context->mux_hdl == NULL)
		context->start_ref = -1;
EXIT:
	return 0;
}

static int _save_codec_cfg(MuxerContext *context, OMX_BUFFERHEADERTYPE *inbuffer)
{
	int index;
	muxer_vpacket_t *venc_packet;
	AV_CodecParameters *codec_cfg;

	oscl_param_check((context != NULL), OMX_ErrorBadPortIndex, NULL);
	oscl_param_check((inbuffer != NULL), OMX_ErrorBadPortIndex, NULL);
	venc_packet = (muxer_vpacket_t *)inbuffer->pBuffer;
	index = inbuffer->nInputPortIndex;
	codec_cfg = &context->stream_cfg.para[index];
	pthread_mutex_lock(&context->context_mutex);
	if (codec_cfg->extradata != NULL) {
		OSCL_LOGI("recevied new extradata\n");
		oscl_free(codec_cfg->extradata);
	}
	codec_cfg->extradata =
		oscl_zalloc(venc_packet->size + AV_INPUT_BUFFER_PADDING_SIZE);
	memcpy(codec_cfg->extradata, venc_packet->vir_addr, venc_packet->size);
	codec_cfg->extradata_size = venc_packet->size;
	OSCL_LOGE("video extradata(%d)\n", codec_cfg->extradata_size);
	pthread_mutex_unlock(&context->context_mutex);
	return 0;
}

static int _frame_rebuild(MuxerContext *context, OMX_BUFFERHEADERTYPE *vbuf)
{
	int ret = -1;
	unsigned char *tmp_buffer;
	muxer_vpacket_t *venc_packet = (muxer_vpacket_t *)vbuf->pBuffer;
	muxer_vpacket_t *combine_packet = &context->combine_packet;

	/* err state in previous rebuilding process */
	if (context->rebuilding_flag && (combine_packet->vir_addr == NULL))
		goto EXIT;

	if (context->rebuilding_flag && IS_ENDING_BUF(context->combine_header))
		goto EXIT;

	/* copy buffer header info when we recevice first part of a frame */
	if (context->rebuilding_flag == 0) {
		context->rebuilding_flag = 1;
		if (context->combine_header != NULL) {
			OSCL_LOGE("combined buffer error!");
			goto EXIT;
		}
		context->combine_header = oscl_zalloc(sizeof(OMX_BUFFERHEADERTYPE));
		memcpy(context->combine_header, vbuf, sizeof(OMX_BUFFERHEADERTYPE));
		context->combine_header->pBuffer = (void *)combine_packet;
		memcpy(combine_packet, venc_packet, sizeof(muxer_vpacket_t));
		combine_packet->vir_addr = NULL;
		combine_packet->size = 0;
	}

	/* realloc buffers when compined packet is full */
	tmp_buffer = oscl_zalloc(combine_packet->size + venc_packet->size);
	if (tmp_buffer && combine_packet->vir_addr)
		memcpy(tmp_buffer, combine_packet->vir_addr, combine_packet->size);
	if (combine_packet->vir_addr)
		oscl_free(combine_packet->vir_addr);
	combine_packet->vir_addr = tmp_buffer;
	if (combine_packet->vir_addr == NULL) {
		OSCL_LOGE("malloc combine_packet pbuffer error");
		memset(combine_packet, 0, sizeof(muxer_vpacket_t));
		if (context->combine_header != NULL) {
			oscl_free(context->combine_header);
			context->combine_header = NULL;
		}
		goto EXIT;
	}
	memcpy(combine_packet->vir_addr + combine_packet->size,
		venc_packet->vir_addr, venc_packet->size);
	combine_packet->size += venc_packet->size;
	context->combine_header->nFlags = vbuf->nFlags;
	ret = 0;
EXIT:
	if (IS_ENDING_BUF(vbuf))
		context->rebuilding_flag = 0;
	return ret;
}

OMX_BUFFERHEADERTYPE *_component_get_buffer(MuxerContext *context, int port_index)
{
	lb_omx_component_t *comp;
	oscl_queue_t *queue;
	OMX_BUFFERHEADERTYPE *tmp_buf = NULL;
	OMX_TICKS tmp_time = 0;
	OMX_TICKS frame_interval = 0;
	base_port_t *base_port;
	int ret = 0;

	comp = (lb_omx_component_t *)context->lb_comp;
	queue = &comp->port[port_index].buffer_queue;
	base_port = &comp->port[port_index];

	tmp_buf = oscl_queue_dequeue(queue);
	if (tmp_buf == NULL)
		goto EXIT;
	oscl_sem_trywait(comp->buf_mgnt_sem);
	if (port_index == VIDEO_INDEX) {
		if (IS_CONFIG_BUF(tmp_buf)) {
			_save_codec_cfg(context, tmp_buf);
			base_port->return_buffer(base_port, tmp_buf);
			tmp_buf = NULL;
			context->stream_cfg.is_ready[VIDEO_INDEX] = 1;
			goto EXIT;
		}
		if (IS_ENDING_BUF(tmp_buf) == 0 || context->rebuilding_flag) {
			ret = _frame_rebuild(context, tmp_buf);
			base_port->return_buffer(base_port, tmp_buf);
			if (ret == 0 && context->rebuilding_flag == 0)
				tmp_buf = context->combine_header;
			else {
				tmp_buf = NULL;
				goto EXIT;
			}
		}
		if (context->stream_cfg.is_ready[VIDEO_INDEX] == 0) {
			OSCL_LOGW("Compress(%d) get videoframe while pps is null.",
				base_port->port_param.format.video.eCompressionFormat);
			context->stream_cfg.is_ready[VIDEO_INDEX] = 1;
		}
	}

	if (tmp_buf->nTimeStamp > context->ref_time[port_index])
		tmp_time = tmp_buf->nTimeStamp - context->ref_time[port_index];
	if (port_index == AUDIO_INDEX)
		tmp_time = AUDPTS_TO_TIME(context, tmp_time);
	frame_interval = tmp_time - context->intime[port_index];
	if (frame_interval > 100)
		OSCL_LOGW("%p (%d) may lost frame, time(%lld):%lld,%lld, ref:%lld",
			context, port_index, frame_interval,
			(long long)context->intime[port_index], (long long)tmp_time,
			(long long)context->ref_time[port_index]);
	if (frame_interval > 0)
		context->intime[port_index] = tmp_time;

	/* for debug */
	if (oscl_queue_get_num(queue) >= base_port->port_param.nBufferCountActual) {
		OSCL_LOGE("%p vbuf num:%d, abuf num:%d, dts:%lld:%lld", context,
			oscl_queue_get_num(&comp->port[VIDEO_INDEX].buffer_queue),
			oscl_queue_get_num(&comp->port[AUDIO_INDEX].buffer_queue),
			context->vbuffer->nTimeStamp, context->abuffer->nTimeStamp);
	}
#if 0
	/* venc component may trans buffer not in chronological order (with p frame)*/
	if (frame_interval < 0) {
		OSCL_LOGW("%p frame_interval < 0 : %lld - %lld", context,
			(long long)tmp_buf->nTimeStamp,
			(long long)context->ref_time[port_index]);
	}
#endif
EXIT:
	return tmp_buf;
}

OMX_BUFFERHEADERTYPE *_component_receive_buffer(MuxerContext *context)
{
	OMX_BUFFERHEADERTYPE *tmpbuf = NULL;
	OMX_TICKS sync_time;

	if (context->vbuffer == NULL)
		context->vbuffer = _component_get_buffer(context, VIDEO_INDEX);
	if (context->abuffer == NULL)
		context->abuffer = _component_get_buffer(context, AUDIO_INDEX);

	if ((context->rebuilding_flag == 1) ||
		(context->stream_cfg.is_ready[VIDEO_INDEX] == 0 &&
			context->stream_cfg.exist[VIDEO_INDEX])) {
		tmpbuf = context->vbuffer;
		goto EXIT;
	}
	if (context->abuffer == NULL || context->vbuffer == NULL) {
		tmpbuf = (context->vbuffer) ? (context->vbuffer) : context->abuffer;
		goto EXIT;
	}

	if (context->intime[AUDIO_INDEX] < context->intime[VIDEO_INDEX])
		tmpbuf = context->abuffer;
	else
		tmpbuf = context->vbuffer;

EXIT:
	if (tmpbuf) {
		sync_time = context->intime[tmpbuf->nInputPortIndex] - context->sync_time;
		if (sync_time > 30000) {
			context->sync_time = context->intime[tmpbuf->nInputPortIndex];
			_update_ref(context);
		}
		if (sync_time < 0)
			context->sync_time = context->intime[tmpbuf->nInputPortIndex];
	}
	return tmpbuf;

}

static OMX_ERRORTYPE muxer_port_flush_buffer(base_port_t *port)
{
	MuxerContext *context = NULL;
	int ret;
	context = ((lb_omx_component_t *)port->component)->component_private;
	if (context && context->colli_info)
		colli_flush(context->colli_info);
	ret = base_port_flush_buffer(port);
	if (port->port_param.eDomain == OMX_PortDomainVideo)
		mux_context_free_combined_packet(context);
	return ret;
}

static OMX_ERRORTYPE muxer_port_recive_buffer(base_port_t *port,
	OMX_BUFFERHEADERTYPE *buffer)
{
	OMX_U32 port_idx;
	lb_omx_component_t *component = (lb_omx_component_t *)port->component;
	MuxerContext *context = NULL;
	int64_t ref_time = 0;

	OSCL_TRACE("%p:%p", port, buffer);
	context = component->component_private;

	port_idx = buffer->nInputPortIndex;
	oscl_param_check((port_idx == port->port_param.nPortIndex),
		OMX_ErrorBadPortIndex, NULL);
	if (port_idx == AUDIO_INDEX)
		ref_time = context->ref_time[AUDIO_INDEX];
	if (buffer->nTimeStamp < ref_time) {
		OSCL_LOGD("%d drop frame(%p) time:%lld ref:%lld", port_idx, buffer,
			(long long)buffer->nTimeStamp, (long long)ref_time);
		return -1;
	}

	return base_port_recive_buffer(port, buffer);
}

static void muxer_component_buffer_handle(OMX_HANDLETYPE stand_com,
	OMX_BUFFERHEADERTYPE *inbuffer,
	OMX_BUFFERHEADERTYPE *outbuffer)
{
	MuxerContext *context = NULL;
	lb_omx_component_t *component;
	int index;
	int ret = 0;

	component = get_lb_component(stand_com);
	context = component->component_private;
	if (inbuffer && context->mux_hdl) {
		ret = muxer_handle_mux_buffer(context->mux_hdl, inbuffer);
		if (context->start_ref < 0) {
			index = VIDEO_INDEX;
			if (!context->stream_cfg.exist[VIDEO_INDEX])
				index = AUDIO_INDEX;
			context->start_ref = context->mux_hdl->start_pts[index];
		}
	}
	if (ret < 0)
		OSCL_LOGE("muxer buffer handle failed!");

	return;
}

static void *muxer_component_buffer_manager(void *param)
{
	lb_omx_component_t *component;
	MuxerContext *context = NULL;
	OMX_COMPONENTTYPE *stand_comp = (OMX_COMPONENTTYPE *)param;
	OMX_BUFFERHEADERTYPE *inbuffer = NULL;
	base_port_t *video_port;
	base_port_t *audio_port;

	OSCL_TRACE(" %p\n", param);
	oscl_param_check((param != NULL), NULL, NULL);
	component = get_lb_component(stand_comp);
	context = component->component_private;
	OSCL_LOGI("======comp:%s", get_component_name(component));
	component->dbg_flag = set_debug_state(component->dbg_flag,
			DEBUG_MSG_SHT, DEBUG_THREAD_START);
	video_port = &component->port[VIDEO_INDEX];
	audio_port = &component->port[AUDIO_INDEX];

	context->start_ref = -1;
	while (component->state == OMX_StateIdle
		|| component->state == OMX_StateExecuting
		|| component->state == OMX_StatePause
		|| component->target_state == OMX_StateIdle) {

		/*Wait till the ports are being flushed*/
		pthread_mutex_lock(&component->flush_mutex);
		while (video_port->is_flushed || audio_port->is_flushed) {
			pthread_mutex_unlock(&component->flush_mutex);
			if (context->vbuffer != NULL)
				mux_context_return_buffer(context, context->vbuffer);
			if (context->abuffer != NULL)
				mux_context_return_buffer(context, context->abuffer);
			context->vbuffer = NULL;
			context->abuffer = NULL;
			sem_post(component->mgmt_flush_sem);
			sem_wait(component->flush_sem);
			inbuffer = NULL;
			pthread_mutex_lock(&component->flush_mutex);
		}
		pthread_mutex_unlock(&component->flush_mutex);

		if (component->state != OMX_StateExecuting) {
			context->start_ref = -1;
			context->intime[AUDIO_INDEX] = 0;
			context->intime[VIDEO_INDEX] = 0;
			context->sync_time = 0;
			inbuffer = NULL;
			sem_wait(component->buf_mgnt_sem);
			continue;
		}

		inbuffer = _component_receive_buffer(context);
		if (inbuffer == NULL) {
			oscl_sem_timedwait_ms(component->buf_mgnt_sem, 1000);
			continue;
		}

		pthread_mutex_lock(&context->context_mutex);
		if (context->mux_hdl) {
			if (_check_stop_muxer(component, inbuffer) ||
				(context->ctrl_flag & CONTEXT_STOP_MASK)) {
				if (inbuffer)
					OSCL_LOGE("stop_buf:%lld", inbuffer->nTimeStamp);
				stop_muxer_handle(context->mux_hdl);
				context->mux_hdl = NULL;
				if (context->ctrl_flag & CONTEXT_STOP_MASK)
					context->start_ref = -1;
			}
		}
		if (context->ctrl_flag & CONTEXT_FILENAME_MASK) {
			if (context->id != AV_MUXER_TYPE_RAW)
				_get_filename(context);
			context->ctrl_flag &= (~CONTEXT_FILENAME_MASK);
			OSCL_LOGE("context:%p nextfile %s", context, context->filename);
		}

		if (context->mux_hdl == NULL &&
			(context->ctrl_flag & CONTEXT_STOP_MASK) == 0)
			_start_new_muxer(context, inbuffer);
		pthread_mutex_unlock(&context->context_mutex);

		if (component->state == OMX_StateExecuting && inbuffer)
			(component->buf_handle)(stand_comp, inbuffer, NULL);

		if (context->colli_info == NULL)
			mux_context_return_buffer(context, inbuffer);
		else
			colli_cache_buffer(context->colli_info, inbuffer);
		if (inbuffer == context->abuffer)
			context->abuffer = NULL;
		if (inbuffer == context->vbuffer)
			context->vbuffer = NULL;
		inbuffer = NULL;
	}

	if ((context->vbuffer != NULL) && (!PORT_IS_SUPPLIER(video_port))) {
		OSCL_LOGE("inport return_buffer before exit");
		mux_context_return_buffer(context, context->vbuffer);
		context->vbuffer = NULL;
	}
	if ((context->abuffer != NULL) && (!PORT_IS_SUPPLIER(audio_port))) {
		OSCL_LOGE("inport return_buffer before exit");
		mux_context_return_buffer(context, context->abuffer);
		context->abuffer = NULL;
	}
	OSCL_LOGW("exit from muxer_buffer_manager:\n");
	OSCL_TRACE(" %p\n", param);
	component->dbg_flag = set_debug_state(component->dbg_flag,
			DEBUG_MSG_SHT, DEBUG_THREAD_EXIT);
	pthread_exit(NULL);
	return NULL;
}

static OMX_ERRORTYPE muxer_component_set_config(OMX_IN OMX_HANDLETYPE hcomp,
	OMX_IN OMX_INDEXTYPE cfg_index,
	OMX_IN OMX_PTR cfg_data)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	lb_omx_component_t *component;
	MuxerContext *context = NULL;
	OMX_TIME_CONFIG_TIMESTAMPTYPE *time;

	OSCL_TRACE(" %p, %d , %p\n", hcomp, cfg_index, cfg_data);
	oscl_param_check((hcomp != NULL) && (cfg_data != NULL),
		OMX_ErrorBadParameter, NULL);

	component = get_lb_component(hcomp);
	oscl_param_check((component != NULL), OMX_ErrorBadParameter, NULL);
	context = component->component_private;

	switch (cfg_index) {
	case omx_index_lombo_config_cur_time:
		time = cfg_data;
		if (time->nPortIndex == AUDIO_INDEX || time->nPortIndex == VIDEO_INDEX) {
			context->ref_time[time->nPortIndex] = time->nTimestamp;
			OSCL_LOGE("%d ref time:%lld", time->nPortIndex, time->nTimestamp);
		}
		break;
	default:
		ret = base_set_config(hcomp, cfg_index, cfg_data);
		break;
	}
	OSCL_TRACE(" %d\n", ret);

	return ret;
}

static OMX_ERRORTYPE muxer_component_get_config(OMX_IN OMX_HANDLETYPE hcomp,
	OMX_IN OMX_INDEXTYPE cfg_index,
	OMX_IN OMX_PTR cfg_data)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	lb_omx_component_t *component;
	MuxerContext *context = NULL;
	OMX_TIME_CONFIG_TIMESTAMPTYPE *cur_time;

	OSCL_TRACE(" %p, %d , %p\n", hcomp, cfg_index, cfg_data);
	oscl_param_check((hcomp != NULL) && (cfg_data != NULL),
		OMX_ErrorBadParameter, NULL);

	component = get_lb_component(hcomp);
	oscl_param_check((component != NULL), OMX_ErrorBadParameter, NULL);
	context = component->component_private;

	switch (cfg_index) {
	case omx_index_lombo_config_cur_time:
		cur_time = (OMX_TIME_CONFIG_TIMESTAMPTYPE *)cfg_data;
		cur_time->nTimestamp = muxer_handle_get_time(context->mux_hdl);
		if (cur_time->nTimestamp < 0)
			cur_time->nTimestamp = 0;
		break;
	default:
		ret = base_get_config(hcomp, cfg_index, cfg_data);
		break;
	}
	OSCL_TRACE(" %d\n", ret);

	return ret;
}

static OMX_ERRORTYPE muxer_component_set_parameter(OMX_IN OMX_HANDLETYPE hComp,
	OMX_IN OMX_INDEXTYPE paramIndex,
	OMX_IN OMX_PTR paramData)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	lb_omx_component_t *component;
	muxer_stream_cfg_t *stream_cfg;
	MuxerContext *context = NULL;
	AV_CodecParameters *codec_para;
	colli_muxfile_cfg_t *colli_para;
	OSCL_TRACE(" %p, %p , %x\n", hComp, paramData, paramIndex);

	oscl_param_check((hComp != NULL) && (paramData != NULL),
		OMX_ErrorBadParameter, NULL);
	component = get_lb_component(hComp);
	oscl_param_check((component != NULL), OMX_ErrorBadParameter, NULL);
	context = component->component_private;
	stream_cfg = &context->stream_cfg;

	if (component->state == OMX_StateExecuting) {
		if (paramIndex >= OMX_IndexAudioStartUnused &&
			paramIndex <= OMX_IndexAudioEndUnused)
			return OMX_ErrorIncorrectStateOperation;
		if (paramIndex == OMX_IndexParamPortDefinition)
			return OMX_ErrorIncorrectStateOperation;
		if (paramIndex == omx_index_vendor_output_filename)
			return OMX_ErrorIncorrectStateOperation;
	}

	OSCL_TRACE(" %x\n", paramIndex);
	switch (paramIndex) {
	case omx_index_vendor_set_max_duration: {
		pthread_mutex_lock(&context->context_mutex);
		context->file_total_time = (*((int *)paramData)) * 1000;
		OSCL_LOGE("context->file_total_time : %d\n", context->file_total_time);
		pthread_mutex_unlock(&context->context_mutex);
		break;
	}
	case omx_index_vendor_output_filename:
		pthread_mutex_lock(&context->context_mutex);
		if (context->filename != NULL) {
			oscl_free(context->filename);
			context->filename = NULL;
		}
		if (paramData != NULL)
			context->filename = oscl_strdup(paramData);
		pthread_mutex_unlock(&context->context_mutex);
		break;
	case OMX_IndexParamAudioPcm: {
		pthread_mutex_lock(&context->context_mutex);
		OSCL_TRACE(" %x\n", paramIndex);
		OMX_AUDIO_PARAM_PCMMODETYPE *audio_params =
			(OMX_AUDIO_PARAM_PCMMODETYPE *)paramData;
		stream_cfg->para[AUDIO_INDEX].codec_type = AV_MEDIA_TYPE_AUDIO;
		stream_cfg->para[AUDIO_INDEX].codec_id = AV_CODEC_TYPE_PCM_S16LE;
		stream_cfg->para[AUDIO_INDEX].sample_rate =
			audio_params->nSamplingRate;
		stream_cfg->para[AUDIO_INDEX].channels = audio_params->nChannels;
		stream_cfg->para[AUDIO_INDEX].channel_layout =
			audio_params->nChannels == 2 ? 3 : 4;
		stream_cfg->para[AUDIO_INDEX].frame_size =  BLKSIZE;
		stream_cfg->para[AUDIO_INDEX].block_align = BLKSIZE *
			audio_params->nChannels * 2;
		stream_cfg->exist[AUDIO_INDEX] = 1;
		pthread_mutex_unlock(&context->context_mutex);
	}
	break;
	case OMX_IndexParamAudioAdpcm: {
		pthread_mutex_lock(&context->context_mutex);
		OSCL_TRACE(" %x\n", paramIndex);
		OMX_AUDIO_PARAM_ADPCMTYPE *audio_params =
			(OMX_AUDIO_PARAM_ADPCMTYPE *)paramData;
		stream_cfg->para[AUDIO_INDEX].codec_type = AV_MEDIA_TYPE_AUDIO;
		stream_cfg->para[AUDIO_INDEX].codec_id = AV_CODEC_TYPE_ADPCM_IMA_WAV;
		stream_cfg->para[AUDIO_INDEX].sample_rate = audio_params->nSampleRate;
		stream_cfg->para[AUDIO_INDEX].channels = audio_params->nChannels;
		stream_cfg->para[AUDIO_INDEX].channel_layout =
			audio_params->nChannels == 2 ? 3 : 4;
		stream_cfg->para[AUDIO_INDEX].frame_size = (BLKSIZE  - 4) * 2 + 1;
		stream_cfg->para[AUDIO_INDEX].block_align = BLKSIZE *
			audio_params->nChannels;
		OSCL_TRACE(" %x\n", paramIndex);
		stream_cfg->exist[AUDIO_INDEX] = 1;
		pthread_mutex_unlock(&context->context_mutex);
	}

	break;
	case OMX_IndexParamAudioAac: {
		pthread_mutex_lock(&context->context_mutex);
		OSCL_TRACE(" %x\n", paramIndex);
		OMX_AUDIO_PARAM_AACPROFILETYPE *audio_params =
			(OMX_AUDIO_PARAM_AACPROFILETYPE *)paramData;
		stream_cfg->para[AUDIO_INDEX].codec_type = AV_MEDIA_TYPE_AUDIO;
		stream_cfg->para[AUDIO_INDEX].codec_id = AV_CODEC_TYPE_AAC;
		stream_cfg->para[AUDIO_INDEX].channels = audio_params->nChannels;
		stream_cfg->para[AUDIO_INDEX].sample_rate = audio_params->nSampleRate;
		OSCL_TRACE(" %x\n", paramIndex);
		stream_cfg->exist[AUDIO_INDEX] = 1;
		pthread_mutex_unlock(&context->context_mutex);
	}
	break;
	case OMX_IndexParamAudioG729: {
		pthread_mutex_lock(&context->context_mutex);
		OMX_AUDIO_PARAM_G729TYPE *audio_params;
		audio_params = (OMX_AUDIO_PARAM_G729TYPE *)paramData;
		stream_cfg->para[AUDIO_INDEX].codec_type = AV_MEDIA_TYPE_AUDIO;
		stream_cfg->para[AUDIO_INDEX].codec_id = AV_CODEC_TYPE_G729;
		stream_cfg->para[AUDIO_INDEX].channels = audio_params->nChannels;
		stream_cfg->para[AUDIO_INDEX].sample_rate = 8000;
		OSCL_TRACE(" %x\n", paramIndex);
		stream_cfg->exist[AUDIO_INDEX] = 1;
		pthread_mutex_unlock(&context->context_mutex);
	}
	break;
	/* case OMX_IndexParamVideoAvc: {
		OMX_VIDEO_PARAM_AVCTYPE *video_params =
			(OMX_VIDEO_PARAM_AVCTYPE *)paramData;
		muxer->para[VIDEO_INDEX].codec_type = AV_MEDIA_TYPE_VIDEO;
		muxer->para[VIDEO_INDEX].codec_id = AV_CODEC_TYPE_H264;
	}
	break; */
	case OMX_IndexParamPortDefinition: {
		OMX_PARAM_PORTDEFINITIONTYPE *port_def =
			(OMX_PARAM_PORTDEFINITIONTYPE *)paramData;
		OMX_VIDEO_PORTDEFINITIONTYPE *video;

		OSCL_TRACE(" %x\n", paramIndex);
		switch (port_def->eDomain) {
		case OMX_PortDomainVideo:
			pthread_mutex_lock(&context->context_mutex);
			video = &port_def->format.video;
			OSCL_LOGE("eCompressionFormat:%d, w:%d, h:%d, framerate:%d",
				video->eCompressionFormat,
				video->nFrameWidth,
				video->nFrameHeight,
				video->xFramerate);
			codec_para = &stream_cfg->para[VIDEO_INDEX];
			codec_para->codec_type = AV_MEDIA_TYPE_VIDEO;
			if (video->eCompressionFormat == OMX_VIDEO_CodingAVC)
				codec_para->codec_id = AV_CODEC_TYPE_H264;
			else if (video->eCompressionFormat == OMX_VIDEO_CodingHEVC)
				codec_para->codec_id = AV_CODEC_TYPE_HEVC;
			else if (video->eCompressionFormat == OMX_VIDEO_CodingMJPEG) {
				codec_para->codec_id = AV_CODEC_TYPE_MJPEG;
				stream_cfg->is_ready[VIDEO_INDEX] = 1;
			} else
				codec_para->codec_id = AV_CODEC_TYPE_INVALID;

			codec_para->width = video->nFrameWidth;
			codec_para->height = video->nFrameHeight;
			codec_para->sample_rate =
				video->xFramerate * VIDEOTIMEFRAME;
			if (codec_para->sample_rate == 0)
				codec_para->sample_rate = 25 * VIDEOTIMEFRAME;
			stream_cfg->exist[VIDEO_INDEX] = 1;
			pthread_mutex_unlock(&context->context_mutex);
			break;
		default:
			ret = OMX_ErrorBadParameter;
			break;
		}
		ret = base_set_parameter(hComp, paramIndex, paramData);
	}
	OSCL_TRACE(" %x\n", paramIndex);
	break;
	case omx_index_lombo_set_stream_callback:
		context->ext_writer = *((external_stream_writer_t *)paramData);
		OSCL_LOGE("set_stream_callback %p", context->ext_writer.write);
		break;
	case omx_index_lombo_set_muxer_format:
		context->id = *((int *)paramData);
		OSCL_LOGI("set_muxer_format %d", context->id);
		break;
	case omx_index_collision_enable:
		if (context->colli_info) {
			colli_deinit(context->colli_info);
			context->colli_info = NULL;
		}
		if (context->colli_info == NULL)
			context->colli_info = colli_init(context, (long)paramData);
		break;
	case omx_index_collision_disable:
		if (context->colli_info) {
			colli_deinit(context->colli_info);
			context->colli_info = NULL;
		}
		break;
	case omx_index_collision_start:
		ret = OMX_ErrorBadParameter;
		if (component->state == OMX_StateExecuting &&
			context->colli_info && paramData) {
			colli_para = (colli_muxfile_cfg_t *)paramData;
			ret = colli_start(context->colli_info, colli_para);
		}
		break;
	case omx_index_collision_stop:
		if (component->state == OMX_StateExecuting && context->colli_info)
			ret = colli_stop(context->colli_info);
		break;
	case omx_index_vendor_print_debug:
		_muxer_print_dbg(component);
		break;
	default:
		OSCL_TRACE(" %x\n", paramIndex);
		ret = base_set_parameter(hComp, paramIndex, paramData);
		break;
	}
	OSCL_TRACE(" %x\n", paramIndex);

	OSCL_TRACE(" %d\n", ret);
	return ret;
}

static OMX_ERRORTYPE muxer_component_set_state(OMX_HANDLETYPE hComp,
	OMX_U32 dest_state)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	lb_omx_component_t *component = NULL;
	MuxerContext *context = NULL;

	OMX_STATETYPE old_state;

	OSCL_TRACE("%p, %x\n", hComp, dest_state);
	oscl_param_check(hComp != NULL, OMX_ErrorBadParameter, NULL);
	component = get_lb_component(hComp);
	old_state = component->state;
	context = component->component_private;

	ret = base_component_set_state(hComp, dest_state);

	if ((old_state == OMX_StateExecuting) && (component->state == OMX_StateIdle)) {
		if (context->mux_hdl != NULL) {
			stop_muxer_handle(context->mux_hdl);
			context->mux_hdl = NULL;
		}
		mux_context_wait_idle(&context->mux_hdl_mgr);
	}
	return ret;
}

static OMX_ERRORTYPE muxer_component_deinit(OMX_IN OMX_HANDLETYPE hComponent)
{
	OMX_COMPONENTTYPE *base_cmp = (OMX_COMPONENTTYPE *)hComponent;
	lb_omx_component_t *component = NULL;
	MuxerContext *context = NULL;

	OSCL_TRACE("base_cmp_handle:%p\n", hComponent);
	oscl_param_check(hComponent != NULL, OMX_ErrorBadParameter, NULL);
	component = (lb_omx_component_t *)base_cmp->pComponentPrivate;
	context = component->component_private;

	mux_context_deinit(context);
	component->component_private = NULL;

	/* deinit port */
	component->port[AUDIO_INDEX].deinit(&component->port[AUDIO_INDEX]);
	component->port[VIDEO_INDEX].deinit(&component->port[VIDEO_INDEX]);
	base_component_deinit(hComponent);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE muxer_component_init(lb_omx_component_t *cmp_handle)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	MuxerContext *context = NULL;
	struct sched_param shed_param = {0};

	context = mux_context_init(cmp_handle);
	oscl_param_check_exit((context != NULL), OMX_ErrorInsufficientResources, NULL);

	ret = base_component_init(cmp_handle);

	pthread_attr_setstacksize(&cmp_handle->buf_thread_attr, 0x2000);
	pthread_attr_getschedparam(&cmp_handle->buf_thread_attr, &shed_param);
	OSCL_LOGI("priority:%d", shed_param.sched_priority);
	pthread_attr_setschedparam(&cmp_handle->buf_thread_attr, &shed_param);
	oscl_param_check_exit((ret == OMX_ErrorNone), ret, NULL);
	cmp_handle->name = "OMX.LB.SINK.MUXER";
	cmp_handle->component_private = context;
	cmp_handle->buf_manager = muxer_component_buffer_manager;
	cmp_handle->buf_handle = muxer_component_buffer_handle;
	cmp_handle->base_comp.ComponentDeInit = muxer_component_deinit;
	cmp_handle->base_comp.SetParameter = muxer_component_set_parameter;
	cmp_handle->base_comp.SetConfig = muxer_component_set_config;
	cmp_handle->base_comp.GetConfig = muxer_component_get_config;
	cmp_handle->do_state_set = muxer_component_set_state;

	cmp_handle->num_ports = 2;
	ret = base_port_init(cmp_handle, &cmp_handle->port[AUDIO_INDEX],
			AUDIO_INDEX,
			OMX_DirInput);
	if (ret != OMX_ErrorNone) {
		base_component_deinit(cmp_handle);
		return ret;
	}
	ret = base_port_init(cmp_handle, &cmp_handle->port[VIDEO_INDEX],
			VIDEO_INDEX,
			OMX_DirInput);
	if (ret != OMX_ErrorNone) {
		base_component_deinit(cmp_handle);
		cmp_handle->port[AUDIO_INDEX].deinit(
			&cmp_handle->port[AUDIO_INDEX]);
		return ret;
	}

	cmp_handle->port[AUDIO_INDEX].port_param.nBufferSize = SINK_INPUT_DEFAULT_SIZE;
	cmp_handle->port[AUDIO_INDEX].port_param.eDomain = OMX_PortDomainAudio;
	cmp_handle->port[AUDIO_INDEX].recive_buffer = muxer_port_recive_buffer;
	cmp_handle->port[AUDIO_INDEX].flush_buffer = muxer_port_flush_buffer;

	cmp_handle->port[VIDEO_INDEX].port_param.nBufferSize = OMX_VIDEO_BUF_SIZE;
	cmp_handle->port[VIDEO_INDEX].port_param.eDomain = OMX_PortDomainVideo;
	cmp_handle->port[VIDEO_INDEX].flush_buffer = muxer_port_flush_buffer;

EXIT:
	if (ret != OMX_ErrorNone) {
		if (context != NULL)
			mux_context_deinit(context);
	}
	OSCL_TRACE("%d\n", ret);
	return ret;
}

#ifdef OMX_DYNAMIC_LOADING
void *omx_component_init = muxer_component_init;
#endif

