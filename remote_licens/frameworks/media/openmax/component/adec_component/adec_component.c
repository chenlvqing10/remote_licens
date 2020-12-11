/*
 * adec_component.c - demo code for adec component.
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
#define LOG_TAG           "adec_component"

#include <oscl.h>
#include <base_component.h>
#include <demuxer_plugin.h>
#include "adec_common.h"
#include <demuxer_plugin.h>
#include <omx_vendor_lb.h>

/* Place global headers in extradata instead of every keyframe */
#define AV_CODEC_FLAG_GLOBAL_HEADER   (1 << 22)

void adec_buffer_handle(OMX_HANDLETYPE hComp,
	OMX_BUFFERHEADERTYPE *inbuf, OMX_BUFFERHEADERTYPE *outbuf)
{
	lb_omx_component_t *component = NULL;
	adec_private_t *priv = NULL;
	AV_CodecParameters *para;
	audio_packet_t *apkt;
	int ret = 0;

	component = get_lb_component(hComp);
	priv = component->component_private;
	para = priv->paudio.para;
	apkt = &priv->pkt;

	if (priv->is_err_state) {
		apkt->size = 0;
		inbuf->nFilledLen = 0;
		return;
	}

	if (priv->fmt_changed && priv->adec_opened) {
		OSCL_LOGI("closing adec");
		adec_close(priv);
		priv->adec_opened = 0;
		priv->fmt_changed = 0;
	}

	if (priv->adec_opened == 0) {
		OSCL_LOGI("opening adec");
		ret = adec_open(priv);
		if (ret < 0) {
			OSCL_LOGE("open adec error(%d)!", ret);
			goto ERR_EXIT;
		}
		priv->adec_opened = 1;
		adec_flush(priv);
		if (priv->extradata != NULL) {
			if (para->extradata != NULL) {
				oscl_free(para->extradata);
				para->extradata = NULL;
			}
			para->extradata = oscl_malloc(priv->extrasize);
			if (para->extradata == NULL) {
				OSCL_LOGE("extradata malloc fail!!!!!!!!");
				ret = -1;
				goto ERR_EXIT;
			}
			memcpy(para->extradata, priv->extradata, priv->extrasize);
			para->extradata_size = priv->extrasize;
			priv->extradata = NULL;
			priv->extrasize = 0;
		}
	}

	if (inbuf->nFlags & OMX_BUFFERFLAG_CODECCONFIG) {
		OSCL_LOGI("extradata_size=%d, pkt len=%ld",
			para->extradata_size, (long)apkt->size);
		if (apkt->size > 0) {
			if (para->extradata != NULL) {
				oscl_free(para->extradata);
				para->extradata = NULL;
			}
			para->extradata = oscl_malloc(apkt->size);
			if (para->extradata == NULL) {
				OSCL_LOGE("extradata malloc fail!!!!!!!!");
				ret = -1;
				goto ERR_EXIT;
			}
			memcpy(para->extradata, apkt->data, apkt->size);
			para->extradata_size = apkt->size;
		}
		apkt->size = 0;
		inbuf->nFilledLen = 0;
		return;
	}

	if (apkt->flags & AV_PACKET_FLAG_RESET) {
		/* adec_flush(priv); */
		OSCL_LOGI("start timestamp %lld", (long long)apkt->timestamp);
		outbuf->nFlags |= OMX_BUFFERFLAG_STARTTIME;
		apkt->flags &= ~AV_PACKET_FLAG_RESET;
	}
	if (apkt->flags & AV_PACKET_FLAG_EOS) {
		OSCL_LOGI("eos detected: size=%ld, ts=%lld, buflen=%d",
			(long)apkt->size, (long long)apkt->timestamp, priv->buf.len);
		outbuf->nFlags |= OMX_BUFFERFLAG_EOS;
	}

	if (apkt->size == 0) {
		OSCL_LOGD("buf len %d, pkt len 0!!", priv->buf.len);
		if (priv->buf.len == 0 || (priv->buf.len != 0 && priv->underflow)) {
			inbuf->nFilledLen = 0;
			return;
		}
	}

	priv->underflow = 0;
	ret = adec_frame(priv, inbuf, outbuf);
	if (ret == DEC_UNDERFLOW) {
		OSCL_LOGD("adec_frame underflow");
		priv->underflow = 1;
	} else if (ret < 0)
		OSCL_LOGW("adec_frame error, ret=%d", ret);
	omx_audio_info_t *info = &priv->info;
	if (para->bits_per_raw_sample == 0)
		para->bits_per_raw_sample = 16;
	if (para->sample_rate != info->sample_rate ||
		para->channels != info->channels ||
		para->bits_per_raw_sample != info->bitspersample) {
		OSCL_LOGI("audio output parameter change, sr=%d, ch=%d, bps=%d, bpcs=%d",
			para->sample_rate, para->channels,
			para->bits_per_raw_sample, para->bits_per_coded_sample);
		info->sample_rate = para->sample_rate;
		info->channels = para->channels;
		info->bitspersample = para->bits_per_raw_sample;
		/* notify audio format changed */
		(*(component->callbacks.EventHandler))(
			&component->base_comp,
			component->callback_data,
			OMX_EventAudioParamChanged,
			0,
			0,
			info);
	}

	return;

ERR_EXIT:
	priv->is_err_state = 1;
	((component->callbacks.EventHandler))(&component->base_comp,
		component->callback_data, OMX_EventError, ret, -1, NULL);
	return;
}

/*
 * This is the central function for component processing. It
 * is executed in a separate thread, is synchronized with
 * semaphores at each port, those are released each time a new buffer
 * is available on the given port.
 */
void *adec_buffer_manager(void *param)
{
	lb_omx_component_t *component;
	base_port_t *inport;
	base_port_t *outport;
	OMX_BUFFERHEADERTYPE *outbuffer = NULL;
	OMX_BUFFERHEADERTYPE *inbuffer = NULL;

	OSCL_TRACE(" %p\n", param);
	oscl_param_check((param != NULL), NULL, NULL);
	component = get_lb_component(param);

	inport = &component->port[BASE_INPUT_PORT];
	outport = &component->port[BASE_OUTPUT_PORT];
	component->dbg_flag = set_debug_state(component->dbg_flag,
			DEBUG_BUF_MGNT_SHT, DEBUG_THREAD_START);
	OSCL_LOGI("%s buffer_manager", component->name);

	/* checks if the component is in a state able to receive buffers */
	while (1) {
		OMX_STATETYPE state = component->state;
		if (state != OMX_StateIdle
			&& state != OMX_StateExecuting
			&& state != OMX_StatePause) {
			OSCL_LOGD("comp-state=%d, state=%d, exit",
				component->state, state);
			break;
		}
		/* Wait till the ports are being flushed */
		component->dbg_wdog = 0;
		pthread_mutex_lock(&component->flush_mutex);
		while (inport->is_flushed || outport->is_flushed) {
			pthread_mutex_unlock(&component->flush_mutex);
			OSCL_LOGW("component:%p,flush buffers:%d %d\n",
				component, inport->is_flushed, outport->is_flushed);
			OSCL_LOGW("inport:%d, buffer:%p, outport:%d, buffer:%p",
				inport->port_param.nPortIndex, inbuffer,
				outport->port_param.nPortIndex, outbuffer);

			if (outbuffer && outport->is_flushed) {
				int ret = outport->return_buffer(outport, outbuffer);
				if (ret != 0) {
					OSCL_LOGW("return outbuf fail");
					outport->recive_buffer(outport, outbuffer);
				}
				outbuffer = NULL;
			}
			if (inbuffer && inport->is_flushed) {
				inport->return_buffer(inport, inbuffer);
				inbuffer = NULL;
			}
			sem_post(component->mgmt_flush_sem);
			sem_wait(component->flush_sem);
			OSCL_LOGW("%s flush in buffer manager thread\n",
				get_component_name(component));
			pthread_mutex_lock(&component->flush_mutex);
			OSCL_LOGI("out buffer count %d, inport buf cnt %d\n",
				outport->buffer_queue.count,
				inport->buffer_queue.count);
		}
		pthread_mutex_unlock(&component->flush_mutex);

		if (component->state != OMX_StateExecuting) {
			OSCL_LOGD("out buffer count %d, sem %d",
				outport->buffer_queue.count,
				sem_get(component->buf_mgnt_sem));
			sem_wait(component->buf_mgnt_sem);
			continue;
		}

		if (inbuffer == NULL) {
			inbuffer = oscl_queue_dequeue(&inport->buffer_queue);
			if (inbuffer != NULL) {
				dmx_packet_t *dmx_pkt = (dmx_packet_t *)inbuffer->pBuffer;
				adec_private_t *priv = component->component_private;
				OSCL_LOGD("ts=%lld, size=%d",
					dmx_pkt->pts, dmx_pkt->data_size);
				priv->pkt.data = dmx_pkt->vir_addr;
				priv->pkt.size = dmx_pkt->data_size;
				priv->pkt.flags = dmx_pkt->flags;
				priv->pkt.timestamp = dmx_pkt->pts;
				priv->pkt.offset = 0;
			}
		}
		if (outbuffer == NULL) {
			outbuffer = oscl_queue_dequeue(&outport->buffer_queue);
			if (outbuffer != NULL) {
				outbuffer->nFilledLen = 0;
				outbuffer->nOffset = 0;
				outbuffer->nFlags = 0;
			}
		}

		base_check_eos(component, outport, outbuffer);
		if ((inbuffer == NULL) || outbuffer == NULL) {
			OSCL_LOGD("waiting buffer:%p %p, sem %d",
				inbuffer, outbuffer,
				sem_get(component->buf_mgnt_sem));
			sem_wait(component->buf_mgnt_sem);
			while (sem_trywait(component->buf_mgnt_sem) == 0)
				continue;
			continue;
		}

		if (component->state == OMX_StateExecuting) {
			if (component->buf_handle) {
				adec_private_t *priv = component->component_private;
				priv->is_decoding_flag = 1;
				((component->buf_handle))(component, inbuffer, outbuffer);
				priv->is_decoding_flag = 0;
			}
		}

		if (outbuffer) {
			if ((outbuffer->nFilledLen != 0)
				|| (outbuffer->nFlags & OMX_BUFFERFLAG_EOS)
				|| (outbuffer->nFlags & OMX_BUFFERFLAG_STARTTIME)) {
				OSCL_LOGD("return outbuf len %d, ts=%lld",
					outbuffer->nFilledLen, outbuffer->nTimeStamp);
				int ret = outport->return_buffer(outport, outbuffer);
				if (ret != 0) {
					OSCL_LOGW("return outbuf fail");
					outport->recive_buffer(outport, outbuffer);
				}
				outbuffer = NULL;
			} else {
				OSCL_LOGD("outbuffer not filled, len %d, flag 0x%x\n",
					outbuffer->nFilledLen, outbuffer->nFlags);
				outport->recive_buffer(outport, outbuffer);
				outbuffer = NULL;
			}
		}
		if (inbuffer) {
			if (inbuffer->nFilledLen == 0) {
				OSCL_LOGD("return inbuf %p\n", inbuffer);
				inport->return_buffer(inport, inbuffer);
				inbuffer = NULL;
			}
		}
	}
	oscl_queue_flush(&inport->buffer_queue);
	oscl_queue_flush(&outport->buffer_queue);
	OSCL_LOGW("exit from buffer_manager:%s\n", component->name);
	OSCL_LOGW("exit, inport buffer left %d, out buffer left %d\n",
		inport->buffer_queue.count, outport->buffer_queue.count);
	OSCL_TRACE(" %p\n", param);
	component->dbg_flag = set_debug_state(component->dbg_flag,
			DEBUG_BUF_MGNT_SHT, DEBUG_THREAD_EXIT);

	return NULL;
}

static OMX_ERRORTYPE adec_get_parameter(
	OMX_IN OMX_HANDLETYPE hComp,
	OMX_IN OMX_INDEXTYPE paramIndex,
	OMX_INOUT OMX_PTR paramData)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	lb_omx_component_t *component = NULL;
	adec_private_t *audio_private = NULL;
	OSCL_TRACE(" %p, %p , %d\n", hComp, paramData, paramIndex);

	oscl_param_check((hComp != NULL) && (paramData != NULL),
		OMX_ErrorBadParameter, NULL);
	component = get_lb_component(hComp);
	oscl_param_check((component != NULL), OMX_ErrorBadParameter, NULL);
	audio_private = component->component_private;

	switch ((OMX_U32)paramIndex) {
	case OMX_IndexParamAudioPcm: {
		OMX_AUDIO_PARAM_PCMMODETYPE *audio_params;
		audio_params = (OMX_AUDIO_PARAM_PCMMODETYPE *)paramData;
		audio_params->nChannels = audio_private->paudio.para->channels;
		audio_params->nBitPerSample = 16;
		audio_params->nSamplingRate = audio_private->paudio.para->sample_rate;
	}
	break;
	default:
		ret = base_get_parameter(hComp, paramIndex, paramData);
		if (ret != OMX_ErrorNone)
			OSCL_LOGE("base get parameter %d error\n", paramIndex);
		break;
	}

	return ret;
}

OMX_ERRORTYPE adec_set_parameter(OMX_IN OMX_HANDLETYPE hComp,
	OMX_IN OMX_INDEXTYPE paramIndex,
	OMX_IN OMX_PTR paramData)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	lb_omx_component_t *component = NULL;
	adec_private_t *audio_private = NULL;
	OSCL_TRACE(" %p, %p , %d\n", hComp, paramData, paramIndex);

	oscl_param_check((hComp != NULL) && (paramData != NULL),
		OMX_ErrorBadParameter, NULL);
	component = get_lb_component(hComp);
	oscl_param_check((component != NULL), OMX_ErrorBadParameter, NULL);
	audio_private = component->component_private;

	switch (paramIndex) {
	case omx_index_vendor_input_filename:
		break;
	case omx_index_media_audio_info: {
		omx_audio_info_t *ainfo = (omx_audio_info_t *)paramData;
		audio_private->paudio.para->codec_id = ainfo->codec_id;
		audio_private->paudio.para->bit_rate = ainfo->bit_rate;
		audio_private->paudio.para->sample_rate = ainfo->sample_rate;
		audio_private->paudio.para->channels = ainfo->channels;
		audio_private->paudio.para->channel_layout = ainfo->channel_layout;
		audio_private->paudio.para->bits_per_coded_sample =
			ainfo->bitspersample;
		audio_private->paudio.para->frame_size = ainfo->frame_size;
		audio_private->paudio.para->block_align = ainfo->block_align;
		audio_private->extradata = ainfo->extradata;
		audio_private->extrasize = ainfo->extradata_size;

		OSCL_LOGI("audio fmt change, codec(old=%x new=%x)",
			audio_private->info.codec_id,
			ainfo->codec_id);
		audio_private->fmt_changed = 1;

		audio_private->info = *ainfo;
		break;
	}
	default:
		ret = base_set_parameter(hComp, paramIndex, paramData);
		break;
	}

	OSCL_TRACE(" %d\n", ret);
	return ret;
}

OMX_ERRORTYPE adec_send_command(OMX_IN OMX_HANDLETYPE cmp_hdl,
	OMX_IN OMX_COMMANDTYPE cmd,
	OMX_IN OMX_U32 param1,
	OMX_IN OMX_PTR cmd_data)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	lb_omx_component_t *component;
	adec_private_t *priv;
	OSCL_TRACE("%p, %x, %x , %p\n", cmp_hdl, cmd, param1, cmd_data);

	oscl_param_check(cmp_hdl != NULL, OMX_ErrorBadParameter, NULL);
	component = get_lb_component(cmp_hdl);
	oscl_param_check(component != NULL, OMX_ErrorBadParameter, NULL);
	oscl_param_check(component->state != OMX_StateInvalid,
		OMX_ErrorInvalidState, NULL);
	priv = component->component_private;

	switch (cmd) {
	case OMX_CommandFlush:
		OSCL_LOGI("flash command");
		memset(&priv->pkt, 0, sizeof(audio_packet_t));
		if (priv->adec_opened)
			adec_flush(priv);
		ret = base_send_command(cmp_hdl, cmd, param1, cmd_data);
		break;
	default:
		ret = base_send_command(cmp_hdl, cmd, param1, cmd_data);
		break;
	}

	OSCL_TRACE(" %d\n", ret);

	return ret;
}

OMX_ERRORTYPE adec_set_state(OMX_HANDLETYPE hComp,
	OMX_U32 dest_state)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	lb_omx_component_t *component;
	adec_private_t *audio_private;
	OMX_STATETYPE pre_state;

	OSCL_TRACE(" %p, %x\n", hComp, dest_state);
	oscl_param_check(hComp != NULL, OMX_ErrorBadParameter, NULL);
	component = get_lb_component(hComp);
	audio_private = component->component_private;

	OSCL_LOGI("%s->%s", omx_state_as_string(component->state),
		omx_state_as_string(dest_state));

	if (dest_state == OMX_StateIdle && component->state == OMX_StateLoaded) {
		audio_private->adec_opened = 0;
		audio_private->fmt_changed = 0;
		audio_private->is_err_state = 0;
	}

	pre_state = component->state;
	ret = base_component_set_state(hComp, dest_state);
	if (dest_state == OMX_StateLoaded && pre_state == OMX_StateIdle) {
		if (audio_private->adec_opened) {
			OSCL_LOGI("closing adec");
			adec_close(audio_private);
		}
		audio_private->adec_opened = 0;
	}

	if (dest_state == OMX_StatePause && pre_state == OMX_StateExecuting) {
		while (audio_private->is_decoding_flag) {
			OSCL_LOGD("wait decode frame end");
			oscl_mdelay(10);
		}
	}

	return ret;
}

/*
 * arec_recive_buffer the entry point for sending buffers to the port
 *
 * This function can be called by the EmptyThisBuffer or FillThisBuffer. It depends on
 * the nature of the port, that can be an input or output port.
 */
#if 0
OMX_ERRORTYPE arec_recive_buffer(base_port_t *port,
	OMX_BUFFERHEADERTYPE *buffer)
{
	OMX_U32 port_idx;
	lb_omx_component_t *component = (lb_omx_component_t *)port->component;
	adec_private_t *audio_private = (adec_private_t *)component->component_private;

	OSCL_TRACE("%p:%p", port, buffer);
	port_idx = (port->port_param.eDir == OMX_DirInput) ? buffer->nInputPortIndex :
		buffer->nOutputPortIndex;
	oscl_param_check((port_idx == port->port_param.nPortIndex),
		OMX_ErrorBadPortIndex, NULL);
	oscl_param_check((OMX_StateInvalid != get_component_state(port)),
		OMX_ErrorInvalidState, NULL);
	oscl_param_check(((OMX_StateExecuting == get_component_state(port))
			|| (OMX_StatePause == get_component_state(port))
			|| (OMX_StateIdle == get_component_state(port))
			|| (!PORT_IS_ENABLED(port))
			|| (!(port->is_flushed))),
		OMX_ErrorIncorrectStateOperation, "IncorrectState");
	if (!PORT_IS_SUPPLIER(port) && port->is_flushed) {
		OSCL_LOGW("recive buffers while flush port(none supplier)\n");
		OSCL_LOGW("%p(%s, %d)", port,
			get_port_name(port), port->port_param.nPortIndex);
	}
	if (buffer) {
		while (buffer->nFilledLen) {
			if (buffer->nFilledLen && buffer->pBuffer) {
				int ret = writePcmData(buffer->pBuffer + buffer->nOffset,
						buffer->nFilledLen,
						audio_private->pcm_manager);
				if (ret > 0)
					buffer->nFilledLen -= ret;
			}
			if (buffer->nFlags & OMX_BUFFERFLAG_EOS) {
				OSCL_LOGE("adv_buf get the end of stream\n");
				audio_private->pcm_manager->nFlags |= OMX_BUFFERFLAG_EOS;
			}
		}
		port->return_buffer(port, buffer);
		buffer = NULL;
	}

#if 0
	/* notify the buffer management thread we have a fresh new buffer to manage*/
	oscl_queue_queue(&port->buffer_queue, buffer);
	if (PORT_IS_SUPPLIER(port) && (port->is_flushed))
		sem_post(&port->buffer_sem);
	sem_post(component->buf_mgnt_sem);
#endif
	OSCL_TRACE("%p:%p", port, buffer);

	return OMX_ErrorNone;
}
#endif

OMX_ERRORTYPE adec_component_deinit(OMX_IN OMX_HANDLETYPE hComp)
{
	OMX_COMPONENTTYPE *base_cmp = (OMX_COMPONENTTYPE *)hComp;
	lb_omx_component_t *component = NULL;
	adec_private_t *audio_private = NULL;
	int ret;

	OSCL_TRACE("base_cmp_handle:%p\n", hComp);
	oscl_param_check(hComp != NULL, OMX_ErrorBadParameter, NULL);
	component = (lb_omx_component_t *)base_cmp->pComponentPrivate;
	audio_private = (adec_private_t *)component->component_private;

	component->port[OMX_DEFAULT_INPUT_PORT].deinit(
		&component->port[OMX_DEFAULT_INPUT_PORT]);
	component->port[OMX_DEFAULT_OUTPUT_PORT].deinit(
		&component->port[OMX_DEFAULT_OUTPUT_PORT]);

	if (audio_private) {
		adec_deinit(audio_private);
		component->component_private = NULL;
	}
	ret = base_component_deinit(hComp);

	return ret;
}

OMX_ERRORTYPE adec_component_init(lb_omx_component_t *cmp_handle)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	adec_private_t *audio_private = NULL;
	struct sched_param shed_param = {0};
	OMX_COMPONENTTYPE *base_cmp = &(cmp_handle->base_comp);

	audio_private = adec_init();
	if (!audio_private) {
		OSCL_LOGE("audio_private error!\n");
		return OMX_ErrorInsufficientResources;
	}

	OSCL_TRACE("adec_component_init:%p\n", cmp_handle);
	ret = base_component_init(cmp_handle);
	if (ret != OMX_ErrorNone)
		return ret;

	/* pthread prrority must high to other */
	pthread_attr_getschedparam(&cmp_handle->buf_thread_attr, &shed_param);
	OSCL_LOGI("priority:%d", shed_param.sched_priority);
	/* shed_param.sched_priority++; */
	pthread_attr_setschedparam(&cmp_handle->buf_thread_attr, &shed_param);
	pthread_attr_setstacksize(&cmp_handle->buf_thread_attr, 0x4000);

	/* port init */
	cmp_handle->num_ports = 2;
	ret = base_port_init(cmp_handle, &cmp_handle->port[OMX_DEFAULT_INPUT_PORT],
			OMX_DEFAULT_INPUT_PORT,
			OMX_DirInput);
	if (ret != OMX_ErrorNone) {
		base_component_deinit(cmp_handle);
		return ret;
	}
	ret = base_port_init(cmp_handle, &cmp_handle->port[OMX_DEFAULT_OUTPUT_PORT],
			OMX_DEFAULT_OUTPUT_PORT,
			OMX_DirOutput);
	if (ret != OMX_ErrorNone) {
		base_component_deinit(cmp_handle);
		cmp_handle->port[OMX_DEFAULT_INPUT_PORT].deinit(
			&cmp_handle->port[OMX_DEFAULT_INPUT_PORT]);
		return ret;
	}
	cmp_handle->port[OMX_DEFAULT_INPUT_PORT].port_param.nBufferSize =
		OMX_DEFAULT_BUF_SIZE;
	cmp_handle->port[OMX_DEFAULT_INPUT_PORT].port_param.eDomain =
		OMX_PortDomainAudio;

	cmp_handle->port[OMX_DEFAULT_OUTPUT_PORT].port_param.nBufferSize =
		OMX_DEFAULT_BUF_SIZE;
	cmp_handle->port[OMX_DEFAULT_OUTPUT_PORT].port_param.eDomain =
		OMX_PortDomainAudio;
	cmp_handle->port[OMX_DEFAULT_OUTPUT_PORT].port_param.nBufferCountActual = 16;

	cmp_handle->name = "OMX.LB.SOURCE.ADEC";
	cmp_handle->component_private = audio_private;
	cmp_handle->base_comp.ComponentDeInit = adec_component_deinit;
	cmp_handle->buf_manager = adec_buffer_manager;
	cmp_handle->buf_handle = adec_buffer_handle;
	cmp_handle->base_comp.SetParameter = adec_set_parameter;
	cmp_handle->base_comp.GetParameter = adec_get_parameter;
	cmp_handle->do_state_set = adec_set_state;

	base_cmp->SendCommand = adec_send_command;

	OSCL_TRACE("adec_component_init:%d\n", ret);

	return ret;
}

#ifdef OMX_DYNAMIC_LOADING
void *omx_component_init = adec_component_init;
#endif

