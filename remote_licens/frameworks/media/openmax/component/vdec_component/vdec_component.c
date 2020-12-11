/*
 * vdec_component.c - Standard functionality for video decode component.
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

#define DBG_LEVEL         DBG_INFO
#define LOG_TAG           "vdec_component"

#include <limits.h>
#include <oscl.h>
#include <base_component.h>
#include <lombo_decplugin.h>
#include <lombo_al_lib.h>

#include <omx_api.h>
#include <av_media_type.h>
#include <demuxer_plugin.h>
#include <omx_vendor_lb.h>

#include "vdecoder.h"
#include "vdec_lib_handle.h"


#define DEFAULT_VDEC_BUFFER_ALIGN_SIZE	(4096)
#define DEFAULT_VDEC_BUFFER_NUM		(6)

typedef struct vdec_ctx {
	OMX_BOOL is_err_state;

	void *decoder; /* video decoder handle */
	pthread_t raw_buffer_thd;
	OMX_BOOL raw_buf_thd_running;

	int dec_err_cnt;
	OMX_BOOL is_decoding_flag;

	vdec_init_param_t param;

	pthread_cond_t flush_cond;
	pthread_mutex_t flush_lock;
	OMX_BOOL wait_flush;
	sem_t raw_buf_mgnt_sem;
	OMX_U32 file_index;
} vdec_ctx_t;

static inline int _put_raw_buffer(vdec_ctx_t *ctx, OMX_BUFFERHEADERTYPE *header)
{
	vdec_frame_t frame;
	frame.vir_addr = header->pBuffer;
	frame.phy_addr = oscl_virt_to_phys(frame.vir_addr);
	return vdecoder_queue_buf(ctx->decoder, &frame);
}

static inline int _put_raw_frame(vdec_ctx_t *ctx, vdec_frame_t *frame)
{
	return vdecoder_queue_buf(ctx->decoder, frame);
}

static void *raw_buffer_manager(void *param)
{
	lb_omx_component_t *component;
	vdec_ctx_t *ctx;
	base_port_t *outport;
	OMX_BUFFERHEADERTYPE *outbuffer = NULL;
	vdec_frame_t frame;
	int i;
	int ret = OMX_ErrorNone;

	oscl_param_check((param != NULL), NULL, NULL);
	prctl(PR_SET_NAME, "raw_buffer_manager");

	component = get_lb_component(param);
	ctx = (vdec_ctx_t *)component->component_private;
	outport = &component->port[OMX_DEFAULT_OUTPUT_PORT];
	frame.vir_addr = NULL;

	while (ctx->raw_buf_thd_running) {
		if (ctx->decoder == NULL || ctx->is_err_state) {
			oscl_mdelay(10);
			continue;
		}

		pthread_mutex_lock(&ctx->flush_lock);
		if (ctx->wait_flush) {
			if (frame.vir_addr != NULL) {
				OSCL_LOGI("queue buffer while flushing");
				_put_raw_frame(ctx, &frame);
				frame.vir_addr = NULL;
			}
			pthread_cond_signal(&ctx->flush_cond);
		}
		pthread_mutex_unlock(&ctx->flush_lock);

		if (component->state != OMX_StateExecuting) {
			ret = oscl_sem_timedwait_ms(&ctx->raw_buf_mgnt_sem, 200);
			if (ret == -1)
				OSCL_LOGD("raw_buf_mgnt_sem wait timeout!");
			continue;
		}

		if (frame.vir_addr == NULL) {
			memset(&frame, 0, sizeof(vdec_frame_t));
			ret = vdecoder_dequeue_buf(ctx->decoder, &frame, 1000);
			if (ret != VDEQBUF_OK) {
				OSCL_LOGW("dequeue buffer failed(ret=%d)!", ret);
				if ((frame.flags & AV_PACKET_FLAG_EOS)
					&& component->state == OMX_StateExecuting
					&& outport->header != NULL
					&& outport->header[0]->pBuffer != NULL) {
					frame.vir_addr = outport->header[0]->pBuffer;
					frame.width_stride = 0;
					frame.height_aligned = 0;
				}
				continue;
			}
			if (frame.vir_addr == NULL)
				OSCL_LOGE("vir_addr[NULL], flags=%x!", frame.flags);
			continue;
		}

		OSCL_LOGD("flags=%x ts=%lld, width_stride=%d, height_aligned=%d",
			frame.flags, frame.time_stamp,
			frame.width_stride, frame.height_aligned);

		for (i = 0; i < outport->num_assigned; i++) {
			if (outport->header[i]->pBuffer == frame.vir_addr)
				break;
		}
		if (i >= outport->num_assigned) {
			OSCL_LOGE("fatal error! can not find the output head");
			((component->callbacks.EventHandler))(&component->base_comp,
				component->callback_data, OMX_EventError, ret, -1, NULL);
			oscl_mdelay(10);
			continue;
		}

		outbuffer = outport->header[i];
		if (frame.width_stride == 0)
			outbuffer->nFilledLen = 0;
		else
			outbuffer->nFilledLen = outbuffer->nAllocLen;
		outbuffer->nTimeStamp = frame.time_stamp;
		outbuffer->nFlags = 0;

		if (frame.flags & AV_PACKET_FLAG_EOS) {
			OSCL_LOGI("EOS ts=%lld, width_stride=%d",
				frame.time_stamp, frame.width_stride);
			outbuffer->nFlags |= OMX_BUFFERFLAG_EOS;
		}
		if (frame.flags & AV_PACKET_FLAG_RESET) {
			OSCL_LOGI("start ts=%lld", frame.time_stamp);
			outbuffer->nFlags |= OMX_BUFFERFLAG_STARTTIME;
		}

		ret = outport->return_buffer(outport, outbuffer);
		if (ret != OMX_ErrorNone) {
			OSCL_LOGW("Failed to return buffer!");
			_put_raw_buffer(ctx, outbuffer);
		}
		if (outbuffer->nFlags & OMX_BUFFERFLAG_EOS) {
			(*(component->callbacks.EventHandler))(&component->base_comp,
				component->callback_data,
				OMX_EventBufferFlag, /* The command was completed */
				outport->port_param.nPortIndex,
				OMX_BUFFERFLAG_EOS,
				NULL);
		}
		frame.vir_addr = NULL;
	}

	if (frame.vir_addr != NULL && ctx->decoder != NULL) {
		OSCL_LOGW("return to codec before exit");
		_put_raw_frame(ctx, &frame);
	}
	oscl_queue_flush(&outport->buffer_queue);
	OSCL_LOGI("raw buffer thread exit");

	return NULL;
}

void vdec_buffer_handle(OMX_HANDLETYPE stand_com,
	OMX_BUFFERHEADERTYPE *inbuffer,
	OMX_BUFFERHEADERTYPE *outbuffer)
{
	int ret = 0;
	lb_omx_component_t *component = NULL;
	vdec_ctx_t *ctx = NULL;
	vdec_packet_t pkt;
	dmx_packet_t *dmx_pkt;

	component = get_lb_component(stand_com);
	ctx = (vdec_ctx_t *)component->component_private;
	dmx_pkt = (dmx_packet_t *)inbuffer->pBuffer;

	if (ctx->is_err_state) {
		inbuffer->nFilledLen = 0;
		return;
	}

	if (ctx->decoder == NULL) {
		base_port_t *outport = &component->port[OMX_DEFAULT_OUTPUT_PORT];
		int is_vdc = 0;

		ctx->decoder = vdecoder_open(ctx->param.vdc_init_parm.codec_id,
			&ctx->param, &is_vdc);
		if (ctx->decoder == NULL) {
			OSCL_LOGE("vdecoder_open error!");
			goto ERR_EXIT;
		}
		if (is_vdc == 0) {
			OSCL_LOGD("vdec");
			outport->port_param.nBufferAlignment = 1024;
		} else {
			OSCL_LOGD("vdc");
			outport->port_param.nBufferAlignment = 256;
		}
	}

	if (dmx_pkt->vir_addr == NULL && dmx_pkt->flags == 0) {
		OSCL_LOGD("NULL packet:size=%d,ts=%lld,type=%d,vir=0,phy=%x,flag=%08x",
			(int)dmx_pkt->data_size, (long long)dmx_pkt->pts,
			dmx_pkt->media_type, (unsigned int)dmx_pkt->phy_addr,
			dmx_pkt->flags);
		inbuffer->nFilledLen = 0;
		return;
	}

	memset(&pkt, 0, sizeof(vdec_packet_t));
	pkt.vir_addr = dmx_pkt->vir_addr;
	pkt.phy_addr  = dmx_pkt->phy_addr;
	pkt.len = dmx_pkt->data_size;
	pkt.ts = dmx_pkt->pts;
	pkt.type = AV_MEDIA_TYPE_VIDEO;
	pkt.flags = dmx_pkt->flags;

	ret = vdecoder_decode_frame(ctx->decoder, &pkt);
	if (ret == VDEC_HEAD_INFO) {
		vdec_info_t info;
		base_port_t *outport = &component->port[OMX_DEFAULT_OUTPUT_PORT];
		OMX_PARAM_PORTDEFINITIONTYPE *p_param = &outport->port_param;
		OMX_VIDEO_PORTDEFINITIONTYPE *v_param = &p_param->format.video;

		OSCL_LOGI("getting head info");
		ret = vdecoder_get_info(ctx->decoder, &info);
		if (ret != 0) {
			OSCL_LOGE("vdec get_info error!");
			goto ERR_EXIT;
		}
		v_param->nFrameWidth = info.crop.crop_width;
		v_param->nFrameHeight = info.crop.crop_height;
		v_param->nStride = info.pic_width;
		v_param->nSliceHeight = info.pic_height;
		p_param->nBufferSize = info.buf_size;
		p_param->nBufferCountActual = info.buf_num;

		(*(component->callbacks.EventHandler))(
			&component->base_comp,
			component->callback_data,
			OMX_EventPortSettingsChanged,
			OMX_DEFAULT_OUTPUT_PORT,
			ctx->file_index,
			NULL);
		ctx->dec_err_cnt = 0;
		return;
	} else if (ret == VDEC_BUF_NOT_ENOUGH) {
		OSCL_LOGW("no frame buffers");
		ctx->dec_err_cnt = 0;
		return;
	} else if (ret == VDEC_DISCARD_PACKET) {
		OSCL_LOGD("decoder drop packet(ts=%lld)!", pkt.ts);
		ctx->dec_err_cnt = 0;
	} else if (ret == VDEC_STREAM_NOT_SUPPORTED) {
		OSCL_LOGE("unsupported stream!");
		ret = OMX_ErrorUnsupportedVideo;
		goto ERR_EXIT;
	} else if (ret != VDEC_OK) {
		OSCL_LOGE("decode error(%d), size=%d, ts=%lld, err_cnt=%d",
			ret, pkt.len, pkt.ts, ctx->dec_err_cnt);
		ctx->dec_err_cnt++;
		if (ctx->dec_err_cnt > 10) {
			OSCL_LOGE("decode error event!");
			ret = OMX_ErrorDecodeVideo;
			goto ERR_EXIT;
		}
	} else {
		ctx->dec_err_cnt = 0;
	}

	inbuffer->nFilledLen = 0;
	return;

ERR_EXIT:
	inbuffer->nFilledLen = 0;
	ctx->is_err_state = OMX_TRUE;
	((component->callbacks.EventHandler)) (&component->base_comp,
		component->callback_data, OMX_EventError, ret, -1, NULL);
}

void *vdec_buffer_manager(void *param)
{
	lb_omx_component_t *component = NULL;
	vdec_ctx_t *ctx;
	base_port_t *inport = NULL;
	base_port_t *outport = NULL;
	OMX_BUFFERHEADERTYPE *inbuffer = NULL;
	OMX_S64 wait_buf_time = oscl_get_msec();

	OSCL_TRACE(" %p\n", param);
	oscl_param_check((param != NULL), NULL, NULL);
	component = get_lb_component(param);
	inport = &component->port[BASE_INPUT_PORT];
	outport = &component->port[OMX_DEFAULT_OUTPUT_PORT];
	ctx = (vdec_ctx_t *)component->component_private;

	while (1) {
		OMX_STATETYPE state = component->state;
		if (state != OMX_StateIdle
			&& state != OMX_StateExecuting
			&& state != OMX_StatePause) {
			OSCL_LOGD("comp-state=%d, state=%d, exit",
				component->state, state);
			break;
		}

		/* flush the ports if they are being flushed */
		pthread_mutex_lock(&component->flush_mutex);
		while (outport->is_flushed || inport->is_flushed) {
			pthread_mutex_unlock(&component->flush_mutex);
			if (inbuffer && inport->is_flushed) {
				inport->return_buffer(inport, inbuffer);
				inbuffer = NULL;
				OSCL_LOGI("retrun inbuffer");
			}
			sem_post(component->mgmt_flush_sem);
			sem_wait(component->flush_sem);
			OSCL_LOGW("flush, in %d, out %d",
				inport->buffer_queue.count, outport->buffer_queue.count);
			pthread_mutex_lock(&component->flush_mutex);
		}
		pthread_mutex_unlock(&component->flush_mutex);

		OSCL_LOGV("state:%d, sem value:%d", component->state,
			sem_get(component->buf_mgnt_sem));
		if (component->state != OMX_StateExecuting) {
			OSCL_LOGD("waiting for OMX_StateExecuting");
			sem_wait(component->buf_mgnt_sem);
			continue;
		}

		if (inbuffer == NULL) {
			inbuffer = oscl_queue_dequeue(&inport->buffer_queue);
			if (inbuffer == NULL) {
				OMX_S64 cur_time = oscl_get_msec();
				int ret;
				if (cur_time - wait_buf_time > 1000) {
					OSCL_LOGW("wait buffer for over %lld ms",
						(cur_time - wait_buf_time));
					wait_buf_time = cur_time;
				}
				ret = oscl_sem_timedwait_ms(
					component->buf_mgnt_sem,
					1000);
				if (ret == -1) {
					if (errno == ETIMEDOUT)
						OSCL_LOGE("wait timeout!");
					else {
						OSCL_LOGE("wait error(%s)!",
							strerror(errno));
					}
				}
				continue;
			}
			wait_buf_time = oscl_get_msec();
		}

		if (component->state == OMX_StateExecuting ||
			component->state == OMX_StatePause) {
			if (component->buf_handle) {
				ctx->is_decoding_flag = OMX_TRUE;
				((component->buf_handle))(component, inbuffer, NULL);
				ctx->is_decoding_flag = OMX_FALSE;
			} else {
				inbuffer->nFilledLen = 0;
			}
		}

		/* return inbuffer if it's consumed */
		if (inbuffer->nFilledLen == 0) {
			inport->return_buffer(inport, inbuffer);
			inbuffer = NULL;
		} else {
			oscl_mdelay(10);
		}
	}
	oscl_queue_flush(&inport->buffer_queue);
	OSCL_LOGW("exit, inport buffer left %d", inport->buffer_queue.count);
	OSCL_TRACE(" %p\n", param);
	return NULL;
}

OMX_ERRORTYPE vdec_fill_this_buffer(OMX_IN OMX_HANDLETYPE hComp,
	OMX_IN OMX_BUFFERHEADERTYPE *header)
{
	lb_omx_component_t *component;
	vdec_ctx_t *ctx;
	int ret;

	oscl_param_check(hComp != NULL, OMX_ErrorBadParameter, NULL);
	component = get_lb_component(hComp);
	ctx = (vdec_ctx_t *)component->component_private;
	ret = _put_raw_buffer(ctx, header);
	if (ret != 0)
		return OMX_ErrorUndefined;
	return OMX_ErrorNone;
}

static OMX_ERRORTYPE vdec_get_parameter(
	OMX_IN OMX_HANDLETYPE hComp,
	OMX_IN OMX_INDEXTYPE paramIndex,
	OMX_INOUT OMX_PTR paramData)
{
	lb_omx_component_t *component;
	vdec_ctx_t *ctx;
	OMX_ERRORTYPE ret = OMX_ErrorNone;

	oscl_param_check(hComp != NULL, OMX_ErrorBadParameter, NULL);
	component = get_lb_component(hComp);
	ctx = component->component_private;

	switch (paramIndex) {
	case omx_index_media_vdec_id: {
		*((OMX_U32 *)paramData) = ctx->file_index;
		break;
	}
	default:
		ret = base_get_parameter(hComp, paramIndex, paramData);
		if (ret != OMX_ErrorNone)
			OSCL_LOGE("base get parameter %d error", paramIndex);
		break;
	}

	return ret;
}

OMX_ERRORTYPE vdec_set_parameter(OMX_IN OMX_HANDLETYPE hComp,
	OMX_IN OMX_INDEXTYPE paramIndex,
	OMX_IN OMX_PTR paramData)
{
	lb_omx_component_t *component = NULL;
	vdec_ctx_t *ctx = NULL;
	OMX_ERRORTYPE ret = OMX_ErrorNone;

	oscl_param_check(hComp != NULL, OMX_ErrorBadParameter, NULL);
	component = get_lb_component(hComp);
	ctx = component->component_private;

	switch (paramIndex) {
	case omx_index_media_video_info: {
		omx_video_info_t *video_info = (omx_video_info_t *)paramData;
		ctx->param.vdc_init_parm.codec_id = video_info->codec_id;
		ctx->param.vdc_init_parm.extra_data = video_info->extradata;
		ctx->param.vdc_init_parm.extradata_len = video_info->extrasize;
		ctx->param.vdc_init_parm.src_width = video_info->width;
		ctx->param.vdc_init_parm.src_height = video_info->height;
		ctx->param.vdc_init_parm.error_handling_mode = ERROR_HANDLE_NORMAL;
		ctx->param.vdc_init_parm.output_mode = VDEC_YUV_SEMI420;
		ctx->param.vdc_init_parm.pp_out_width = 0;
		ctx->param.vdc_init_parm.pp_out_height = 0;
		OSCL_LOGI("dmx input: codec=%x, size(%dx%d), extradata=%p, extrasize=%d",
			ctx->param.vdc_init_parm.codec_id,
			ctx->param.vdc_init_parm.src_width,
			ctx->param.vdc_init_parm.src_height,
			ctx->param.vdc_init_parm.extra_data,
			ctx->param.vdc_init_parm.extradata_len);
		break;
	}
	case omx_index_media_vdec_id: {
		ctx->file_index = *((OMX_U32 *)paramData);
		break;
	}
	default:
		ret = base_set_parameter(hComp, paramIndex, paramData);
		if (ret != OMX_ErrorNone)
			OSCL_LOGE("base_set_parameter 0x%x error", paramIndex);
		break;
	}

	return ret;
}

OMX_ERRORTYPE vdec_set_state(OMX_HANDLETYPE hComp,
	OMX_U32 dest_state)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	lb_omx_component_t *component = NULL;
	vdec_ctx_t *ctx = NULL;
	OMX_STATETYPE pre_state;

	OSCL_TRACE(" %p, %x", hComp, dest_state);
	oscl_param_check(hComp != NULL, OMX_ErrorBadParameter, NULL);
	component = get_lb_component(hComp);
	ctx = component->component_private;

	OSCL_LOGI("%s->%s", omx_state_as_string(component->state),
		omx_state_as_string(dest_state));

	pre_state = component->state;
	ret = base_component_set_state(hComp, dest_state);

	if (dest_state == OMX_StateIdle && pre_state == OMX_StateLoaded) {
		ctx->is_err_state = OMX_FALSE;
		ctx->raw_buf_thd_running = OMX_TRUE;
		ret = pthread_create(&ctx->raw_buffer_thd, NULL,
			raw_buffer_manager, hComp);
		if (ret != 0) {
			OSCL_LOGE(" create raw_buffer_manager error!");
			return OMX_ErrorInsufficientResources;
		}
	}

	if (dest_state == OMX_StateIdle && pre_state == OMX_StateExecuting) {
		while (ctx->is_decoding_flag) {
			OSCL_LOGW("wait decode frame end");
			oscl_mdelay(100);
		}
	}
	if (dest_state == OMX_StateLoaded && pre_state == OMX_StateIdle) {
		sem_post(&ctx->raw_buf_mgnt_sem);
		OSCL_LOGI("joining raw buffer thread");
		ctx->raw_buf_thd_running = OMX_FALSE;
		if (ctx->decoder != NULL)
			vdecoder_ctrl(ctx->decoder, VDEC_CMD_EXIT_DEQUEUE_BUF, NULL);
		pthread_join(ctx->raw_buffer_thd, NULL);
		if (ctx->decoder != NULL) {
			OSCL_LOGI("close video decoder");
			vdecoder_close(ctx->decoder);
			ctx->decoder = NULL;
		}
	}
	if (dest_state == OMX_StateExecuting) {
		if (pre_state == OMX_StateIdle || pre_state == OMX_StatePause)
			sem_post(&ctx->raw_buf_mgnt_sem);
	}
	if (dest_state == OMX_StateInvalid) {
		if (ctx->raw_buf_thd_running) {
			ctx->raw_buf_thd_running = OMX_FALSE;
			sem_post(&ctx->raw_buf_mgnt_sem);
			vdecoder_ctrl(ctx->decoder, VDEC_CMD_EXIT_DEQUEUE_BUF, NULL);
			pthread_join(ctx->raw_buffer_thd, NULL);
		}
		if (ctx->decoder != NULL) {
			OSCL_LOGI("close video decoder");
			vdecoder_close(ctx->decoder);
			ctx->decoder = NULL;
		}
	}
	return ret;
}

OMX_ERRORTYPE vdec_allocate_buffer(OMX_IN OMX_HANDLETYPE cmp_hdl,
	OMX_INOUT OMX_BUFFERHEADERTYPE **header,
	OMX_IN OMX_U32 port_index,
	OMX_IN OMX_PTR data,
	OMX_IN OMX_U32 bytes)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	lb_omx_component_t *component;

	OSCL_LOGD(" %p %p %d %p %d\n", cmp_hdl, header, port_index, data, bytes);

	oscl_param_check(cmp_hdl != NULL, OMX_ErrorBadParameter, NULL);
	component = get_lb_component(cmp_hdl);
	oscl_param_check(component != NULL, OMX_ErrorBadParameter, NULL);
	oscl_param_check(port_index < component->num_ports, OMX_ErrorBadParameter, NULL);

	ret = base_allocate_buffer(cmp_hdl, header, port_index, data, bytes);
	oscl_param_check(ret == OMX_ErrorNone, OMX_ErrorInsufficientResources, "");

	return ret;
}

OMX_ERRORTYPE vdec_send_command(OMX_IN OMX_HANDLETYPE cmp_hdl,
	OMX_IN OMX_COMMANDTYPE cmd,
	OMX_IN OMX_U32 param1,
	OMX_IN OMX_PTR cmd_data)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	lb_omx_component_t *component;
	oscl_message_t message;
	vdec_ctx_t *ctx;
	OSCL_TRACE("%p, %x, %x , %p\n", cmp_hdl, cmd, param1, cmd_data);

	oscl_param_check(cmp_hdl != NULL, OMX_ErrorBadParameter, NULL);
	component = get_lb_component(cmp_hdl);
	oscl_param_check(component != NULL, OMX_ErrorBadParameter, NULL);
	oscl_param_check(component->state != OMX_StateInvalid,
		OMX_ErrorInvalidState, NULL);
	ctx = component->component_private;

	message.comp = component;
	message.msg_type = cmd;
	message.para1 = param1;
	message.data = cmd_data;
	OSCL_LOGD("%s message:%p msg_type:%x header:%p",
		component->name,
		&message, message.msg_type,
		component->port[0].header);
	switch (cmd) {
	case OMX_CommandStateSet:
		component->target_state = param1;
		ret = oscl_message_put(&component->msg_queue, &message);
		if (ret)
			return OMX_ErrorInsufficientResources;
		sem_post(component->msg_sem);
		break;
	case OMX_CommandFlush:
		if (ctx->decoder != NULL) {
			while (ctx->is_decoding_flag) {
				OSCL_LOGD("wait decode frame end");
				oscl_mdelay(10);
			}
			OSCL_LOGI("vdecoder_clear");
			ret = vdecoder_clear(ctx->decoder);
			if (ret != 0)
				OSCL_LOGE("vdecoder_clear error");
			pthread_mutex_lock(&ctx->flush_lock);
			sem_post(&ctx->raw_buf_mgnt_sem);
			ctx->wait_flush = OMX_TRUE;
			oscl_cond_timedwait_s(&ctx->flush_cond, &ctx->flush_lock, 2);
			ctx->wait_flush = OMX_FALSE;
			pthread_mutex_unlock(&ctx->flush_lock);
		}
		ret = base_send_command(cmp_hdl, cmd, param1, cmd_data);
		break;
	default:
		ret = base_send_command(cmp_hdl, cmd, param1, cmd_data);
		break;
	}

	OSCL_TRACE(" %d\n", ret);

	return ret;
}

OMX_ERRORTYPE vdec_component_deinit(OMX_IN OMX_HANDLETYPE hComponent)
{
	OMX_COMPONENTTYPE *base_cmp = (OMX_COMPONENTTYPE *)hComponent;
	lb_omx_component_t *component = NULL;
	vdec_ctx_t *ctx = NULL;
	OMX_ERRORTYPE ret = OMX_ErrorNone;

	OSCL_TRACE("base_cmp_handle:%p\n", hComponent);
	oscl_param_check(hComponent != NULL, OMX_ErrorBadParameter, NULL);
	component = (lb_omx_component_t *)base_cmp->pComponentPrivate;
	ctx = (vdec_ctx_t *)component->component_private;

	pthread_cond_destroy(&ctx->flush_cond);
	pthread_mutex_destroy(&ctx->flush_lock);
	sem_destroy(&ctx->raw_buf_mgnt_sem);

	base_port_deinit(&component->port[OMX_DEFAULT_INPUT_PORT]);
	base_port_deinit(&component->port[OMX_DEFAULT_OUTPUT_PORT]);
	oscl_free(ctx);
	ret = base_component_deinit(hComponent);

	return ret;
}

OMX_ERRORTYPE vdec_component_init(lb_omx_component_t *cmp_handle)
{
	vdec_ctx_t *ctx = NULL;
	base_port_t *inport, *outport;
	OMX_COMPONENTTYPE *base_cmp;
	OMX_ERRORTYPE ret = OMX_ErrorNone;

	memset(cmp_handle, 0, sizeof(lb_omx_component_t));
	base_cmp = &cmp_handle->base_comp;

	ctx = oscl_zalloc(sizeof(vdec_ctx_t));
	if (!ctx) {
		OSCL_LOGE("alloc vdec_ctx_t error!\n");
		return OMX_ErrorInsufficientResources;
	}

	pthread_cond_init(&ctx->flush_cond, NULL);
	pthread_mutex_init(&ctx->flush_lock, NULL);
	sem_init(&ctx->raw_buf_mgnt_sem, 0, 0);

	/* init component */
	ret = base_component_init(cmp_handle);
	if (ret != OMX_ErrorNone) {
		OSCL_LOGE("base_component_init error!\n");
		goto ERROR1;
	}
	cmp_handle->name = "OMX.LB.VIDEO.DEC";
	cmp_handle->component_private = ctx;
	cmp_handle->buf_manager = vdec_buffer_manager;
	cmp_handle->buf_handle = vdec_buffer_handle;
	base_cmp->ComponentDeInit = vdec_component_deinit;
	base_cmp->GetParameter = vdec_get_parameter;
	base_cmp->SetParameter = vdec_set_parameter;
	base_cmp->AllocateBuffer = vdec_allocate_buffer;
	base_cmp->FillThisBuffer = vdec_fill_this_buffer;
	base_cmp->SendCommand = vdec_send_command;
	cmp_handle->do_state_set = vdec_set_state;
	cmp_handle->num_ports = 2;
	/* cmp_handle->log_level = DBG_INFO; */

	inport = &cmp_handle->port[OMX_DEFAULT_INPUT_PORT];
	/* init input & output port */
	ret = base_port_init(cmp_handle, inport, OMX_DEFAULT_INPUT_PORT, OMX_DirInput);
	if (ret != OMX_ErrorNone) {
		OSCL_LOGE(" input base_port_init error!\n");
		goto ERROR2;
	}
	inport->port_param.eDomain = OMX_PortDomainVideo;
	/* inport->log_level = DBG_INFO; */

	outport = &cmp_handle->port[OMX_DEFAULT_OUTPUT_PORT];
	ret = base_port_init(cmp_handle, outport, OMX_DEFAULT_OUTPUT_PORT, OMX_DirOutput);
	if (ret != OMX_ErrorNone) {
		OSCL_LOGE(" input base_port_init error!\n");
		goto ERROR3;
	}
	outport->port_param.eDomain = OMX_PortDomainVideo;
	if (vdecoder_check() == 0)
		outport->port_param.nBufferAlignment = 256;
	else
		outport->port_param.nBufferAlignment = 1024;
	outport->port_param.nBufferCountActual = DEFAULT_VDEC_BUFFER_NUM;
	outport->port_param.bBuffersContiguous = 1;
	/* outport->log_level = DBG_INFO; */

	return ret;

ERROR3:
	base_port_deinit(inport);
ERROR2:
	ret = base_component_deinit(&cmp_handle->base_comp);
ERROR1:
	if (ctx != NULL)
		oscl_free(ctx);
	OSCL_TRACE("%d arec:%p\n", ret, cmp_handle);

	return ret;
}

#ifdef OMX_DYNAMIC_LOADING
void *omx_component_init = vdec_component_init;
#endif

