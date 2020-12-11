#define DBG_LEVEL         DBG_ERROR
#define LOG_TAG           "sdec_component"

#include <oscl.h>
#include <base_component.h>
#include <demuxer_plugin.h>
#include "sdec_common.h"
#include "sdec_component.h"
#include <demuxer_plugin.h>
#include <omx_vendor_lb.h>

static av_codec_type_e conv_scodec_type(av_codec_type_e codectype)
{
	switch (codectype) {
	case AV_CODEC_TYPE_ASS:
		return AV_CODEC_TYPE_ASS;
	default:
		return codectype;
	}
}

void sdec_buffer_handle(OMX_HANDLETYPE hComp,
	OMX_BUFFERHEADERTYPE *inbuf, OMX_BUFFERHEADERTYPE *outbuf)
{
	lb_omx_component_t *component = NULL;
	sdec_private_t *priv = NULL;
	dmx_packet_t *dmx_pkt = (dmx_packet_t *)inbuf->pBuffer;
	OMX_S32 ret = 0;
	component = get_lb_component(hComp);
	priv = component->component_private;
	if (dmx_pkt->flags & AV_PACKET_FLAG_RESET) {
		sdec_flush(priv);
		OSCL_LOGI("start pts %lld", (long long)dmx_pkt->pts);
		outbuf->nFlags |= OMX_BUFFERFLAG_STARTTIME;
		dmx_pkt->flags &= ~AV_PACKET_FLAG_RESET;
	}
	if (dmx_pkt->flags & AV_PACKET_FLAG_EOS) {
		OSCL_LOGI("detected: size=%d, ts=%lld",
			dmx_pkt->data_size, (long long)dmx_pkt->pts);
		outbuf->nFlags |= OMX_BUFFERFLAG_EOS;
	}
	if (!priv->open_error) {
		ret = sdec_frame(priv, inbuf, outbuf);
		if (ret >= 0)
			OSCL_LOGD("sdec_frame success");
		else if (ret < 0)
			OSCL_LOGE("sdec_frame error, ret=%d", ret);
	}

	inbuf->nFilledLen = 0;

	return;
}

void *sdec_buffer_manager(void *param)
{
	lb_omx_component_t *component;
	base_port_t *inport;
	base_port_t *outport;
	OMX_BUFFERHEADERTYPE *outbuffer = NULL;
	OMX_BUFFERHEADERTYPE *inbuffer = NULL;

	OSCL_TRACE(" %p\n", param);
	oscl_param_check((param != NULL), NULL, NULL);
	component = get_lb_component(param);

	inport = &component->port[SDEC_INPUT_PORT_INDEX];
	outport = &component->port[SDEC_OUTPUT_PORT_INDEX];
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
			OSCL_LOGD("STATE %s", omx_state_as_string(component->state));
			OSCL_LOGD("out buffer count %d, sem %d",
				outport->buffer_queue.count,
				sem_get(component->buf_mgnt_sem));
			sem_wait(component->buf_mgnt_sem);
			continue;
		}

		if (inbuffer == NULL)
			inbuffer = oscl_queue_dequeue(&inport->buffer_queue);

		if (outbuffer == NULL)
			outbuffer = oscl_queue_dequeue(&outport->buffer_queue);
		if (outbuffer) {
			outbuffer->nFilledLen = 0;
			outbuffer->nFlags = 0;
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

		if (component->state == OMX_StateExecuting && inbuffer->nFilledLen != 0) {
			if (component->buf_handle)
				((component->buf_handle))(component, inbuffer, outbuffer);
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
OMX_ERRORTYPE sdec_fill_this_buffer(OMX_IN OMX_HANDLETYPE hComp,
		OMX_IN OMX_BUFFERHEADERTYPE *header)

{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	lb_omx_component_t *component;
	sdec_private_t *sdec_private;
	av_subtitle_t *subinfo = (av_subtitle_t *)header->pBuffer;
	OMX_STATETYPE state;

	oscl_param_check(hComp != NULL, OMX_ErrorBadParameter, NULL);
	oscl_param_check(header != NULL, OMX_ErrorBadParameter, NULL);
	component = get_lb_component(hComp);
	sdec_private = (sdec_private_t *)(component->component_private);

	state = component->state;
	if (subinfo->data.pdata && (state != OMX_StateInvalid) &&
		(state != OMX_StateLoaded)) {
		ret = sdec_return_frame(sdec_private, subinfo);
		if (ret != 0) {
			OSCL_LOGE("return packet error(ret=%d)", ret);
			ret = OMX_ErrorUndefined;
		}
	}
	memset(subinfo, 0x00, sizeof(av_subtitle_t));
	header->nFilledLen = 0;
	header->nTimeStamp = 0;
	ret = base_fill_this_buffer(hComp, header);

	return ret;
}

OMX_ERRORTYPE sdec_set_parameter(OMX_IN OMX_HANDLETYPE hComp,
	OMX_IN OMX_INDEXTYPE paramIndex,
	OMX_IN OMX_PTR paramData)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	lb_omx_component_t *component = NULL;
	sdec_private_t *sub_private = NULL;
	OSCL_TRACE(" %p, %p , %d\n", hComp, paramData, paramIndex);
	oscl_param_check((hComp != NULL) && (paramData != NULL),
		OMX_ErrorBadParameter, NULL);
	component = get_lb_component(hComp);
	oscl_param_check((component != NULL), OMX_ErrorBadParameter, NULL);
	sub_private = component->component_private;
	switch (paramIndex) {
	case omx_index_media_info:
		ret = 0;
		break;
	case omx_index_media_subtitle_info: {
		omx_sub_info_t *sinfo = (omx_sub_info_t *)paramData;
		sub_private->para->codec_id =
			conv_scodec_type(sinfo->codec_id);
		sub_private->extradata = sinfo->extradata;
		sub_private->extrasize = sinfo->extrasize;

		OSCL_LOGI("subtitle fmt change, codec(old=%x new=%x)",
			sub_private->info.codec_id,
			sinfo->codec_id);

		sub_private->info = *sinfo;
		break;
	}
	default:
		ret = base_set_parameter(hComp, paramIndex, paramData);
		break;
	}
	OSCL_TRACE(" %d\n", ret);
	return ret;
}

OMX_ERRORTYPE sdec_set_state(OMX_HANDLETYPE hComp,
	OMX_U32 dest_state)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	lb_omx_component_t *component;
	sdec_private_t *sub_private;
	OMX_STATETYPE pre_state;

	OSCL_TRACE(" %p, %x\n", hComp, dest_state);
	oscl_param_check(hComp != NULL, OMX_ErrorBadParameter, NULL);
	component = get_lb_component(hComp);
	sub_private = component->component_private;

	OSCL_LOGI("%s->%s", omx_state_as_string(component->state),
		omx_state_as_string(dest_state));
	pre_state = component->state;
	ret = base_component_set_state(hComp, dest_state);
	if (dest_state == OMX_StateIdle && pre_state == OMX_StateLoaded) {
		OSCL_LOGE("opening sdec");
		sub_private->open_error = OMX_FALSE;
		ret = sdec_open(sub_private);
		if (ret < 0) {
			OSCL_LOGE("open sdec error(%d)!", ret);
			sub_private->open_error = OMX_TRUE;
			ret = OMX_ErrorNone;
			goto ERR_EXIT;
		}
	}
	if (dest_state == OMX_StateLoaded && pre_state == OMX_StateIdle) {
		OSCL_LOGI("closing sdec");
		sdec_close(sub_private);
	}
	return ret;
ERR_EXIT:
	((component->callbacks.EventHandler))(&component->base_comp,
		component->callback_data, OMX_EventError, ret, -1, NULL);
	return ret;

}

OMX_ERRORTYPE sdec_component_deinit(OMX_IN OMX_HANDLETYPE hComp)
{
	OMX_COMPONENTTYPE *base_cmp = (OMX_COMPONENTTYPE *)hComp;
	lb_omx_component_t *component = NULL;
	sdec_private_t *sub_private = NULL;
	OMX_S32 ret;

	OSCL_TRACE("base_cmp_handle: %p\n", hComp);
	oscl_param_check(hComp != NULL, OMX_ErrorBadParameter, NULL);
	component = (lb_omx_component_t *)base_cmp->pComponentPrivate;
	sub_private = (sdec_private_t *)component->component_private;

	component->port[SDEC_INPUT_PORT_INDEX].deinit(
		&component->port[SDEC_INPUT_PORT_INDEX]);
	component->port[SDEC_OUTPUT_PORT_INDEX].deinit(
		&component->port[SDEC_OUTPUT_PORT_INDEX]);
	if (sub_private) {
		sdec_deinit(sub_private);
		component->component_private = NULL;
	}
	ret = base_component_deinit(hComp);
	return ret;
}

OMX_ERRORTYPE sdec_component_init(lb_omx_component_t *cmp_handle)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	sdec_private_t *sub_private = NULL;
	struct sched_param shed_param = {0};

	sub_private = sdec_init();
	if (!sub_private) {
		OSCL_LOGE("sub_private error!\n");
		return OMX_ErrorInsufficientResources;
	}
	OSCL_TRACE("sdec_component_init: %p\n", cmp_handle);
	ret = base_component_init(cmp_handle);
	if (ret != OMX_ErrorNone)
		return ret;

	pthread_attr_getschedparam(&cmp_handle->buf_thread_attr, &shed_param);
	OSCL_LOGE("priority: %d", shed_param.sched_priority);

	pthread_attr_setschedparam(&cmp_handle->buf_thread_attr, &shed_param);
	pthread_attr_setstacksize(&cmp_handle->buf_thread_attr, 0x4000);

	/* port init */
	cmp_handle->num_ports = 2;
	ret = base_port_init(cmp_handle, &cmp_handle->port[SDEC_INPUT_PORT_INDEX],
			SDEC_INPUT_PORT_INDEX,
			OMX_DirInput);
	if (ret != OMX_ErrorNone) {
		base_component_deinit(cmp_handle);
		return ret;
	}
	ret = base_port_init(cmp_handle, &cmp_handle->port[SDEC_OUTPUT_PORT_INDEX],
			SDEC_OUTPUT_PORT_INDEX,
			OMX_DirOutput);
	if (ret != OMX_ErrorNone) {
		base_component_deinit(cmp_handle);
		cmp_handle->port[SDEC_INPUT_PORT_INDEX].deinit(
			&cmp_handle->port[SDEC_INPUT_PORT_INDEX]);
		return ret;
	}
	cmp_handle->port[SDEC_INPUT_PORT_INDEX].port_param.nBufferSize =
		OMX_DEFAULT_BUF_SIZE;
	cmp_handle->port[SDEC_INPUT_PORT_INDEX].port_param.eDomain =
		OMX_PortDomainSubtitle;

	cmp_handle->port[SDEC_OUTPUT_PORT_INDEX].port_param.nBufferSize =
		OMX_DEFAULT_BUF_SIZE;
	cmp_handle->port[SDEC_OUTPUT_PORT_INDEX].port_param.eDomain =
		OMX_PortDomainSubtitle;
	cmp_handle->port[SDEC_OUTPUT_PORT_INDEX].port_param.nBufferCountActual = 16;

	cmp_handle->name = "OMX.LB.SINK.SDEC";
	cmp_handle->component_private = sub_private;
	cmp_handle->base_comp.ComponentDeInit = sdec_component_deinit;
	cmp_handle->buf_manager = sdec_buffer_manager;
	cmp_handle->buf_handle = sdec_buffer_handle;
	cmp_handle->base_comp.SetParameter = sdec_set_parameter;
	cmp_handle->base_comp.FillThisBuffer = sdec_fill_this_buffer;
	cmp_handle->do_state_set = sdec_set_state;
	OSCL_TRACE("sdec_component_init:%d\n", ret);
	return ret;
}

#ifdef OMX_DYNAMIC_LOADING
void *omx_component_init = sdec_component_init;
#endif

