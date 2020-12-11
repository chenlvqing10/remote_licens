/*
 * app_callback.c - Standard functionality for utunnel callback func set by user.
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
#include "subdec.h"
#define get_comp_state(port) \
	(((comp_info_t *)(port->comp_info))->state)

static void printf_extra_info(omx_extra_data_t *data)
{
	OSCL_LOGI("----------------untunel extra info-----------------");
	OSCL_LOGI("exposure_time:%d", data->isp_inf.exposure_time);
	OSCL_LOGI("shutter_speed:%d", data->isp_inf.shutter_speed);
	OSCL_LOGI("aperture:%d", data->isp_inf.aperture);
	OSCL_LOGI("brightness:%d", data->isp_inf.brightness);
	OSCL_LOGI("exposure_bias:%d", data->isp_inf.exposure_bias);
	OSCL_LOGI("iso_speed:%d", data->isp_inf.iso_speed);
	OSCL_LOGI("flash:%d", data->isp_inf.flash);
	OSCL_LOGI("illu_id:%d", data->isp_inf.illu_id);
	OSCL_LOGI("back_score:%d", data->isp_inf.back_score);
	OSCL_LOGI("-------------------------------------------");
}
static void printf_sub_info(subtitle_frame_info_t *frame)
{
	OSCL_LOGE("\n*************************start print sub info********************");
	OSCL_LOGE("frame->comm_info.resx: %d", frame->comm_info.resx);
	OSCL_LOGE("frame->comm_info.resy: %d", frame->comm_info.resy);
	OSCL_LOGE("frame->comm_info.startdisptime: %lld",
		frame->comm_info.start_disp_time);
	OSCL_LOGE("frame->comm_info.enddisptime: %lld",
		frame->comm_info.end_disp_time);
	OSCL_LOGE("frame->style.has_style_flag: %d", frame->style.has_style_flag);
	OSCL_LOGE("frame->style.name: %s", frame->style.name);
	OSCL_LOGE("frame->style.font_style: %d", frame->style.font_style);
	OSCL_LOGE("frame->style.fontsize: %d", frame->style.fontsize);
	OSCL_LOGE("frame->style.primarycolour: %d", frame->style.primarycolour);
	OSCL_LOGE("frame->style.secondarycolour: %d", frame->style.secondarycolour);
	OSCL_LOGE("frame->style.outlinecolour: %d", frame->style.outlinecolour);
	OSCL_LOGE("frame->style.backcolour: %d", frame->style.backcolour);
	OSCL_LOGE("frame->style.bold: %d", frame->style.bold);
	OSCL_LOGE("frame->style.italic: %d", frame->style.italic);
	OSCL_LOGE("frame->style.underline: %d", frame->style.underline);
	OSCL_LOGE("frame->style.strikeout: %d", frame->style.strikeout);
	OSCL_LOGE("frame->style.scalex: %f", frame->style.scalex);
	OSCL_LOGE("frame->style.scaley: %f", frame->style.scaley);
	OSCL_LOGE("frame->style.spacing: %d", frame->style.spacing);
	OSCL_LOGE("frame->style.angle: %d", frame->style.angle);
	OSCL_LOGE("frame->style.borderstyle: %d", frame->style.borderstyle);
	OSCL_LOGE("frame->style.outline: %f", frame->style.outline);
	OSCL_LOGE("frame->style.shadow: %f", frame->style.shadow);
	OSCL_LOGE("frame->style.alignment: %d", frame->style.alignment);
	OSCL_LOGE("frame->style.marginl: %d", frame->style.marginl);
	OSCL_LOGE("frame->style.marginr: %d", frame->style.marginr);
	OSCL_LOGE("frame->style.marginv: %d", frame->style.marginv);
	OSCL_LOGE("frame->style.alphalevel: %d", frame->style.alphalevel);
	OSCL_LOGE("frame->style.encoding: %d", frame->style.encoding);
	OSCL_LOGE("frame->data.dialogue_marginl: %d", frame->data.dialogue_marginl);
	OSCL_LOGE("frame->data.dialogue_marginr: %d", frame->data.dialogue_marginr);
	OSCL_LOGE("frame->data.dialogue_marginv: %d", frame->data.dialogue_marginv);
	OSCL_LOGE("frame->data.sub_effect_type: %d", frame->data.sub_effect_type);
	OSCL_LOGE("frame->data.effectstartx: %d", frame->data.effectstartx);
	OSCL_LOGE("frame->data.effectendx: %d", frame->data.effectendx);
	OSCL_LOGE("frame->data.effectstarty: %d", frame->data.effectstarty);
	OSCL_LOGE("frame->data.effectendy: %d", frame->data.effectendy);
	OSCL_LOGE("frame->data.effectdelaytime: %d", frame->data.effectdelaytime);
	OSCL_LOGE("frame->data.type: %d", frame->data.type);
	OSCL_LOGE("frame->data.datasize: %d", frame->data.datasize);
	OSCL_LOGE("frame->data.pdata: %s", frame->data.pdata);
	OSCL_LOGE("*******************************end printf*************************\n");
}
static int copy_sub_info(subtitle_frame_info_t *frame, av_subtitle_t *psub)
{
	if ((NULL == frame) || (NULL == psub)) {
		OSCL_LOGE("when copy sub info, para is NULL: %p, %p", frame, psub);
		return -1;
	}

	frame->comm_info.index = psub->info.index;
	frame->comm_info.resx = psub->info.resx;
	frame->comm_info.resy = psub->info.resy;
	frame->comm_info.start_disp_time =
		psub->info.start_display_time;
	frame->comm_info.end_disp_time =
		psub->info.end_display_time;
	frame->style.has_style_flag =  psub->style.has_style_flag;
	if (psub->style.has_style_flag) {
		frame->style.name = (char *)psub->style.name;
		frame->style.font_style = psub->style.font_style;
		frame->style.fontsize = psub->style.fontsize;
		frame->style.primarycolour = psub->style.primarycolour;
		frame->style.secondarycolour = psub->style.secondarycolour;
		frame->style.outlinecolour = psub->style.outlinecolour;
		frame->style.backcolour = psub->style.backcolour;
		frame->style.bold = psub->style.bold;
		frame->style.italic = psub->style.italic;
		frame->style.underline = psub->style.underline;
		frame->style.strikeout = psub->style.strikeout;
		frame->style.scalex = psub->style.scalex;
		frame->style.scaley = psub->style.scaley;
		frame->style.spacing = psub->style.spacing;
		frame->style.angle = psub->style.angle;
		frame->style.borderstyle = psub->style.borderstyle;
		frame->style.outline = psub->style.outline;
		frame->style.shadow = psub->style.shadow;
		frame->style.alignment = psub->style.alignment;
		frame->style.marginl = psub->style.marginl;
		frame->style.marginr = psub->style.marginr;
		frame->style.marginv = psub->style.marginv;
		frame->style.alphalevel = psub->style.alphalevel;
		frame->style.encoding = psub->style.encoding;
	}
	frame->data.dialogue_marginl = psub->data.dialogue_marginl;
	frame->data.dialogue_marginr = psub->data.dialogue_marginr;
	frame->data.dialogue_marginv = psub->data.dialogue_marginv;
	frame->data.sub_effect_type = psub->data.sub_effect_type;
	frame->data.effectstartx = psub->data.effectstartx;
	frame->data.effectendx = psub->data.effectendx;
	frame->data.effectstarty = psub->data.effectstarty;
	frame->data.effectendy = psub->data.effectendy;
	frame->data.effectdelaytime = psub->data.effectdelaytime;
	frame->data.datasize = psub->data.datasize;
	frame->data.pdata = (char *)psub->data.pdata;
	frame->data.type = psub->data.type;
#if 0
	printf_sub_info(frame);
#endif
	return 0;
}
static int set_app_frame(frame_t *frame,
	app_cb_private_t *port_private, OMX_BUFFERHEADERTYPE *buffer)
{
	omx_extra_data_t extra;
	port_info_t *port;

	oscl_param_check((frame && buffer && port_private), -1, NULL);
	port = port_private->port;

	memset(frame, 0, sizeof(frame_t));
	frame->priv_data = port_private;
	frame->header = buffer;
	if (port->domain == OMX_PortDomainVideo) {
		frame->type = STREAM_TYPE_VIDEO;
		memcpy(&frame->info.video, &port_private->info.video,
			sizeof(video_frame_info_t));
		frame->info.video.addr[0] = buffer->pBuffer;
		if (frame->info.video.size[1])
			frame->info.video.addr[1] = frame->info.video.addr[0]
						    + frame->info.video.size[0];
		if (frame->info.video.size[2])
			frame->info.video.addr[2] = frame->info.video.addr[1]
						    + frame->info.video.size[1];
		frame->info.video.time_stamp = buffer->nTimeStamp * 1000;

		memset(&extra, 0, sizeof(extra));
		memcpy(&extra,
			buffer->pBuffer + buffer->nAllocLen - sizeof(omx_extra_data_t),
			sizeof(omx_extra_data_t));
		printf_extra_info(&extra);

		if (1 == extra.isp_inf.valid) {
			video_isp_info_t *info = &frame->info.video.isp_info;

			info->valid = 1;
			info->exposure_time = extra.isp_inf.exposure_time;
			info->shutter_speed = extra.isp_inf.shutter_speed;
			info->aperture = extra.isp_inf.aperture;
			info->brightness = extra.isp_inf.brightness;
			info->exposure_bias = extra.isp_inf.exposure_bias;
			info->iso_speed = extra.isp_inf.iso_speed;
			info->flash = extra.isp_inf.flash;
			info->illu_id = extra.isp_inf.illu_id;
			info->back_score = extra.isp_inf.back_score;
			if (sizeof(info->res) == sizeof(extra.isp_inf.res))
				memcpy(info->res, extra.isp_inf.res, sizeof(info->res));
		}
	}
	if (port->domain == OMX_PortDomainSubtitle) {
		av_subtitle_t *p_sub = (av_subtitle_t *)buffer->pBuffer;
		OMX_STATETYPE com_state = OMX_StateInvalid;
		comp_info_t *hcom = (comp_info_t *)(port->comp_info);
		/*OSCL_LOGE("component name: %s", hcom->name);*/
		com_state = hcom->state;
		if (com_state != OMX_StateExecuting && com_state != OMX_StatePause) {
			frame->type = STREAM_TYPE_UNKNOWN;
			return -1;
		} else {
			frame->type = STREAM_TYPE_SUBTITLE;
			copy_sub_info(&frame->info.subtitle, p_sub);
		}
	}
	return 0;
}

int init_cb_info(port_info_t *port)
{
	int ret = 0;
	app_cb_private_t *cb_private;
	OMX_PARAM_PORTDEFINITIONTYPE port_def;

	oscl_param_check((port != NULL), OMX_ErrorBadParameter, NULL);
	if (port->cb_async.buf_handle != NULL && port->cb_filter.buf_handle != NULL) {
		OSCL_LOGE("do not support filter in async cb port");
		return -1;
	}

	/* no cb is set */
	if (port->cb_async.buf_handle == NULL && port->cb_filter.buf_handle == NULL)
		return 0;

	/* port is not setup yet */
	if (port->state != PORT_STATE_UNTUN_SETUP)
		return 0;

	if (port->priv_data) {
		OSCL_LOGE("cb info has already init! check!");
		return 0;
	}
	port->priv_data = oscl_zalloc(sizeof(app_cb_private_t));
	oscl_param_check_exit(NULL != port->priv_data,
			OMX_ErrorInsufficientResources, NULL);

	port->priv_data->port = port;
	port->priv_data->cb_frame = oscl_zalloc(port->nbuffer * sizeof(frame_t));
	oscl_param_check_exit(NULL != port->priv_data->cb_frame,
			OMX_ErrorInsufficientResources, NULL);

	cb_private = port->priv_data;

	/* save output config to private */
	port_def.nPortIndex = port->index;
	port_def.nVersion.nVersion = OMX_VERSION;
	ret = OMX_GetParameter(((comp_info_t *)port->comp_info)->cmp_hdl,
			OMX_IndexParamPortDefinition, &port_def);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);
	OSCL_LOGI("port_def.eDomain:%d", port_def.eDomain);
	if (port_def.eDomain == OMX_PortDomainVideo) {
		cb_private->info.video.height = port_def.format.video.nFrameHeight;
		cb_private->info.video.width = port_def.format.video.nFrameWidth;
		cb_private->info.video.stride = port_def.format.video.nStride;
		cb_private->info.video.sliceHeight = port_def.format.video.nSliceHeight;
		cb_private->info.video.color_fmt = port_def.format.video.eColorFormat;
		cb_private->info.video.planar = 2;
		calc_frame_size(cb_private->info.video.color_fmt,
			cb_private->info.video.stride,
			cb_private->info.video.sliceHeight,
			(OMX_U32 *)cb_private->info.video.size,
			port_def.nBufferAlignment);
	}

EXIT:
	return ret;
}

static int cb_async_buffer_free(port_info_t *port)
{
	int i;
	int wait_cnt = 10;
	if (port->priv_data == NULL || port->priv_data->cb_frame == NULL)
		return 0;
	for (i = 0; i < port->nbuffer; i++) {
		if (port->priv_data->cb_frame[i].header != NULL) {
			OSCL_LOGE("framebuffer[%d]:%p not freed by app",
				i, port->priv_data->cb_frame[i].header);
			oscl_mdelay(100);
			if (wait_cnt--)
				i--;
			else
				app_empty_buffer_done(&port->priv_data->cb_frame[i]);
		}
	}
	oscl_free(port->priv_data->cb_frame);
	port->priv_data->cb_frame = NULL;
	return 0;
}

int deinit_cb_info(port_info_t *port)
{
	if (port == NULL || port->priv_data == NULL)
		return 0;
	cb_async_buffer_free(port);
	oscl_free(port->priv_data);
	port->priv_data = NULL;
	return 0;
}


static int app_empty_this_buffer(port_info_t *port, OMX_BUFFERHEADERTYPE *buffer)
{
	int ret;
	frame_t *frame = NULL;
	int i;
	app_cb_private_t *port_private;

	port_private = port->priv_data;
	for (i = 0; i < port->nbuffer; i++) {
		if (port_private->cb_frame[i].header == NULL) {
			frame = &port_private->cb_frame[i];
			break;
		}
	}
	if (frame == NULL) {
		OSCL_LOGE("====port_private->cb_frame====");
		for (i = 0; i < port->nbuffer; i++) {
			OSCL_LOGE("cb frame %d. %p:%p", i, &port_private->cb_frame[i],
				port_private->cb_frame[i].header);
		}
		OSCL_LOGE("====port frame====");
		for (i = 0; i < port->nbuffer; i++)
			OSCL_LOGE("port frame %d. %p", i, port->header[i]);
		OSCL_LOGE("%s can not find free cb_frame when empty %p",
			(((comp_info_t *)(port->comp_info))->name), buffer);
		return -1;
	}
	if (i == port->nbuffer - 1)
		OSCL_LOGE("====last empty cb frame, check app return frames");

	ret = set_app_frame(frame, port_private, buffer);
	if (ret == 0)
		ret = port->cb_async.buf_handle(port->cb_async.app_data, frame);
	if (ret != 0) {
		OSCL_LOGE("cb_async.buf_handle return failed:%d", ret);
		frame->header = NULL;
	}
	if (port->cb_async.type == VS_IMAGE_FRAME ||
			port->cb_async.type == VS_IMAGE_ROT_FRAME) {
		memset(&port->cb_async, 0, sizeof(app_frame_cb_t));
	}

	return ret;
}

int app_empty_buffer_done(frame_t *frame)
{
	int ret = -1;
	app_cb_private_t *port_private;
	comp_info_t *comp;
	port_info_t *port;
	int i;
	void *header = NULL;

	oscl_param_check((frame != NULL && frame->priv_data != NULL), -1, NULL);
	port_private = frame->priv_data;
	port = port_private->port;
	oscl_param_check((port != NULL && frame->header != NULL),
			-1, NULL);

	comp = port->comp_info;

	for (i = 0; i < port->nbuffer; i++) {
		if (port_private->cb_frame[i].header == frame->header) {
			header = frame->header;
			port_private->cb_frame[i].header = NULL;
			break;
		}
	}
	if (i >= port->nbuffer)
		OSCL_LOGE("Fatal error! unknown header %p", frame->header);

	/* empty buffer done, ask output port fill this buffer */
	if (header)
		ret = OMX_FillThisBuffer(comp->cmp_hdl, header);
	return ret;
}

int app_callback_handle(port_info_t *port, OMX_BUFFERHEADERTYPE *buffer,
			port_cb_event_e event)
{
	int ret = APP_BUFFER_PASS_ON;
	frame_t frame;
	comp_info_t *comp;

	if (port->priv_data == NULL)
		return APP_BUFFER_PASS_ON;
	if (event == OUTPORT_FILL_DONE) {
		if (port->cb_async.buf_handle) {
			ret = app_empty_this_buffer(port, buffer);
			if (ret == 0)
				ret = APP_BUFFER_ASYNC;
			else
				ret = APP_BUFFER_HANDLED;
		}
		if (port->cb_filter.buf_handle) {
			set_app_frame(&frame, port->priv_data, buffer);
			ret = port->cb_filter.buf_handle(port->cb_async.app_data, &frame);
			oscl_cache_flush_vir(buffer->pBuffer);
			/* currently not support async in filter */
			if (ret != APP_BUFFER_HANDLED)
				ret = APP_BUFFER_PASS_ON;
		}
	}
	if (ret == APP_BUFFER_HANDLED) {
		comp = port->comp_info;
		OMX_FillThisBuffer(comp->cmp_hdl, buffer);
	}
	return ret;
}

int untunnel_setup_cb(port_info_t *port, app_frame_cb_t *cb)
{
	int ret = 0;
	int i;
	comp_info_t *comp;

	oscl_param_check((port != NULL), OMX_ErrorBadParameter, NULL);
	OSCL_LOGI("out_port:%p(%p)", port, port->comp_info);
	comp = port->comp_info;
	port->tunnel_hdl = NULL;
	port->connect_port = NULL;
	port->state = PORT_STATE_UNTUN_SETUP;
	/* require buffer : init header array */
	if (port->header != NULL) {
		OSCL_LOGE("err! header is not null while set up ports!");
		oscl_free(port->header);
	}
	port->header = oscl_zalloc(port->nbuffer * sizeof(OMX_BUFFERHEADERTYPE *));
	oscl_param_check_exit(NULL != port->header,
			OMX_ErrorInsufficientResources, NULL);

	/* require buffer : allocate buffer in output port */
	for (i = 0; i < port->nbuffer; i++) {
		OSCL_LOGI("=i:%d==", i);
		port->header[i] = NULL;
		ret = OMX_AllocateBuffer(comp->cmp_hdl, &port->header[i],
				port->index, comp, port->buf_size);
		OSCL_LOGI("=====AllocBuffer %d(p%d), %p(%p)", i, port->index,
				port->header[i], port->header[i]->pBuffer);
		if (ret != OMX_ErrorNone) {
			port->nbuffer = i;
			OSCL_LOGE("get Buffer %d on port %d fail, size:%d",
				i, port->index, port->buf_size);
			break;
		}
		port->header[i]->nInputPortIndex = -1;
	}

	for (i = 0; (i < port->nbuffer) && (port->header[i]); i++) {
		ret = OMX_FillThisBuffer(comp->cmp_hdl, port->header[i]);
		OSCL_LOGI("OMX_FillThisBuffer %d on port %d", i, port->index);
		if (ret != OMX_ErrorNone) {
			OSCL_LOGE("fill Buffer %d on port %d fail", i, port->index);
			break;
		}
	}

	memset(&port->cb_async, 0, sizeof(app_frame_cb_t));
	if (cb)
		memcpy(&port->cb_async, cb, sizeof(app_frame_cb_t));

	ret = init_cb_info(port);
EXIT:
	if (ret != 0)
		untunnel_unset_cb(port);
	return ret;
}

void untunnel_unset_cb(port_info_t *out_port)
{
	untunnel_unset_ports(out_port, NULL);
	memset(&out_port->cb_async, 0, sizeof(out_port->cb_async));
}

int untunnel_set_filter(port_info_t *port, app_port_filter_t *filter)
{
	int ret = 0;
	comp_info_t *comp;

	oscl_param_check((port != NULL), OMX_ErrorBadParameter, NULL);
	OSCL_LOGI("out_port:%p(%p)", port, port->comp_info);
	comp = port->comp_info;

	pthread_mutex_lock(&comp->state_lock);
	memset(&port->cb_filter, 0, sizeof(app_port_filter_t));
	if (filter)
		memcpy(&port->cb_filter, filter, sizeof(app_port_filter_t));
	ret = init_cb_info(port);
	pthread_mutex_unlock(&comp->state_lock);

	return ret;
}


