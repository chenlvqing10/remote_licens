/*
 * untunnel.c - Standard functionality for utunnel mode.
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
#define LOG_TAG			"untunnel"

#include <oscl.h>
#include <omx_vendor_lb.h>
#include <base_component.h>
#include <omx_api.h>
#include <lb_omx_as_string.h>

#define get_comp_state(port) \
	(((comp_info_t *)(port->comp_info))->state)

/* Callbacks implementation */
OMX_ERRORTYPE untunnel_event_handler(
	OMX_HANDLETYPE comp_hdl,
	OMX_PTR app_data,
	OMX_EVENTTYPE event,
	OMX_U32 data1,
	OMX_U32 data2,
	OMX_PTR event_data)
{
	comp_info_t *comp_info = app_data;
	OSCL_TRACE("comp:%p, app_data:%p, event:%x, data1:%x, data2:%x, event data:%p",
		comp_hdl, app_data, event, data1, data2, event_data);

	oscl_param_check((comp_hdl != NULL && app_data != NULL),
		OMX_ErrorBadParameter, NULL);
	comp_info = (comp_info_t *)app_data;

	OSCL_LOGI("%s event:%s", comp_info->name, omx_evt_as_string(event));
	if (event == OMX_EventCmdComplete) {
		OSCL_LOGI("command complete:%s", omx_cmd_as_string(data1));
		if (data1 == OMX_CommandStateSet)
			OSCL_LOGI("set state complete:%s", omx_state_as_string(data2));
		sem_post(comp_info->sem_cmd);
	}
	if (event == OMX_EventError) {
		OSCL_LOGI("command err:%s, %x", omx_cmd_as_string(data2), data1);
		if (data2 == OMX_CommandStateSet ||
			data2 == OMX_CommandFlush ||
			data2 == OMX_CommandPortDisable ||
			data2 == OMX_CommandPortEnable ||
			data2 == OMX_CommandMarkBuffer)
			sem_post(comp_info->sem_cmd);
	}
	if (event == OMX_EventBufferFlag)
		OSCL_LOGI("%s OMX_EventBufferFlag %x", comp_info->name, data2);

	return OMX_ErrorNone;
}

static OMX_BUFFERHEADERTYPE *get_mapped_header(port_info_t *port,
	void *conect_port, OMX_BUFFERHEADERTYPE *buffer)
{
	port_info_t *connect_port;
	OMX_BUFFERHEADERTYPE *ret = NULL;
	int i;

	oscl_param_check((port != NULL && conect_port != NULL && buffer != NULL),
		NULL, NULL);
	connect_port = (port_info_t *)conect_port;
	if (connect_port->header == NULL)
		return buffer;

	for (i = 0; i < port->nbuffer; i++) {
		if (buffer == port->header[i]
			&& (buffer->pBuffer == connect_port->header[i]->pBuffer)) {
			ret = connect_port->header[i];
			ret->nFilledLen = buffer->nFilledLen;
			ret->nOffset = buffer->nOffset;
			ret->nTickCount = buffer->nTickCount;
			ret->nTimeStamp = buffer->nTimeStamp;
			ret->nFlags = buffer->nFlags;
			break;
		}
	}
	return ret;
}

int untunnel_hold_buffer(port_info_t *port, void *pbuffer)
{
	int index;
	oscl_param_check(port != NULL, OMX_ErrorBadParameter, NULL);
	oscl_param_check(port->edir == OMX_DirOutput, OMX_ErrorBadParameter, NULL);

	if (port->hold_map == NULL) {
		port->hold_map = oscl_malloc(sizeof(*port->hold_map) * port->nbuffer);
		oscl_param_check(port->hold_map != NULL,
			OMX_ErrorInsufficientResources, NULL);
		for (index = 0; index < port->nbuffer; index++)
			port->hold_map[index] = -1;
	}

	for (index = 0; index < port->nbuffer; index++) {
		if (pbuffer == port->header[index]->pBuffer)
			break;
	}
	oscl_param_check(index < port->nbuffer, OMX_ErrorBadParameter, NULL);

	if (port->hold_map[index] == -1) {
		port->hold_map[index] = index;
		port->nbuffer_hold++;
	} else if (port->hold_map[index] == index)
		OSCL_LOGE("port:%s already hold buf %d!", port_name(port), index);
	else {
		OSCL_LOGE("fatal error! port:%s hold_map[%d] corruption (%d)!",
			port_name(port), index, port->hold_map[index]);
	}

	OSCL_LOGI("port:%s-%d: hold buf=%p idx=%d, num=%d!", port_name(port),
		  port->index, pbuffer, index, port->nbuffer_hold);
	return 0;
}

int _port_queue_buffers(port_info_t *port)
{
	int ret = 0;
	OMX_COMPONENTTYPE *omx_hdl;
	int i;

	oscl_param_check(port != NULL, OMX_ErrorBadParameter, NULL);
	oscl_param_check(port->edir == OMX_DirOutput, OMX_ErrorBadParameter, NULL);
	oscl_param_check(port->hold_map != NULL, OMX_ErrorBadParameter, NULL);
	if (port->header == NULL || port->nbuffer_hold == 0) {
		OSCL_LOGI("none buffer to be queued");
		return 0;
	}
	OSCL_LOGW("nhold:%d", port->nbuffer_hold);

	if (port->nbuffer_hold != port->nbuffer)
		OSCL_LOGW("buffer:%d-%d",
			port->nbuffer_hold, port->nbuffer);
	omx_hdl = ((comp_info_t *)(port->comp_info))->cmp_hdl;

	for (i = 0; i < port->nbuffer; i++) {
		if (port->hold_map[i] == i) {
			ret = OMX_FillThisBuffer(omx_hdl, port->header[i]);
			if (ret != 0) {
				OSCL_LOGW("queue buffer failed!");
				untunnel_hold_buffer(port, port->header[i]->pBuffer);
			} else {
				port->hold_map[i] = -1;
				port->nbuffer_hold--;
			}
		} else if (port->hold_map[i] != -1)
			OSCL_LOGE("fatal error! port:%s hold_map[%d] corruption (%d)!",
				port_name(port), i, port->hold_map[i]);
	}

	if (port->nbuffer_hold)
		OSCL_LOGE("err! hold %d buffers after queue buffer", port->nbuffer_hold);
	if (port->nbuffer_hold < 0)
		port->nbuffer_hold = 0;
	OSCL_LOGW("nhold:%d", port->nbuffer_hold);
	return 0;
}

void untunnel_queue_buffers(comp_info_t *al)
{
	int i;
	for (i = 0; i < al->num_port; i++) {
		if (al->port_info[i].nbuffer_hold) {
			OSCL_LOGW("%s port %d queue buffers", al->name, i);
			_port_queue_buffers(&al->port_info[i]);
		}
	}
}

OMX_ERRORTYPE untunnel_empty_buffer_done(
	OMX_HANDLETYPE comp_hdl,
	OMX_PTR app_data,
	OMX_BUFFERHEADERTYPE *buffer)
{
	port_info_t *port, *tunnel_port;
	comp_info_t *comp, *tunnel_comp;
	OMX_BUFFERHEADERTYPE *outport_buffer;
	int ret = 0;

	OSCL_TRACE("comp_hdl:%p, app_data:%p, buffer:%p", comp_hdl, app_data, buffer);

	oscl_param_check((buffer != NULL && app_data != NULL),
		OMX_ErrorBadParameter, NULL);
	comp = (comp_info_t *)app_data;
	OSCL_TRACE("comp:%s, buffer:%p", comp->name, buffer);

	oscl_param_check((buffer->nInputPortIndex < comp->num_port),
		OMX_ErrorBadParameter, NULL);
	port = &comp->port_info[buffer->nInputPortIndex];
	oscl_param_check((port->state == PORT_STATE_UNTUN_SETUP),
			 OMX_ErrorBadParameter, NULL);
	oscl_param_check((port->tunnel_hdl != NULL), OMX_ErrorInvalidState, NULL);

	/* tunneled component will not call this callback */
	if (port->state != PORT_STATE_UNTUN_SETUP) {
		OSCL_LOGE("err: port is tunneled!");
		ret = OMX_ErrorBadParameter;
		goto EXIT;
	}
	tunnel_port = port->connect_port;
	tunnel_comp = tunnel_port->comp_info;
	OSCL_LOGD("%s-->%s:pbuffer=%p",
		comp->name,
		tunnel_comp->name,
		buffer->pBuffer);
	pthread_mutex_lock(&tunnel_comp->state_lock);
	/* client(out port) hold this buffer while not executing in untunnel mode */
	if (get_comp_state(tunnel_port) != OMX_StateExecuting &&
		get_comp_state(tunnel_port) != OMX_StatePause &&
		(tunnel_port->is_shared_buffer == 0)) {
		OSCL_LOGI("%s-->%s state %d, hold buffer",
			comp->name,
			tunnel_comp->name,
			get_comp_state(tunnel_port));
		untunnel_hold_buffer(tunnel_port, buffer->pBuffer);
		ret = OMX_ErrorNone;
		pthread_mutex_unlock(&tunnel_comp->state_lock);
		goto EXIT;
	} else {
		if (tunnel_port->nbuffer_hold > 0) {
			OSCL_LOGW("%s(%d): hold %d buffer while in executing state!!",
				tunnel_comp->name,
				tunnel_port->index,
				tunnel_port->nbuffer_hold);
			untunnel_queue_buffers(tunnel_comp);
		}
	}
	pthread_mutex_unlock(&tunnel_comp->state_lock);
	/* get output port buffer header*/
	outport_buffer = get_mapped_header(port, port->connect_port, buffer);
	oscl_param_check(outport_buffer != NULL, OMX_ErrorBadParameter, NULL);

	/* empty buffer done, ask output port fill this buffer */
	ret = OMX_FillThisBuffer(port->tunnel_hdl, outport_buffer);
	if (ret != 0) {
		if (tunnel_port->is_shared_buffer == 0) {
			OSCL_LOGW("fill buffer failed, hold it!");
			OSCL_LOGE("====%s-->%s, hold buffer",
				comp->name,
				tunnel_comp->name);
			untunnel_hold_buffer(tunnel_port, buffer->pBuffer);
			ret = OMX_ErrorNone;
		} else
			OSCL_LOGW("fill buffer failed, return to input port!");
	}
EXIT:
	OSCL_TRACE("%x", ret);
	return ret;
}

OMX_ERRORTYPE untunnel_fill_buffer_done(
	OMX_HANDLETYPE comp_hdl,
	OMX_PTR app_data,
	OMX_BUFFERHEADERTYPE *buffer)
{
	port_info_t *port;
	comp_info_t *comp;
	OMX_BUFFERHEADERTYPE *inport_buffer;
	int ret = 0;

	OSCL_TRACE("comp_hdl:%p, app_data:%p, buffer:%p", comp_hdl, app_data, buffer);

	/* get app layer component info and port info */
	oscl_param_check((buffer != NULL && app_data != NULL),
		OMX_ErrorBadParameter, NULL);
	comp = (comp_info_t *)app_data;
	oscl_param_check((buffer->nOutputPortIndex < comp->num_port),
		OMX_ErrorBadParameter, NULL);
	OSCL_TRACE("comp:%s, buffer:%p, num_port:%d, port idx:%d",
		comp->name, buffer, comp->num_port, buffer->nOutputPortIndex);
	port = &comp->port_info[buffer->nOutputPortIndex];

	/* tunneled component will not call this callback */
	if (port->state != PORT_STATE_UNTUN_SETUP) {
		OSCL_LOGE("err: port is tunneled!");
		ret = OMX_ErrorBadParameter;
		goto EXIT;
	}

	/* if callback is set, send buffer to app */
	ret = app_callback_handle(port, buffer, OUTPORT_FILL_DONE);
	if (ret == APP_BUFFER_ASYNC || ret == APP_BUFFER_HANDLED) {
		ret = 0;
		goto EXIT;
	}

	if (port->connect_port == NULL || port->tunnel_hdl == NULL) {
		ret = OMX_ErrorUndefined;
		goto EXIT;
	}

	OSCL_LOGD("%s-->%s:pbuffer=%p", comp->name,
			((comp_info_t *)(port->connect_port->comp_info))->name,
			buffer->pBuffer);

	/* client hold this buffer while not executing in untunnel mode */
	pthread_mutex_lock(&comp->state_lock);
	if (get_comp_state(port->connect_port) != OMX_StateExecuting &&
		get_comp_state(port->connect_port) != OMX_StatePause) {
		if (port->is_shared_buffer == 0) {
			OSCL_LOGI("%s-->%s state %d, hold buffer",
				comp->name,
				((comp_info_t *)((port->connect_port)->comp_info))->name,
				get_comp_state(port->connect_port));
			untunnel_hold_buffer(port, buffer->pBuffer);
			ret = OMX_ErrorNone;
		} else {
			ret = OMX_ErrorIncorrectStateOperation;
		}
		pthread_mutex_unlock(&comp->state_lock);
		goto EXIT;
	}
	if (port->nbuffer_hold > 0 &&
		(comp->state == OMX_StateExecuting || comp->state == OMX_StatePause)) {
		OSCL_LOGW("%s(%d): hold %d buffer while in executing state!!",
			comp->name,
			port->index,
			port->nbuffer_hold);
		untunnel_queue_buffers(comp);
	}
	pthread_mutex_unlock(&comp->state_lock);

	/* get input port buffer header*/
	inport_buffer = get_mapped_header(port, port->connect_port, buffer);
	oscl_param_check(inport_buffer != NULL, OMX_ErrorBadParameter, comp->name);
	if (inport_buffer->nFilledLen == 0)
		OSCL_LOGI("component:%s(%d) inbuf:%p,%d ",
			comp->name, port->index,
			inport_buffer, inport_buffer->nFilledLen);

	/* fill buffer done, ask input port empty this buffer */
	ret = OMX_EmptyThisBuffer(port->tunnel_hdl, inport_buffer);
	if (ret != 0) {
		if (port->is_shared_buffer == 0) {
			OSCL_LOGW("%s:%d empty %p failed, hold it!",
				comp->name, port->index, inport_buffer);
			untunnel_hold_buffer(port, buffer->pBuffer);
			ret = OMX_ErrorNone;
		} else
			OSCL_LOGI("empty buffer failed, return to output, %s %d:%p,%p!",
				comp->name, port->index, buffer, buffer->pBuffer);
	}
EXIT:
	OSCL_TRACE("%x", ret);
	return ret;
}

OMX_CALLBACKTYPE untunnel_common_callbacks = {
	.EventHandler = untunnel_event_handler,
	.EmptyBufferDone = untunnel_empty_buffer_done,
	.FillBufferDone = untunnel_fill_buffer_done,
};


int untunnel_setup_ports(port_info_t *out_port, port_info_t *in_port)
{
	int ret;
	int i;
	comp_info_t *out;
	comp_info_t *in;
	OMX_PARAM_PORTDEFINITIONTYPE port_def;
	OMX_BUFFERHEADERTYPE *outport_buf;

	oscl_param_check((out_port != NULL && in_port != NULL),
		OMX_ErrorBadParameter, NULL);
	oscl_param_check((out_port->domain == in_port->domain),
		OMX_ErrorBadParameter, NULL);

	OSCL_LOGI("peer ports:%s(%d)--%s(%d)",
		((comp_info_t *)out_port->comp_info)->name, out_port->index,
		((comp_info_t *)in_port->comp_info)->name, in_port->index);
	out_port->tunnel_hdl = ((comp_info_t *)in_port->comp_info)->cmp_hdl;
	out_port->connect_port = in_port;
	in_port->tunnel_hdl = ((comp_info_t *)out_port->comp_info)->cmp_hdl;
	in_port->connect_port = out_port;

	/* set output config to input component: get output config */
	port_def.nPortIndex = out_port->index;
	port_def.nVersion.nVersion = OMX_VERSION;
	ret = OMX_GetParameter(((comp_info_t *)out_port->comp_info)->cmp_hdl,
			OMX_IndexParamPortDefinition, &port_def);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);
	OSCL_LOGI("port_def %d, %d", port_def.nBufferCountActual, port_def.nBufferSize);
	OSCL_LOGI("out_port %d, %d", out_port->nbuffer, out_port->buf_size);
	out_port->nbuffer = port_def.nBufferCountActual;
	out_port->buf_size = port_def.nBufferSize;

	if (port_def.eDomain == OMX_PortDomainVideo)
		OSCL_LOGI("eColorFormat:%d, w-h(%d, %d), (%d, %d)",
			port_def.format.video.eColorFormat,
			port_def.format.video.nFrameWidth,
			port_def.format.video.nFrameHeight,
			port_def.format.video.nStride,
			port_def.format.video.nSliceHeight);

	/* set output config to input component: set input config */
	port_def.nPortIndex = in_port->index;
	OSCL_LOGI("port_def %d, %d", port_def.nBufferCountActual, port_def.nBufferSize);
	ret = OMX_SetParameter(((comp_info_t *)in_port->comp_info)->cmp_hdl,
			OMX_IndexParamPortDefinition, &port_def);
	OSCL_LOGI("port_def %d, %d", port_def.nBufferCountActual, port_def.nBufferSize);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);
	in_port->nbuffer = port_def.nBufferCountActual;
	in_port->buf_size = port_def.nBufferSize;

	/* require buffer : init header array */
	if (out_port->header != NULL) {
		OSCL_LOGE("err! header is not null while set up ports!");
		oscl_free(out_port->header);
	}
	out_port->header = oscl_zalloc(
			out_port->nbuffer * sizeof(OMX_BUFFERHEADERTYPE *));
	oscl_param_check_exit(NULL != out_port->header,
		OMX_ErrorInsufficientResources, NULL);

	in_port->nbuffer = out_port->nbuffer;
	if (in_port->header != NULL) {
		OSCL_LOGE("err! header is not null while set up ports!");
		oscl_free(in_port->header);
	}
	in_port->header = oscl_zalloc(in_port->nbuffer * sizeof(OMX_BUFFERHEADERTYPE *));
	oscl_param_check_exit(NULL != in_port->header,
		OMX_ErrorInsufficientResources, NULL);

	out = out_port->comp_info;
	in = in_port->comp_info;
	for (i = 0; i < out_port->nbuffer; i++) {
		/* require buffer : allocate buffer in output port */
		out_port->header[i] = NULL;
		ret = OMX_AllocateBuffer(out->cmp_hdl, &out_port->header[i],
				out_port->index, out, out_port->buf_size);
		OSCL_LOGI("=====AllocBuffer %d(p%d), %p(%p)", i, out_port->index,
			out_port->header[i],
			out_port->header[i]->pBuffer);
		if (ret != OMX_ErrorNone) {
			out_port->nbuffer = i;
			OSCL_LOGE("get Buffer %d on port %d fail, size:%d",
				i, out_port->index, out_port->buf_size);
			break;
		}

		outport_buf = out_port->header[i];
		outport_buf->nInputPortIndex = in_port->index;

		/* require buffer : use buffer in input port */
		in_port->header[i] = NULL;
		ret = OMX_UseBuffer(in->cmp_hdl, &in_port->header[i], in_port->index,
				in, in_port->buf_size, outport_buf->pBuffer);
		OSCL_LOGI("=====OMX_UseBuffer %d(p%d), %d, %p(%p)", i, in_port->index,
			in_port->buf_size, in_port->header[i],
			in_port->header[i]->pBuffer);
		if (ret != OMX_ErrorNone) {
			in_port->nbuffer = i;
			OSCL_LOGE("OMX_UseBuffer %d on port %d fail", i, in_port->index);
			break;
		}
		in_port->header[i]->nOutputPortIndex = out_port->index;
		in_port->header[i]->pOutputPortPrivate = outport_buf->pOutputPortPrivate;
		outport_buf->pInputPortPrivate = in_port->header[i]->pInputPortPrivate;

	}

	for (i = 0; (i < out_port->nbuffer) && (out_port->header[i]); i++) {
		ret = OMX_FillThisBuffer(out->cmp_hdl, out_port->header[i]);
		OSCL_LOGI("OMX_FillThisBuffer %d on port %d", i, out_port->index);
		if (ret != OMX_ErrorNone) {
			OSCL_LOGE("fill Buffer %d on port %d fail", i, out_port->index);
			break;
		}
	}
	out_port->state = PORT_STATE_UNTUN_SETUP;
	in_port->state = PORT_STATE_UNTUN_SETUP;
	init_cb_info(out_port);
	init_cb_info(in_port);
EXIT:
	return ret;
}

void _reset_port(port_info_t *port)
{
	int i;
	int ret;
	OMX_COMPONENTTYPE *omx_comp;

	if (port == NULL || (port->comp_info == NULL) || (port->header == NULL))
		return;
	deinit_cb_info(port);

	port->tunnel_hdl = NULL;
	port->connect_port = NULL;
	port->nbuffer_hold = 0;
	port->state = PORT_STATE_INIT;
	omx_comp = ((comp_info_t *)(port->comp_info))->cmp_hdl;
	OSCL_LOGI("%s OMX_FreeBuffer on p%d",
		((comp_info_t *)(port->comp_info))->name, port->index);
	for (i = 0; i < port->nbuffer; i++) {
		OSCL_LOGI("%s OMX_FreeBuffer %d on p%d:%p,%p",
			((comp_info_t *)(port->comp_info))->name,
			i, port->index, port->header[i],
			(port->header[i]) ? port->header[i]->pBuffer : NULL);
		ret = OMX_FreeBuffer(omx_comp, port->index, port->header[i]);
		if (ret != OMX_ErrorNone)
			OSCL_LOGE("%s OMX_FreeBuffer %d on p%d fail",
				((comp_info_t *)(port->comp_info))->name,
				i, port->index);
		if (port->hold_map != NULL)
			port->hold_map[i] = -1;
	}
	oscl_free(port->header);
	port->header = NULL;
	if (port->hold_map != NULL) {
		oscl_free(port->hold_map);
		port->hold_map = NULL;
	}
}

void untunnel_unset_ports(port_info_t *out_port, port_info_t *in_port)
{
	int nbuf_hold = 0;
	int nbuf = 0;
	int nretry = 10;
	comp_info_t *out = NULL;
	comp_info_t *in = NULL;

	if (out_port && in_port && (!out_port->is_shared_buffer)) {
		in = in_port->comp_info;
		out = out_port->comp_info;
	}
	if (in && out) {
		OSCL_LOGI("outport:%s-%d!", out->name, out_port->index);
		OSCL_LOGI("in_port:%s-%d!", in->name, in_port->index);
		do {
			nbuf_hold = out_port->nbuffer_hold + in_port->nbuffer_hold;
			nbuf = out_port->nbuffer;
			oscl_mdelay(10);
		} while (nbuf_hold != nbuf && nretry--);
	}
	if (nretry <= 0)
		OSCL_LOGE("%s:%s nhold(%d:%d/%d) / nbuf(%d), flag:%d!",
			out->name, in->name,
			nbuf_hold, out_port->nbuffer_hold,
			in_port->nbuffer_hold, nbuf, out_port->is_shared_buffer);
	_reset_port(out_port);
	_reset_port(in_port);
}


