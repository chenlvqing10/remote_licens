#define DBG_LEVEL		DBG_INFO

#include <oscl.h>
#include <pthread.h>
#include "oscl_queue.h"
#include <base_component.h>
#include "clock_component.h"
#include "clock_port.h"
#include "srender_component.h"
#include "subdec.h"

#define DEFAULT_DROPFRAME_TIMEDIFF	300000
#define MAX_DELAY_TIME_DIFF		3000
#define DEFAULT_THRESH	10000
/*#define MAX_DELAY_THRESH 100000*/

typedef struct srender_priv {
	OMX_U64 pts;
	OMX_TIME_CLOCKSTATE clk_state; /* clk state */
	OMX_S32             xscale;    /* scale of the clk */
	OMX_BOOL avsync;
	OMX_S32 fast_forward_mode;
	OMX_S32 back_forward_mode;
	OMX_S32 target_state;
} srender_priv_t;
static void srender_deal_clk_port(OMX_HANDLETYPE stand_com)
{
	lb_omx_component_t *component = NULL;
	base_port_t *clkport = NULL;
	srender_priv_t *srender_private = NULL;
	OMX_BUFFERHEADERTYPE		*clk_buffer = NULL;
	OMX_TIME_MEDIATIMETYPE		*mediatime;

	component = get_lb_component(stand_com);
	srender_private = component->component_private;
	clkport = &component->port[SRENDER_CLOCK_PORT_INDEX];

	/* check for any scale change or clock state change
	 * information from the clock component */
	if (sem_get(&clkport->buffer_sem) > 0) {
		OSCL_LOGI("sem value %d\n", sem_get(&clkport->buffer_sem));
		clk_buffer = oscl_queue_dequeue(&clkport->buffer_queue);
		sem_wait(&clkport->buffer_sem);
	}
	if (clk_buffer != NULL) {
		mediatime = (OMX_TIME_MEDIATIMETYPE *)clk_buffer->pBuffer;
		if (mediatime->eUpdateType == OMX_TIME_UpdateScaleChanged) {
			srender_private->xscale = mediatime->xScale;
			OSCL_LOGI("got scale changed scale %d\n",
				srender_private->xscale);
		} else if (mediatime->eUpdateType == OMX_TIME_UpdateClockStateChanged) {
			srender_private->clk_state = mediatime->eState;
			OSCL_LOGI("clock state change %d\n", srender_private->clk_state);
		}
		clkport->return_buffer(clkport, clk_buffer);
	}
}

static OMX_BOOL sub_clock_handle_frame(OMX_HANDLETYPE stand_com,
	OMX_BUFFERHEADERTYPE *inbuffer)
{
	lb_omx_component_t *component = NULL;
	base_port_t *clkport = NULL;
	srender_priv_t *srender_private = NULL;
	OMX_HANDLETYPE			clk_cmp = NULL;
	OMX_BUFFERHEADERTYPE		*clk_buffer = NULL;
	OMX_TIME_MEDIATIMETYPE		*mediatime;
	OMX_TIME_CONFIG_TIMESTAMPTYPE	cur_mediatime;
	OMX_TIME_CONFIG_TIMESTAMPTYPE	client_timestamp;
	OMX_ERRORTYPE			err;
	OMX_BOOL			sendframe;
	OMX_TICKS mediatime_diff, walltime_diff;
	OMX_S32 scale;
	av_subtitle_t *subin = (av_subtitle_t *)inbuffer->pBuffer;

	component = get_lb_component(stand_com);
	srender_private = component->component_private;
	clkport = &component->port[SRENDER_CLOCK_PORT_INDEX];
	clk_cmp = clkport->tunneled_component;

	sendframe = OMX_TRUE;

#if 0
	/* check for any scale change or clock state change
	 * information from the clock component */
	if (sem_get(&clkport->buffer_sem) > 0) {
		OSCL_LOGI("sem value %d\n", sem_get(&clkport->buffer_sem));
		clk_buffer = oscl_queue_dequeue(&clkport->buffer_queue);
		sem_wait(&clkport->buffer_sem);
	}
	if (clk_buffer != NULL) {
		mediatime = (OMX_TIME_MEDIATIMETYPE *)clk_buffer->pBuffer;
		if (mediatime->eUpdateType == OMX_TIME_UpdateScaleChanged) {
			srender_private->xscale = mediatime->xScale;
			OSCL_LOGI("got scale changed scale %d\n",
				srender_private->xscale);
		} else if (mediatime->eUpdateType == OMX_TIME_UpdateClockStateChanged) {
			srender_private->clk_state = mediatime->eState;
			OSCL_LOGI("clock state change %d\n", srender_private->clk_state);
		}
		clkport->return_buffer(clkport, clk_buffer);
		clk_buffer = NULL;
	}
#endif

	/* if first timestamp is received, then notity clk component */
	if (inbuffer->nFlags & OMX_BUFFERFLAG_STARTTIME) {

		OSCL_LOGD("first timestamp received %lld\n", inbuffer->nTimeStamp);
		client_timestamp.nSize = sizeof(client_timestamp);
		client_timestamp.nPortIndex = clkport->tunnel_ports;
		client_timestamp.nTimestamp = inbuffer->nTimeStamp;
		err = OMX_SetConfig(clk_cmp,
			OMX_IndexConfigTimeClientStartTime, &client_timestamp);
		if (err != OMX_ErrorNone)
			OSCL_LOGE("set client start time error!\n");

		/* wait for state change notification */
		OSCL_LOGI("sem %d (%s, %d)", sem_get(&clkport->buffer_sem),
			get_port_name(clkport), clkport->port_param.nPortIndex);
		oscl_sem_timedwait_ms(&clkport->buffer_sem, 1000);
		clk_buffer = oscl_queue_dequeue(&clkport->buffer_queue);
		if (clk_buffer != NULL) {
			mediatime = (OMX_TIME_MEDIATIMETYPE *)clk_buffer->pBuffer;
			srender_private->clk_state = mediatime->eState;
			srender_private->xscale = mediatime->xScale;
			clkport->return_buffer(clkport, clk_buffer);
			clk_buffer = NULL;
			OSCL_LOGE("srender clock state %d\n", srender_private->clk_state);
		}
	}

	if (inbuffer->nFlags & OMX_BUFFERFLAG_EOS) {
		/* eos frame, send it anyway */
		OSCL_LOGE("read eos frame");
		return OMX_TRUE;
	}
	/* do not send the data to sink and return back,
	 * if the clock is not running */
	if (srender_private->clk_state != OMX_TIME_ClockStateRunning) {
		OSCL_LOGE("clock is not running now, drop this frame(ts=%lld)!",
			inbuffer->nTimeStamp);
		inbuffer->nFilledLen = 0;
		sendframe = OMX_FALSE;
		return sendframe;
	}

	/* get cur mediatime */
	err = OMX_GetConfig(clk_cmp, OMX_IndexConfigTimeCurrentMediaTime, &cur_mediatime);
	if (err != OMX_ErrorNone) {
		OSCL_LOGW("get cur mediatime err!\n");
		return sendframe;
	}
	mediatime_diff = inbuffer->nTimeStamp - cur_mediatime.nTimestamp;
	scale = srender_private->xscale >> 16;
	if (scale == 0) {
		OSCL_LOGE("avsync isn't working!");
		OMX_TIME_CONFIG_SCALETYPE scale_param;
		memset(&scale_param, 0x00, sizeof(OMX_TIME_CONFIG_SCALETYPE));
		err = OMX_GetConfig(clk_cmp,
			OMX_IndexConfigTimeScale, &scale_param);
		if (err != OMX_ErrorNone)
			OSCL_LOGE("set client start time error!\n");
		srender_private->xscale = scale_param.xScale;
		scale = srender_private->xscale >> 16;
		if (scale == 0)
			return sendframe;
	}

	if ((subin->info.end_display_time) &&
		(subin->info.end_display_time < cur_mediatime.nTimestamp)) {
		sendframe = OMX_FALSE;
		OSCL_LOGW("drop one frame, buf st: %lld, cur st: %lldd",
			subin->info.end_display_time, cur_mediatime.nTimestamp);
	} else {

		walltime_diff = mediatime_diff / scale;
		if (walltime_diff > DEFAULT_THRESH) {
			while (1) {
				if ((component->state != OMX_StateExecuting) ||
					(srender_private->target_state !=
					OMX_StateExecuting)) {
					sendframe = OMX_FALSE;
					break;
				}
				oscl_mdelay(10);
				err = OMX_GetConfig(clk_cmp,
					OMX_IndexConfigTimeCurrentMediaTime,
					&cur_mediatime);
				if (err != OMX_ErrorNone) {
					OSCL_LOGW("get cur mediatime err!\n");
					return sendframe;
				}
				mediatime_diff =
					inbuffer->nTimeStamp - cur_mediatime.nTimestamp;
				walltime_diff = mediatime_diff / scale;
				if (walltime_diff <= DEFAULT_THRESH)
					break;
			}
		}
		sendframe = OMX_TRUE;

	}
	return sendframe;
}

void srender_buffer_handle(OMX_HANDLETYPE stand_com,
	OMX_BUFFERHEADERTYPE *inbuffer,
	OMX_BUFFERHEADERTYPE *outbuffer)
{
	av_subtitle_t *subin = (av_subtitle_t *)inbuffer->pBuffer;
	av_subtitle_t *subout = (av_subtitle_t *)outbuffer->pBuffer;

	if (inbuffer->nFilledLen == 0)
		return;
	outbuffer->nFilledLen = inbuffer->nFilledLen;
	*subout = *subin;
	subout->data.pdata = NULL;
	subout->style.name = NULL;
	if (subin->style.name)
		subout->style.name = (OMX_S8 *)strdup((const char *)subin->style.name);
	if (subin->data.pdata) {
		subout->data.pdata = (OMX_U8 *)oscl_zalloc(subin->data.datasize + 1);
		if (subout->data.pdata == NULL) {
			OSCL_LOGE("malloc subout->data.pdata failed");
			inbuffer->nFilledLen = 0;
			return;
		}
		memcpy(subout->data.pdata, subin->data.pdata, subin->data.datasize);
		subout->data.pdata[subin->data.datasize] = '\0';
	}

	inbuffer->nFilledLen = 0;
}

OMX_ERRORTYPE srender_set_parameter(OMX_IN OMX_HANDLETYPE hComp,
	OMX_IN OMX_INDEXTYPE paramIndex,
	OMX_IN OMX_PTR paramData)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	lb_omx_component_t *component = NULL;
	srender_priv_t *sink_private = NULL;
	OSCL_TRACE(" %p, %p , %x\n", hComp, paramData, paramIndex);
	oscl_param_check((hComp != NULL), OMX_ErrorBadParameter, NULL);
	component = get_lb_component(hComp);
	oscl_param_check((component != NULL), OMX_ErrorBadParameter, NULL);
	sink_private = component->component_private;
	switch (paramIndex) {
	case omx_index_media_avsync:
		sink_private->avsync = *((OMX_BOOL *)paramData);
		OSCL_LOGI("avsync %d\n", sink_private->avsync);
		break;
	default:
		ret = base_set_parameter(hComp, paramIndex, paramData);
		OSCL_TRACE(" %d\n", ret);
	}
	return ret;
}

void *srender_buffer_manager(void *param)
{
	lb_omx_component_t *component;
	base_port_t *inport;
	base_port_t *outport;
	OMX_BUFFERHEADERTYPE *outbuffer = NULL;
	OMX_BUFFERHEADERTYPE *inbuffer = NULL;
	OMX_COMPONENTTYPE *stand_comp = (OMX_COMPONENTTYPE *)param;
	srender_priv_t *srender_private = NULL;
	OSCL_TRACE(" %p\n", param);
	oscl_param_check((param != NULL), NULL, NULL);
	component = get_lb_component(stand_comp);
	srender_private = component->component_private;
	inport = &component->port[SRENDER_INPUT_PORT_INDEX];
	outport = &component->port[SRENDER_OUTPUT_PORT_INDEX];
	OSCL_LOGW("%s buffer_manager", component->name);

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
		pthread_mutex_lock(&component->flush_mutex);
		while (inport->is_flushed || outport->is_flushed) {
			pthread_mutex_unlock(&component->flush_mutex);
			OSCL_LOGW("flush in: %p", inbuffer);
			if (inbuffer && inport->is_flushed) {
				inport->return_buffer(inport, inbuffer);
				inbuffer = NULL;
			}
			OSCL_LOGW("flush out: %p", outbuffer);
			if (outbuffer && outport->is_flushed) {
				outport->recive_buffer(outport, outbuffer);
				outbuffer = NULL;
			}
			OSCL_LOGW("flush over");
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
		srender_deal_clk_port(component);
		if (component->state != OMX_StateExecuting) {
			OSCL_LOGD("==========%d", sem_get(component->buf_mgnt_sem));
			OSCL_LOGD("out buffer count %d\n", outport->buffer_queue.count);
			sem_wait(component->buf_mgnt_sem);
			continue;
		}
		if (inbuffer == NULL)
			inbuffer = oscl_queue_dequeue(&inport->buffer_queue);

		if (outbuffer == NULL)
			outbuffer = oscl_queue_dequeue(&outport->buffer_queue);

		base_check_eos(component, outport, outbuffer);
		if (inbuffer == NULL || outbuffer == NULL) {
			OSCL_LOGD("waiting buffer:%p %p\n", inbuffer, outbuffer);
			/*OSCL_LOGW("==========%d", sem_get(component->buf_mgnt_sem));*/
			sem_wait(component->buf_mgnt_sem);
			while (sem_trywait(component->buf_mgnt_sem) == 0)
				continue;
			continue;
		}

		if (srender_private->avsync &&
			!sub_clock_handle_frame(component, inbuffer)) {
			inport->return_buffer(inport, inbuffer);
			inbuffer = NULL;
			OSCL_LOGD("skip one subtitle frame\n");
			continue;
		}
		if (component->state == OMX_StateExecuting) {
			if (component->buf_handle)
				((component->buf_handle))(component, inbuffer, outbuffer);
		}
		if (outbuffer) {
			OSCL_LOGD("outbuffer->nfillen %d", outbuffer->nFilledLen);
			if ((outbuffer->nFilledLen != 0)
				|| (outbuffer->nFlags & OMX_BUFFERFLAG_EOS)
				|| (outbuffer->nFlags & OMX_BUFFERFLAG_STARTTIME)) {
				OSCL_LOGD("return outbuf %p, len %d\n",
					outbuffer, outbuffer->nFilledLen);
				outport->return_buffer(outport, outbuffer);
				outbuffer = NULL;
			} else {
				OSCL_LOGD("outbuffer not filled, len %d, flag %d\n",
					outbuffer->nFilledLen, outbuffer->nFlags);
				outport->recive_buffer(outport, outbuffer);
				outbuffer = NULL;
			}
		}
		if (inbuffer->nFlags & OMX_BUFFERFLAG_STARTTIME) {
			inbuffer->nFlags &= (~OMX_BUFFERFLAG_STARTTIME);
			(*(component->callbacks.EventHandler))
			(stand_comp, component->callback_data,
				OMX_EventOutputRendered, 0, 0, NULL);
			OSCL_LOGD("out put render\n");
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

	return NULL;
}

OMX_ERRORTYPE srender_set_state(OMX_HANDLETYPE hComp,
	OMX_U32 dest_state)
{
	lb_omx_component_t *component = NULL;
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	srender_priv_t *srender_private = NULL;

	oscl_param_check(hComp != NULL, OMX_ErrorBadParameter, NULL);
	OSCL_TRACE(" %p, %x\n", hComp, dest_state);
	component = get_lb_component(hComp);
	srender_private = component->component_private;
	srender_private->target_state = dest_state;
	ret = base_component_set_state(hComp, dest_state);

	return ret;
}
OMX_ERRORTYPE srender_fill_this_buffer(OMX_IN OMX_HANDLETYPE hComp,
	OMX_IN OMX_BUFFERHEADERTYPE *header)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	av_subtitle_t *subinfo = (av_subtitle_t *)header->pBuffer;

	oscl_param_check(hComp != NULL, OMX_ErrorBadParameter, NULL);
	oscl_param_check(header != NULL, OMX_ErrorBadParameter, NULL);
	/*OSCL_LOGI("srender return buffer");*/
	if (subinfo->data.pdata)
		free(subinfo->data.pdata);
	if (subinfo->style.name)
		free(subinfo->style.name);
	memset(subinfo, 0x00, sizeof(av_subtitle_t));

	header->nFilledLen = 0;
	header->nTimeStamp = 0;
	ret = base_fill_this_buffer(hComp, header);

	return ret;
}

OMX_ERRORTYPE srender_component_deinit(OMX_IN OMX_HANDLETYPE hComponent)
{
	lb_omx_component_t *component = NULL;
	srender_priv_t *srender_private = NULL;
	OSCL_TRACE("base cmp_handle:%p\n", hComponent);
	oscl_param_check(hComponent != NULL, OMX_ErrorBadParameter, NULL);
	component = get_lb_component(hComponent);
	srender_private = component->component_private;
	base_port_deinit(&component->port[SRENDER_INPUT_PORT_INDEX]);
	base_port_deinit(&component->port[SRENDER_OUTPUT_PORT_INDEX]);
	clock_port_deinit(&component->port[SRENDER_CLOCK_PORT_INDEX]);
	oscl_free(srender_private);
	base_component_deinit(hComponent);
	OSCL_LOGD("srender deinit complete!\n");
	return OMX_ErrorNone;
}

OMX_ERRORTYPE srender_component_init(lb_omx_component_t *cmp_handle)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	srender_priv_t *srender_private = NULL;
	OMX_COMPONENTTYPE *base_cmp = &(cmp_handle->base_comp);

	srender_private = oscl_zalloc(sizeof(srender_priv_t));
	if (!srender_private) {
		OSCL_LOGE("malloc srender_priv_t error!\n");
		return OMX_ErrorInsufficientResources;
	}
	ret = base_component_init(cmp_handle);
	if (ret != OMX_ErrorNone) {
		OSCL_LOGE("base_component_init error!\n");
		goto error1;
	}
	cmp_handle->name = "OMX.LB.SINK.SRENDER";
	cmp_handle->component_private = srender_private;
	cmp_handle->buf_manager = srender_buffer_manager;
	cmp_handle->buf_handle = srender_buffer_handle;
	cmp_handle->base_comp.ComponentDeInit = srender_component_deinit;
	cmp_handle->base_comp.SetParameter = srender_set_parameter;
	cmp_handle->do_state_set = srender_set_state;
	cmp_handle->num_ports = 3;

	base_cmp->FillThisBuffer = srender_fill_this_buffer;

	ret = base_port_init(cmp_handle, &cmp_handle->port[SRENDER_INPUT_PORT_INDEX],
			SRENDER_INPUT_PORT_INDEX, OMX_DirInput);
	if (ret != OMX_ErrorNone) {
		OSCL_LOGE("init base port error!\n");
		goto error2;
	}

	ret = base_port_init(cmp_handle, &cmp_handle->port[SRENDER_OUTPUT_PORT_INDEX],
			SRENDER_OUTPUT_PORT_INDEX, OMX_DirOutput);
	if (ret != OMX_ErrorNone) {
		OSCL_LOGE("init base port error!\n");
		goto error3;
	}

	ret = clock_port_init(cmp_handle, &cmp_handle->port[SRENDER_CLOCK_PORT_INDEX],
			SRENDER_CLOCK_PORT_INDEX, OMX_DirInput);
	if (ret != OMX_ErrorNone) {
		OSCL_LOGE("init clock port error!\n");
		goto error4;
	}
	cmp_handle->port[SRENDER_INPUT_PORT_INDEX].port_param.nBufferSize =
		SRENDER_INPUT_DEFAULT_SIZE;
	cmp_handle->port[SRENDER_INPUT_PORT_INDEX].port_param.eDomain =
		OMX_PortDomainSubtitle;
	cmp_handle->port[SRENDER_OUTPUT_PORT_INDEX].port_param.nBufferSize =
		SRENDER_INPUT_DEFAULT_SIZE;
	cmp_handle->port[SRENDER_OUTPUT_PORT_INDEX].port_param.eDomain =
		OMX_PortDomainSubtitle;
	cmp_handle->port[SRENDER_CLOCK_PORT_INDEX].port_param.nBufferSize =
		sizeof(av_subtitle_t) > SRENDER_INPUT_DEFAULT_SIZE ?
		sizeof(av_subtitle_t) : SRENDER_INPUT_DEFAULT_SIZE;
	cmp_handle->port[SRENDER_CLOCK_PORT_INDEX].port_param.eDomain =
		OMX_PortDomainOther;
	return ret;
error4:
	base_port_deinit(&cmp_handle->port[SRENDER_OUTPUT_PORT_INDEX]);
error3:
	base_port_deinit(&cmp_handle->port[SRENDER_CLOCK_PORT_INDEX]);
error2:
	base_component_deinit(cmp_handle);
error1:
	if (srender_private != NULL)
		oscl_free(srender_private);
	return ret;
}

#ifdef OMX_DYNAMIC_LOADING
void *omx_component_init = srender_component_init;
#endif

