/*
 * vrender_component.c
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

#define DBG_LEVEL		DBG_INFO
#define LOG_TAG			"vrend_component"

#include <pthread.h>
#include <oscl.h>
#include <base_component.h>
#include <omx_vendor_lb.h>
#include "vrender_component.h"
#include "clock_component.h"
#include "clock_port.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(_A) (sizeof(_A) / sizeof((_A)[0]))
#endif /* ARRAY_SIZE */

#define SINK_INPUT_DEFAULT_SIZE (640 * 480 + ((640 * 480) >> 1))
#define VRENDER_ROTATE_BUFFER_NUM	(8)
#define DEFAULT_DROP_CNT		2
#define DEFAULT_DROPFRAME_TIMEDIFF	1000000
#define DEFAULT_THRESH                  10000
#define MAX_DELAY_TIME_DIFF             3000
#ifdef __linux__
#define CACHED_FRAME_CNT		3
#define OFFLINE_SE_CACHE		CACHED_FRAME_CNT
#else
#define CACHED_FRAME_CNT		2
#define OFFLINE_SE_CACHE		CACHED_FRAME_CNT

#endif

typedef struct vrender_private {
	void		*disp_hdl;	/* display device handle */
	void		*layer;		/* window handle */
	disp_size_t	scn_size;	/* screen size */
	layer_data_t	layer_data;	/* window display parameter to driver */
	vdisp_para_t	disp_para;	/* window display parameter to user */
	oscl_queue_t	release_queue;	/* picture save queue */
	OMX_S32		y_size;
	OMX_S32		cb_size;
	OMX_S32		cr_size;
	OMX_S32		first_frame;
	OMX_S32		win_change;
	OMX_S32		is_release;
	OMX_TIME_CLOCKSTATE clk_state; /* clk state */
	OMX_S32             xscale;    /* scale of the clk */
	OMX_BOOL            drop_flag;
	OMX_U32             drop_count;
	OMX_BOOL            avsync;
	OMX_S32		rotate_mode;	/* rotate_mode_e */
	OMX_BOOL	video_change;
	OMX_STATETYPE	wait_stat;

	scale_mode_e	scale_mode;
	OMX_COLOR_FORMATTYPE scale_out_format;
	scale_buffer_t  scale_buf[OFFLINE_SE_CACHE];
	OMX_U32		scale_buf_index;

	OMX_BOOL buf_queued;

	OMX_BOOL clkbuf_thd_run;
	pthread_t clkbuf_thd;
	int drop_frm_cnt;

	pthread_cond_t cond;
	pthread_mutex_t lock;
	OMX_BOOL sync_wait;
} vrender_private_t;

static void scale_out_buffer_free(vrender_private_t *sink_private)
{
	OMX_S32 i = 0, j = 0;
	scale_buffer_t *se_buf = NULL;

	for (i = 0; i < ARRAY_SIZE(sink_private->scale_buf); i++) {
		se_buf = &sink_private->scale_buf[i];
		for (j = 0; j < 3; j++) {
			if (se_buf->addr[j]) {
				oscl_free_align(se_buf->addr[j]);
				se_buf->addr[j] = NULL;
			}
		}
	}
}

static OMX_S32 scale_out_buffer_malloc(vrender_private_t *sink_private,
		disp_rect_t *rect, OMX_S32 align)
{
	OMX_S32 i = 0, j = 0;
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	OMX_U32 plane_size[3] = {0};
	scale_buffer_t *se_buf = NULL;

	for (i = 0; i < ARRAY_SIZE(sink_private->scale_buf); i++) {
		se_buf = &sink_private->scale_buf[i];
		ret = calc_frame_size(sink_private->scale_out_format,
					rect->width,
					rect->height,
					plane_size,
					align);
		oscl_param_check((OMX_ErrorNone == ret), ret, NULL);
		memset(se_buf, 0, sizeof(scale_buffer_t));
		se_buf->index = i;
		se_buf->fb_size.width = rect->width;
		se_buf->fb_size.height = rect->height;
		se_buf->crop_rect.left = 0;
		se_buf->crop_rect.top = 0;
		se_buf->crop_rect.width = rect->width;
		se_buf->crop_rect.height = rect->height;
		se_buf->eColorFormat = sink_private->scale_out_format;

		for (j = 0; j < 3; j++) {
			if (se_buf->addr[j]) {
				oscl_free_align(se_buf->addr[j]);
				se_buf->addr[j] = NULL;
			}
			if (plane_size[j]) {
				se_buf->size[j] = plane_size[j];
				se_buf->addr[j] = oscl_malloc_align(plane_size[j], align);
				OSCL_LOGI("secout buff[%d]: size:%d, addr:%p align:%d",
					j, plane_size[j], se_buf->addr[j], align);
				oscl_param_check_exit((NULL != se_buf->addr[j]),
					OMX_ErrorInsufficientResources, NULL);
			}
		}
	}
EXIT:
	if (ret != OMX_ErrorNone)
		scale_out_buffer_free(sink_private);
	return ret;
}

static OMX_ERRORTYPE vrender_config_crop_para(vdisp_para_t *disp_para,
			disp_rect_t *rect, layer_data_t *dst_cfg)
{
	OMX_S32 ratio = 0;
#if 0
	dst_cfg->src_size.width = rect->width + rect->left;
	dst_cfg->src_size.height = rect->height + rect->top;
#endif
	OSCL_LOGI("disp_para->mode:%d", disp_para->mode);
	switch (disp_para->mode) {
	case VDISP_WINDOW_ORIGINAL:
	case VDISP_WINDOW_FULL_SCREEN_VIDEO_RATIO:
	case VDISP_WINDOW_4R3MODE:
	case VDISP_WINDOW_16R9MODE:
		dst_cfg->crop_rect.left = rect->left;
		dst_cfg->crop_rect.top = rect->top;
		dst_cfg->crop_rect.width = rect->width;
		dst_cfg->crop_rect.height = rect->height;
		break;
	case VDISP_WINDOW_FULL_SCREEN_SCREEN_RATIO:
		dst_cfg->crop_rect.left = rect->left;
		dst_cfg->crop_rect.top = rect->top;
		ratio = (disp_para->win_rect.width * 10000) / disp_para->win_rect.height;
		if (ratio > (rect->width * 10000) / rect->height) {
			dst_cfg->crop_rect.width = rect->width;
			dst_cfg->crop_rect.height = (rect->width * 10000) / ratio;
		} else if (ratio == (rect->width * 10000) / rect->height) {
			dst_cfg->crop_rect.width = rect->width;
			dst_cfg->crop_rect.height = rect->height;
		} else {
			dst_cfg->crop_rect.height = rect->height;
			dst_cfg->crop_rect.width = (rect->height * ratio) / 10000;
		}

		if (dst_cfg->crop_rect.width > rect->width)
			dst_cfg->crop_rect.width = rect->width;
		if (dst_cfg->crop_rect.height > rect->height)
			dst_cfg->crop_rect.height = rect->height;
		dst_cfg->crop_rect.left = rect->left +
			((rect->width - dst_cfg->crop_rect.width) >> 1);
		dst_cfg->crop_rect.top = rect->top +
			((rect->height - dst_cfg->crop_rect.height) >> 1);
		break;
	case VDISP_WINDOW_CUTEDGE:
		dst_cfg->crop_rect.left = rect->left;
		dst_cfg->crop_rect.top = rect->top;
		dst_cfg->crop_rect.width = rect->width;
		dst_cfg->crop_rect.height = rect->height;
		if (disp_para->y_cut_ratio > 0) {
			dst_cfg->crop_rect.height = rect->height *
				disp_para->y_cut_ratio / 100;
			dst_cfg->crop_rect.top = dst_cfg->crop_rect.top +
				((rect->height - dst_cfg->crop_rect.height) >> 1);
		}
		if (disp_para->x_cut_ratio > 0) {
			dst_cfg->crop_rect.width = rect->width *
				disp_para->x_cut_ratio / 100;
			dst_cfg->crop_rect.left = dst_cfg->crop_rect.left +
				((rect->width - dst_cfg->crop_rect.width) >> 1);
		}
		break;

	default:
		OSCL_LOGE("Input display mode not support. %d",
			disp_para->mode);
		return OMX_ErrorBadParameter;
	}

	return OMX_ErrorNone;
}

static OMX_ERRORTYPE vrender_config_win_para(vrender_private_t *sink_private,
							layer_data_t *layer_cfg)
{
	OMX_S32 ratio = 0;
	disp_rect_t *pic_rect = NULL;
	disp_rect_t *src_win = NULL;
	disp_rect_t *dst_win = NULL;

	OSCL_LOGI("disp_mode:%d", sink_private->disp_para.mode);
	pic_rect = &layer_cfg->crop_rect;
	src_win = &sink_private->disp_para.win_rect;
	dst_win = &layer_cfg->win_rect;
	if (VDISP_WINDOW_ORIGINAL == sink_private->disp_para.mode) {
		if ((pic_rect->width > src_win->width) ||
			(pic_rect->height > src_win->height)) {

			/* if picture is large than screen, need scaler */
			sink_private->disp_para.mode =
					VDISP_WINDOW_FULL_SCREEN_VIDEO_RATIO;
		}
	}
	switch (sink_private->disp_para.mode) {
	case VDISP_WINDOW_ORIGINAL:
		dst_win->left = (src_win->width - pic_rect->width) >> 1;
		dst_win->top = (src_win->height - pic_rect->height) >> 1;
		dst_win->width = pic_rect->width;
		dst_win->height = pic_rect->height;
		break;
	case VDISP_WINDOW_FULL_SCREEN_VIDEO_RATIO:
		dst_win->left = src_win->left;
		dst_win->top = src_win->top;
		ratio = (pic_rect->width * 10000) / pic_rect->height;
		if (ratio > (src_win->width * 10000) / pic_rect->height) {
			dst_win->width = src_win->width;
			dst_win->height = (src_win->width * 10000) / ratio;
		} else if (ratio == (src_win->width * 10000) / src_win->height) {
			dst_win->width = src_win->width;
			dst_win->height = src_win->height;
		} else {
			dst_win->height = src_win->height;
			dst_win->width = (src_win->height * ratio) / 10000;
		}

		if (dst_win->width > src_win->width)
			dst_win->width = src_win->width;
		if (dst_win->height > src_win->height)
			dst_win->height = src_win->height;
		dst_win->left = src_win->left +
			((src_win->width - dst_win->width) >> 1);
		dst_win->top = src_win->top +
			((src_win->height - dst_win->height) >> 1);
		break;
	case VDISP_WINDOW_FULL_SCREEN_SCREEN_RATIO:
	case VDISP_WINDOW_CUTEDGE:
		dst_win->left = src_win->left;
		dst_win->top = src_win->top;
		dst_win->width = src_win->width;
		dst_win->height = src_win->height;
		break;
	case VDISP_WINDOW_4R3MODE:
		if ((VDISP_ROTATE_90 != sink_private->rotate_mode)
			&& (VDISP_ROTATE_270 != sink_private->rotate_mode)) {
			if ((src_win->width * 3) > (src_win->height * 4)) {
				dst_win->width = (src_win->height * 4) / 3;
				dst_win->height = src_win->height;
				dst_win->left = src_win->left +
					((src_win->width - dst_win->width) >> 1);
				dst_win->top = src_win->top;
			} else {
				dst_win->width = src_win->width;
				dst_win->height = (src_win->width * 3) >> 2;
				dst_win->left = src_win->left;
				dst_win->top = src_win->top +
					((src_win->height - dst_win->height) >> 1);
			}
		} else { /* if rotate 90 or 270, use 3:4 ratio */
			if (src_win->width * 4 > src_win->height * 3) {
				dst_win->width = (src_win->height * 3) / 4;
				dst_win->height = src_win->height;
				dst_win->left = src_win->left +
					((src_win->width - dst_win->width) >> 1);
				dst_win->top = src_win->top;
			} else {
				dst_win->width = src_win->width;
				dst_win->height = (src_win->width * 4) / 3;
				dst_win->left = src_win->left;
				dst_win->top = src_win->top +
					((src_win->height - dst_win->height) >> 1);
			}
		}
		break;
	case VDISP_WINDOW_16R9MODE:
		if ((VDISP_ROTATE_90 != sink_private->rotate_mode)
			&& (VDISP_ROTATE_270 != sink_private->rotate_mode)) {
			if ((src_win->width * 9) > (src_win->height * 16)) {
				dst_win->width = (src_win->height * 16) / 9;
				dst_win->height = src_win->height;
				dst_win->left = src_win->left +
					((src_win->width - dst_win->width) >> 1);
				dst_win->top = src_win->top;
			} else {
				dst_win->width = src_win->width;
				dst_win->height = (src_win->width * 9) / 16;
				dst_win->left = src_win->left;
				dst_win->top = src_win->top +
					((src_win->height - dst_win->height) >> 1);
			}
		} else { /* if rotate 90 or 270, use 9:16 ratio */
			if (src_win->width * 16 > src_win->height * 9) {
				dst_win->width = (src_win->height * 9) / 16;
				dst_win->height = src_win->height;
				dst_win->left = src_win->left +
					((src_win->width - dst_win->width) >> 1);
				dst_win->top = src_win->top;
			} else {
				dst_win->width = src_win->width;
				dst_win->height = (src_win->width * 16) / 9;
				dst_win->left = src_win->left;
				dst_win->top = src_win->top +
					((src_win->height - dst_win->height) >> 1);
			}
		}
		break;
	default:
		OSCL_LOGE("Input display mode not support. %d",
			sink_private->disp_para.mode);
		return OMX_ErrorBadParameter;
	}

	OSCL_LOGI("rotate:%d scn:(left:%d top:%d) (w:%d h:%d)",
		sink_private->rotate_mode, src_win->left, src_win->top,
		src_win->width, src_win->height);
	OSCL_LOGI("window:(left:%d top:%d) (w:%d h:%d)",
		dst_win->left, dst_win->top,
		dst_win->width, dst_win->height);

	return OMX_ErrorNone;
}

static OMX_ERRORTYPE vrender_config_disp(vrender_private_t *sink_private,
	OMX_COLOR_FORMATTYPE eColorFormat, disp_rect_t *rect)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	layer_data_t *layer_data = &sink_private->layer_data;

	layer_data->eColorFormat = eColorFormat;
	if (VDISP_WINDOW_USERDEF != sink_private->disp_para.mode) {
		ret = vrender_config_crop_para(&sink_private->disp_para, rect,
							&sink_private->layer_data);
		oscl_param_check((OMX_ErrorNone == ret), ret, NULL);

		ret = vrender_config_win_para(sink_private, &sink_private->layer_data);
		oscl_param_check((OMX_ErrorNone == ret), ret, NULL);
	} else {
		memcpy(&layer_data->win_rect, &sink_private->disp_para.win_rect,
			sizeof(layer_data->win_rect));
		memcpy(&layer_data->crop_rect, &sink_private->disp_para.crop_rect,
			sizeof(layer_data->crop_rect));

#if 0
		layer_data->src_size.width = rect->width + rect->left;
		layer_data->src_size.height = rect->height + rect->top;
#endif
		if ((layer_data->crop_rect.width <= 0)
			|| (layer_data->crop_rect.height <= 0)) {
			layer_data->crop_rect.left = rect->left;
			layer_data->crop_rect.top = rect->top;
			layer_data->crop_rect.width = rect->width;
			layer_data->crop_rect.height = rect->height;
		}
	}
	OSCL_LOGI("Disp cfg>>mode:%d win[%d %d %d %d] src[%d %d] crop[%d %d %d %d]",
		sink_private->disp_para.mode,
		layer_data->win_rect.left, layer_data->win_rect.top,
		layer_data->win_rect.width, layer_data->win_rect.height,
		layer_data->src_size.width, layer_data->src_size.height,
		layer_data->crop_rect.left, layer_data->crop_rect.top,
		layer_data->crop_rect.width, layer_data->crop_rect.height);
	return OMX_ErrorNone;
}

static void *clkbuf_manager(void *param)
{
	lb_omx_component_t *comp = param;
	vrender_private_t *priv = comp->component_private;
	base_port_t *clkport = &comp->port[SINK_CLOCK_PORT_INDEX];
	OMX_BUFFERHEADERTYPE *clk_buffer = NULL;

	OSCL_LOGD("running");
	while (priv->clkbuf_thd_run) {
		sem_wait(&clkport->buffer_sem);
		clk_buffer = oscl_queue_dequeue(&clkport->buffer_queue);
		if (clk_buffer == NULL)
			continue;

		OMX_TIME_MEDIATIMETYPE *mediatime;
		mediatime = (OMX_TIME_MEDIATIMETYPE *)clk_buffer->pBuffer;
		if (mediatime->eUpdateType == OMX_TIME_UpdateScaleChanged) {
			priv->xscale = mediatime->xScale;
			OSCL_LOGI("got scale changed scale %d\n", priv->xscale);
		} else if (mediatime->eUpdateType == OMX_TIME_UpdateClockStateChanged) {
			priv->clk_state = mediatime->eState;
			OSCL_LOGI("clock state change %d\n", priv->clk_state);
		}
		clkport->return_buffer(clkport, clk_buffer);
	}

	OSCL_LOGD("exit");
	return NULL;
}

static int clk_buf_init(lb_omx_component_t *comp)
{
	vrender_private_t *priv = comp->component_private;
	int ret;

	priv->clkbuf_thd_run = OMX_TRUE;
	ret = pthread_create(&priv->clkbuf_thd, NULL, clkbuf_manager, comp);
	if (ret != 0) {
		OSCL_LOGE(" create clkbuf_manager error!");
		return -1;
	}

	return 0;
}

static void clk_buf_deinit(lb_omx_component_t *comp)
{
	vrender_private_t *priv = comp->component_private;
	base_port_t *clkport = &comp->port[SINK_CLOCK_PORT_INDEX];

	OSCL_LOGD("joining clkbuf_thd");
	priv->clkbuf_thd_run = OMX_FALSE;
	sem_post(&clkport->buffer_sem);
	pthread_join(priv->clkbuf_thd, NULL);
	OSCL_LOGD("clkbuf_thd joined");
}

static OMX_BOOL sync_frame(OMX_HANDLETYPE stand_com,
	OMX_BUFFERHEADERTYPE *inbuffer)
{
	lb_omx_component_t *component = NULL;
	base_port_t *clkport = NULL;
	vrender_private_t *sink_private = NULL;
	OMX_HANDLETYPE                        clk_cmp = NULL;
	OMX_TIME_CONFIG_TIMESTAMPTYPE         cur_mediatime;
	OMX_TIME_CONFIG_TIMESTAMPTYPE         client_timestamp;
	OMX_ERRORTYPE                         err;
	OMX_BOOL                              sendframe = OMX_TRUE;
	OMX_TICKS mediatime_diff, walltime_diff;
	int scale;

	component = get_lb_component(stand_com);
	sink_private = component->component_private;
	clkport = &component->port[SINK_CLOCK_PORT_INDEX];
	clk_cmp = clkport->tunneled_component;

	/* if first timestamp is received, then notify clk component */
	if (inbuffer->nFlags & OMX_BUFFERFLAG_STARTTIME) {
		OSCL_LOGI("first timestamp received %lld\n", inbuffer->nTimeStamp);
		client_timestamp.nSize = sizeof(client_timestamp);
		client_timestamp.nPortIndex = clkport->tunnel_ports;
		client_timestamp.nTimestamp = inbuffer->nTimeStamp;
		err = OMX_SetConfig(clk_cmp,
			OMX_IndexConfigTimeClientStartTime, &client_timestamp);
		if (err != OMX_ErrorNone)
			OSCL_LOGE("set client start time error!\n");

		/* wait for state change notification */
		int i = 0;
		while (sink_private->clk_state != OMX_TIME_ClockStateRunning) {
			if (i++ > 1000) {
				OSCL_LOGW("wait clock running failed!");
				break;
			}
			oscl_mdelay(1);
		}
		/* first frame, send it anyway */
		return OMX_TRUE;
	}
	if (inbuffer->nFlags & OMX_BUFFERFLAG_EOS) {
		/* eos frame, send it anyway */
		return OMX_TRUE;
	}

	/* update mediatime */
	client_timestamp.nPortIndex = clkport->tunnel_ports;
	client_timestamp.nTimestamp = inbuffer->nTimeStamp;
	OSCL_LOGD("config current video timestamp %lld", client_timestamp.nTimestamp);
	err = OMX_SetConfig(clk_cmp,
		OMX_IndexConfigTimeCurrentVideoReference,
		&client_timestamp);
	if (err != OMX_ErrorNone)
		OSCL_LOGE("set video ref time error!\n");

	/* do not send the data to sink and return back,
	 * if the clock is not running */
	if (sink_private->clk_state != OMX_TIME_ClockStateRunning) {
		sink_private->drop_frm_cnt++;
		if (sink_private->drop_frm_cnt % 10 == 0) {
			OSCL_LOGI("clock is not running now, drop %d frames(ts=%lld)!",
				sink_private->drop_frm_cnt, inbuffer->nTimeStamp);
		}
		inbuffer->nFilledLen = 0;
		return OMX_FALSE;
	}

	/* get cur mediatime */
	err = OMX_GetConfig(clk_cmp, OMX_IndexConfigTimeCurrentMediaTime, &cur_mediatime);
	if (err != OMX_ErrorNone) {
		OSCL_LOGW("get cur mediatime err!\n");
		return OMX_TRUE;
	}
	mediatime_diff = inbuffer->nTimeStamp - cur_mediatime.nTimestamp;
	scale = sink_private->xscale >> 16;
	if (scale == 0) {
		OSCL_LOGW("avsync isn't working!");
		return OMX_TRUE;
	}
	OSCL_LOGD("scale=%d, buf-ts=%lld, cur-ts=%lld, diff=%lld", scale,
		inbuffer->nTimeStamp,  cur_mediatime.nTimestamp, mediatime_diff);
	if ((mediatime_diff < -DEFAULT_DROPFRAME_TIMEDIFF && scale > 0) ||
			(mediatime_diff > DEFAULT_DROPFRAME_TIMEDIFF && scale < 0)) {
		sendframe = OMX_FALSE;
		sink_private->drop_frm_cnt++;
		if (sink_private->drop_frm_cnt % 10 == 0) {
			OSCL_LOGI("drop %d frames, buf-ts %lld, cur-ts %lld, diff %lld",
			sink_private->drop_frm_cnt, inbuffer->nTimeStamp,
			cur_mediatime.nTimestamp, mediatime_diff);
		}
	} else {
		sink_private->drop_frm_cnt = 0;
		walltime_diff = mediatime_diff / scale;
		sendframe = OMX_TRUE;
		if (walltime_diff > DEFAULT_THRESH) {
			OMX_U32 delay_time = walltime_diff / 1000;
			if (delay_time > MAX_DELAY_TIME_DIFF)
				delay_time = MAX_DELAY_TIME_DIFF;
			OSCL_LOGD("delay time %d\n", delay_time);
			pthread_mutex_lock(&sink_private->lock);
			sink_private->sync_wait = OMX_TRUE;
			oscl_cond_timedwait_ms(
				&sink_private->cond, &sink_private->lock,
				delay_time);
			sink_private->sync_wait = OMX_FALSE;
			pthread_mutex_unlock(&sink_private->lock);
		}
	}

	return sendframe;
}

#if 0
/* this is a standard implement of omx for avsync, we use
** a simpler way as above right now, see sync_frame func */
static OMX_BOOL render_clock_port_handle(OMX_HANDLETYPE stand_com,
	OMX_BUFFERHEADERTYPE *inbuffer)
{
	lb_omx_component_t *component = NULL;
	base_port_t *inport = NULL;
	base_port_t *clkport = NULL;
	clock_port_private_t *clk_priv = NULL;
	vrender_private_t *sink_private = NULL;
	OMX_HANDLETYPE                        clk_cmp = NULL;
	OMX_BUFFERHEADERTYPE                  *clk_buffer = NULL;
	OMX_TIME_MEDIATIMETYPE                *mediatime;
	OMX_TIME_CONFIG_TIMESTAMPTYPE         cur_mediatime;
	OMX_TIME_CONFIG_TIMESTAMPTYPE         client_timestamp;
	OMX_ERRORTYPE                         err;
	OMX_BOOL                              sendframe;

	component = get_lb_component(stand_com);
	sink_private = component->component_private;
	inport = &component->port[SINK_INPUT_PORT_INDEX];
	clkport = &component->port[SINK_CLOCK_PORT_INDEX];
	clk_priv = (clock_port_private_t *)clkport->port_private;
	clk_cmp = clkport->tunneled_component;

	sendframe = OMX_FALSE;

	/* if first timestamp is received, then notify clk component */
	if (inbuffer->nFlags & OMX_BUFFERFLAG_STARTTIME) {
		OSCL_LOGI("first timestamp received %llx\n", inbuffer->nTimeStamp);
		inbuffer->nFlags = 0;
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
		sem_wait(&clkport->buffer_sem);

		clk_buffer = oscl_queue_dequeue(&clkport->buffer_queue);
		if (clk_buffer != NULL) {
			mediatime = (OMX_TIME_MEDIATIMETYPE *)clk_buffer->pBuffer;
			sink_private->clk_state = mediatime->eState;
			sink_private->xscale    = mediatime->xScale;
			clkport->return_buffer(clkport, clk_buffer);
			clk_buffer = NULL;
		}
		/* first frame, send it anyway */
		sendframe = OMX_TRUE;
		return sendframe;
	}

	/* do not send the data to sink and return back,
	 * if the clock is not running */
	if (sink_private->clk_state != OMX_TIME_ClockStateRunning) {
		inbuffer->nFilledLen = 0;
		sendframe = OMX_FALSE;
		return sendframe;
	}

	/* check for any scale change information from the clock component */
	if (sem_get(&clkport->buffer_sem) > 0) {
		OSCL_LOGI("sem value %d\n", sem_get(&clkport->buffer_sem));
		clk_buffer = oscl_queue_dequeue(&clkport->buffer_queue);
		sem_wait(&clkport->buffer_sem);
	}
	if (clk_buffer != NULL) {
		mediatime = (OMX_TIME_MEDIATIMETYPE *)clk_buffer->pBuffer;
		if (mediatime->eUpdateType == OMX_TIME_UpdateScaleChanged) {
			/* scale changed, update mediatime base */
			client_timestamp.nPortIndex = clkport->tunnel_ports;
			client_timestamp.nTimestamp = inbuffer->nTimeStamp;
			err = OMX_SetConfig(clk_cmp,
				OMX_IndexConfigTimeCurrentVideoReference,
				&client_timestamp);
			if (err != OMX_ErrorNone)
				OSCL_LOGE("set video ref time error!\n");
			/* we only drop frames in fast forward or backward mode */
			if (mediatime->xScale > (1 << 16) ||
				mediatime->xScale < (-1 << 16)) {
				sink_private->drop_flag = OMX_TRUE;
				sink_private->drop_count = 0;
			}
			sink_private->xscale = mediatime->xScale;
			OSCL_LOGI("got scale changed scale %d\n", sink_private->xscale);
		}
		clkport->return_buffer(clkport, clk_buffer);
	}

	/* drop next n frames on scale change
	 * and rebase clock time base, note: what is a good value for n */
	if (sink_private->drop_flag && sink_private->drop_count < DEFAULT_DROP_CNT) {
		OSCL_LOGI("=======drop one frame====\n");
		sink_private->drop_count++;
		if (sink_private->drop_count == DEFAULT_DROP_CNT) {
			client_timestamp.nPortIndex = clkport->tunnel_ports;
			client_timestamp.nTimestamp = inbuffer->nTimeStamp;
			err = OMX_SetConfig(clk_cmp,
				OMX_IndexConfigTimeCurrentVideoReference,
				&client_timestamp);
			if (err != OMX_ErrorNone)
				OSCL_LOGE("set video refclk time error!\n");
			sink_private->drop_flag = OMX_FALSE;
			sink_private->drop_count = 0;
			sendframe = OMX_TRUE;
		} else
			sendframe = OMX_FALSE;
		return sendframe;
	}

	/* frame is not to be dropped so send the request
	 * for the timestamp for the data delivery */
	if (!clkport->is_flushed && !inport->is_flushed) {
		clk_priv->mediatime_request.nMediaTimestamp = inbuffer->nTimeStamp;
		clk_priv->mediatime_request.nOffset = 100;
		clk_priv->mediatime_request.nPortIndex = clkport->tunnel_ports;
		/* fill approriate value later */
		clk_priv->mediatime_request.pClientPrivate = NULL;
		if (sem_get(&clkport->buffer_sem) > 0) {
			OSCL_LOGI("sem value %d\n", sem_get(&clkport->buffer_sem));
			clk_buffer = oscl_queue_dequeue(&clkport->buffer_queue);
			sem_wait(&clkport->buffer_sem);
			clkport->return_buffer(clkport, clk_buffer);
		}
		err = OMX_SetConfig(clk_cmp,
			OMX_IndexConfigTimeMediaTimeRequest,
			&clk_priv->mediatime_request);
		if (err != OMX_ErrorNone)
			OSCL_LOGE("set video refclk time error!\n");
		OSCL_LOGD("fullfillment sem value %d\n", sem_get(&clkport->buffer_sem));
		sem_wait(&clkport->buffer_sem);
		OSCL_LOGD("fullfillment has return\n");
		clk_buffer = oscl_queue_dequeue(&clkport->buffer_queue);
		if (clk_buffer != NULL) {
			mediatime = (OMX_TIME_MEDIATIMETYPE *)clk_buffer->pBuffer;
			if (mediatime->eUpdateType ==
				OMX_TIME_UpdateRequestFulfillment) {
				if (mediatime->nOffset > 0)
					sendframe = OMX_TRUE;
				else
					sendframe = OMX_FALSE;
			} else
				OSCL_LOGE("err update type %d\n", mediatime->eUpdateType);
			clkport->return_buffer(clkport, clk_buffer);
		}
		OSCL_LOGD("media time offset %lld\n", mediatime->nOffset);
	}
	return sendframe;
}
#endif

void vrender_buffer_handle(OMX_HANDLETYPE stand_com,
	OMX_BUFFERHEADERTYPE *inbuffer,
	OMX_BUFFERHEADERTYPE *outbuffer)
{
	vrender_private_t *sink_private = NULL;
	lb_omx_component_t *component = NULL;
	base_port_t *inport = NULL;
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	OMX_VIDEO_PORTDEFINITIONTYPE *video;
	OMX_COLOR_FORMATTYPE eColorFormat;

	component = get_lb_component(stand_com);
	inport = &component->port[SINK_INPUT_PORT_INDEX];
	sink_private = component->component_private;
	if (0 == inbuffer->nFilledLen)
		return;
	video = &inport->port_param.format.video;
	sink_private->layer_data.src_size.width = video->nStride;
	sink_private->layer_data.src_size.height = video->nSliceHeight;
	eColorFormat = inport->port_param.format.video.eColorFormat;

	if ((0 == sink_private->first_frame) ||
		(1 == sink_private->win_change) ||
		OMX_TRUE == sink_private->video_change) {
		disp_rect_t disp_area;

		disp_area.width = video->nFrameWidth;
		disp_area.height = video->nFrameHeight;
		if (VDISP_ROTATE_NONE == sink_private->rotate_mode) {
			disp_area.left = 0;
			disp_area.top = 0;
		} else if (VDISP_ROTATE_90 == sink_private->rotate_mode) {
			disp_area.left = video->nStride - video->nFrameWidth;
			disp_area.top = 0;
		} else if (VDISP_ROTATE_270 == sink_private->rotate_mode) {
			disp_area.left = 0;
			disp_area.top = video->nSliceHeight - video->nFrameHeight;
		} else if (VDISP_ROTATE_180 == sink_private->rotate_mode) {
			disp_area.left = video->nStride - video->nFrameWidth;
			disp_area.top = video->nSliceHeight - video->nFrameHeight;
		} else {
			disp_area.left = 0;
			disp_area.top = 0;
		}
		ret = vrender_config_disp(sink_private, eColorFormat, &disp_area);
		oscl_param_check_exit((OMX_ErrorNone == ret), ret, NULL);

		if (SCALE_MODE_OFFLINE == sink_private->scale_mode) {
			ret = oscl_open_scale_submodule(sink_private->disp_hdl);
			oscl_param_check_exit((OMX_ErrorNone == ret), ret, NULL);
			ret = scale_out_buffer_malloc(sink_private,
				&sink_private->layer_data.win_rect,
				inport->port_param.nBufferAlignment);
			if (OMX_ErrorNone != ret) {
				OSCL_LOGE("scale buffer malloc fail.");
				oscl_close_scale_submodule(sink_private->disp_hdl);
				goto EXIT;
			}
		}
		sink_private->first_frame = 1;
		sink_private->win_change = 0;
		sink_private->video_change = OMX_FALSE;
	}

	sink_private->layer_data.size[0] = sink_private->y_size;
	sink_private->layer_data.size[1] = sink_private->cb_size;
	sink_private->layer_data.size[2] = sink_private->cr_size;
	sink_private->layer_data.addr[0] = inbuffer->pBuffer;
	if (sink_private->cb_size) {
		sink_private->layer_data.addr[1] =
			inbuffer->pBuffer +
			sink_private->y_size;
	}
	if (sink_private->cr_size) {
		sink_private->layer_data.addr[2] =
			inbuffer->pBuffer +
			sink_private->y_size + sink_private->cb_size;
	}
	if (SCALE_MODE_OFFLINE == sink_private->scale_mode) {
		scale_buffer_t src, dst;
		OMX_U32 index = 0;

		if (sink_private->scale_buf_index >= OFFLINE_SE_CACHE)
			sink_private->scale_buf_index = 0;
		index = sink_private->scale_buf_index % OFFLINE_SE_CACHE;
		memcpy(&dst, &sink_private->scale_buf[index], sizeof(scale_buffer_t));
		memset(&src, 0, sizeof(scale_buffer_t));

		src.eColorFormat = eColorFormat;
		src.fb_size.width = sink_private->layer_data.src_size.width;
		src.fb_size.height = sink_private->layer_data.src_size.height;
		src.crop_rect.top = sink_private->layer_data.crop_rect.top;
		src.crop_rect.left = sink_private->layer_data.crop_rect.left;
		src.crop_rect.width = sink_private->layer_data.crop_rect.width;
		src.crop_rect.height = sink_private->layer_data.crop_rect.height;
		src.size[0] = sink_private->layer_data.size[0];
		src.size[1] = sink_private->layer_data.size[1];
		src.size[2] = sink_private->layer_data.size[2];
		src.addr[0] = sink_private->layer_data.addr[0];
		src.addr[1] = sink_private->layer_data.addr[1];
		src.addr[2] = sink_private->layer_data.addr[2];

		ret = oscl_disp_hw_scale(sink_private->disp_hdl, &src, &dst);
		if (OMX_ErrorNone == ret)
			sink_private->scale_buf_index++;
		inbuffer->nFilledLen = 0;

		sink_private->layer_data.src_size.width = dst.fb_size.width;
		sink_private->layer_data.src_size.height = dst.fb_size.height;
		sink_private->layer_data.crop_rect.top = dst.crop_rect.top;
		sink_private->layer_data.crop_rect.left = dst.crop_rect.left;
		sink_private->layer_data.crop_rect.width = dst.crop_rect.width;
		sink_private->layer_data.crop_rect.height = dst.crop_rect.height;
		sink_private->layer_data.eColorFormat = dst.eColorFormat;
		sink_private->layer_data.size[0] = dst.size[0];
		sink_private->layer_data.size[1] = dst.size[1];
		sink_private->layer_data.size[2] = dst.size[2];
		sink_private->layer_data.addr[0] = dst.addr[0];
		sink_private->layer_data.addr[1] = dst.addr[1];
		sink_private->layer_data.addr[2] = dst.addr[2];
	}
	oscl_disp_update_layer_data(sink_private->disp_hdl,
			sink_private->layer, &sink_private->layer_data);
	inbuffer->nFilledLen = 0;
	if (SCALE_MODE_ONLINE == sink_private->scale_mode) {
		oscl_queue_queue(&sink_private->release_queue, inbuffer);
		sink_private->buf_queued = OMX_TRUE;
	}

EXIT:
	return;
}

void *vrender_buffer_manager(void *param)
{
	lb_omx_component_t *component = NULL;
	OMX_COMPONENTTYPE *stand_comp = (OMX_COMPONENTTYPE *)param;
	base_port_t *inport = NULL;
	OMX_BUFFERHEADERTYPE *inbuffer = NULL;
	OMX_BUFFERHEADERTYPE *return_buf = NULL;
	vrender_private_t *sink_private = NULL;
	int64_t wait_buf_time = oscl_get_msec();

	OSCL_TRACE(" %p\n", param);
	oscl_param_check((param != NULL), NULL, NULL);
	component = get_lb_component(stand_comp);
	inport = &component->port[SINK_INPUT_PORT_INDEX];
	sink_private = component->component_private;

	OSCL_LOGI("======comp:%s, queue len:%d header:%p", get_component_name(component),
		oscl_queue_get_num(&inport->buffer_queue), inport->header);
	while (1) {
		OMX_STATETYPE state = component->state;
		if (state != OMX_StateIdle
			&& state != OMX_StateExecuting
			&& state != OMX_StatePause) {
			OSCL_LOGD("comp-state=%d, state=%d, exit",
				component->state, state);
			break;
		}

		/*Wait till the ports are being flushed*/
		pthread_mutex_lock(&component->flush_mutex);
		while (inport->is_flushed) {
			pthread_mutex_unlock(&component->flush_mutex);
			do {
				if (inbuffer) {
					OSCL_LOGI("sink return_buffer %p", inbuffer);
					inport->return_buffer(inport, inbuffer);
					inbuffer = NULL;
				}
				inbuffer = oscl_queue_dequeue
					(&sink_private->release_queue);
			} while (inbuffer);
			sem_post(component->mgmt_flush_sem);
			OSCL_LOGI("%s sem_wait component->flush_sem",
				get_component_name(component));
			sem_wait(component->flush_sem);
			pthread_mutex_lock(&component->flush_mutex);
		}
		pthread_mutex_unlock(&component->flush_mutex);
		sink_private->wait_stat = component->state;
		if (component->state != OMX_StateExecuting) {
			OSCL_LOGD("QUEUE SIZE:%d %d", oscl_message_count
					(&sink_private->release_queue),
					oscl_message_count(&inport->buffer_queue));
			if (component->state == OMX_StateIdle) {
				do {
					if (inbuffer) {
						inport->return_buffer(inport, inbuffer);
						inbuffer = NULL;
					}
					inbuffer = oscl_queue_dequeue
						(&sink_private->release_queue);
				} while (inbuffer);
			}

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
						cur_time - wait_buf_time);
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

		if (sink_private->avsync &&
			!sync_frame(stand_comp, inbuffer)) {
			if (!(inbuffer->nFlags & OMX_BUFFERFLAG_EOS)) {
				inport->return_buffer(inport, inbuffer);
				inbuffer = NULL;
				OSCL_LOGD("skip one frame\n");
				continue;
			}
		}
		if (component->mark.hMarkTargetComponent) {
			inbuffer->hMarkTargetComponent
				= component->mark.hMarkTargetComponent;
			inbuffer->pMarkData
				= component->mark.pMarkData;
			component->mark.hMarkTargetComponent = NULL;
			component->mark.pMarkData = NULL;
		}

		OSCL_LOGD("comp:%s, state:%d,handle:%p,pOutputPortPrivate:%p",
			get_component_name(component),
			component->state,
			component->buf_handle, inbuffer->pOutputPortPrivate);
		sink_private->buf_queued = OMX_FALSE;
		if (component->state == OMX_StateExecuting) {
			if (component->buf_handle && inbuffer->nFilledLen > 0)
				(*(component->buf_handle))(stand_comp, inbuffer, NULL);
		}
		if (inbuffer->hMarkTargetComponent == stand_comp) {
			(*(component->callbacks.EventHandler))
				(stand_comp,
				component->callback_data,
				OMX_EventMark,
				0,
				0,
				inbuffer->pMarkData);
		}
		if (inbuffer->nFlags & OMX_BUFFERFLAG_STARTTIME) {
			OSCL_LOGI("start ts %lld", inbuffer->nTimeStamp);
			inbuffer->nFlags &= (~OMX_BUFFERFLAG_STARTTIME);
			(*(component->callbacks.EventHandler))
			(stand_comp, component->callback_data,
				OMX_EventOutputRendered, 0, 0, NULL);
		}
		if (inbuffer->nFlags & OMX_BUFFERFLAG_EOS && inbuffer->nFilledLen == 0) {
			OSCL_LOGI("EOS ts %lld", inbuffer->nTimeStamp);
			(*(component->callbacks.EventHandler))
			(stand_comp,
				component->callback_data,
				OMX_EventBufferFlag,
				inport->port_param.nPortIndex,
				OMX_BUFFERFLAG_EOS,
				NULL);
			component->eos_flag = OMX_TRUE;
		}
		if (!sink_private->buf_queued) {
			inbuffer->nFlags = 0;
			inport->return_buffer(inport, inbuffer);
			inbuffer = NULL;
			continue;
		}
		if (SCALE_MODE_ONLINE == sink_private->scale_mode) {
			OMX_S32 num = oscl_queue_get_num(&sink_private->release_queue);
			if ((component->target_state == OMX_StateLoaded) ||
				(num >= CACHED_FRAME_CNT)) {
				return_buf =
					oscl_queue_dequeue(&sink_private->release_queue);
				if (return_buf) {
					OSCL_LOGD("render return_buffer %p", return_buf);
					inport->return_buffer(inport, return_buf);
					return_buf = NULL;
				}
			}
			/* the buffer is already handle, set to null */
			if (inbuffer && inbuffer->nFilledLen == 0)
				inbuffer = NULL;
		} else {
			if (inbuffer && inbuffer->nFilledLen == 0) {
				inport->return_buffer(inport, inbuffer);
				inbuffer = NULL;
			}
		}
	}
	if ((inbuffer != NULL) && (!PORT_IS_SUPPLIER(inport))) {
		OSCL_LOGE("inport return_buffer before exit");
		inport->return_buffer(inport, inbuffer);
	}
	oscl_queue_flush(&sink_private->release_queue);
	oscl_queue_flush(&inport->buffer_queue);
#ifdef __linux__
	OSCL_LOGI("exit from vrender_buffer_manager:%lu\n", pthread_self());
#elif defined __EOS__
	OSCL_LOGI("exit from vrender_buffer_manager:%s\n", pthread_self()->name);
#endif
	OSCL_TRACE(" %p\n", param);

	return NULL;
}

OMX_ERRORTYPE vrender_set_parameter(OMX_IN OMX_HANDLETYPE hComp,
	OMX_IN OMX_INDEXTYPE paramIndex,
	OMX_IN OMX_PTR paramData)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	lb_omx_component_t *component = NULL;
	vrender_private_t *sink_private = NULL;

	OSCL_TRACE(" %p, %p , %x\n", hComp, paramData, paramIndex);
	oscl_param_check((hComp != NULL), OMX_ErrorBadParameter, NULL);
	component = get_lb_component(hComp);
	oscl_param_check((component != NULL), OMX_ErrorBadParameter, NULL);
	sink_private = component->component_private;

	if (NULL == sink_private->disp_hdl) {
		sink_private->disp_hdl = oscl_open_disp_engine();
		if (NULL == sink_private->disp_hdl) {
			OSCL_LOGE("Open display device fail.");
			return OMX_ErrorInsufficientResources;
		}
		ret = oscl_disp_get_screen_size(
				sink_private->disp_hdl, &sink_private->scn_size);
		if (OMX_ErrorNone != ret) {
			OSCL_LOGE("Get display screen size fail.");
			return OMX_ErrorInsufficientResources;
		}
	}
	switch (paramIndex) {
	case omx_index_vrender_mode: {
		disp_rect_t *win_rect = &sink_private->disp_para.win_rect;
		disp_size_t *scn_size = &sink_private->scn_size;

		memcpy(&sink_private->disp_para, paramData, sizeof(vdisp_para_t));
		oscl_param_check((win_rect->left >= 0), OMX_ErrorBadParameter, NULL);
		oscl_param_check((win_rect->top >= 0), OMX_ErrorBadParameter, NULL);
		oscl_param_check((win_rect->width > 0), OMX_ErrorBadParameter, NULL);
		oscl_param_check((win_rect->height > 0), OMX_ErrorBadParameter, NULL);
		if (((win_rect->left + win_rect->width) > scn_size->width) ||
			((win_rect->top + win_rect->height) > scn_size->height)) {
			OSCL_LOGE("Set display mode error. win[%d %d %d %d] scn[%d %d]",
				win_rect->left, win_rect->top,
				win_rect->width, win_rect->height,
				scn_size->width, scn_size->height);
			return OMX_ErrorBadParameter;
		}
		sink_private->win_change = 1;
		break;
	}
	case OMX_IndexConfigCommonRotate: {
		OMX_CONFIG_ROTATIONTYPE *rotationtype =
				(OMX_CONFIG_ROTATIONTYPE *)paramData;
		OSCL_LOGD("paramData:%p", paramData);
		oscl_param_check((NULL != rotationtype), OMX_ErrorBadParameter, NULL);
		sink_private->rotate_mode = rotationtype->nRotation;
		break;
	}
	case omx_index_lombo_video_size: {
		oscl_param_check((NULL != paramData), OMX_ErrorBadParameter, NULL);
		omx_size_t *size = (omx_size_t *)paramData;
		base_port_t *inport = &component->port[SINK_INPUT_PORT_INDEX];
		OMX_VIDEO_PORTDEFINITIONTYPE *video = &inport->port_param.format.video;
		if (size->width != video->nFrameWidth ||
			size->height != video->nFrameHeight ||
			size->stride != video->nStride ||
			size->slice_height != video->nSliceHeight) {
			sink_private->video_change = OMX_TRUE;
			video->nFrameWidth = size->width;
			video->nFrameHeight = size->height;
			video->nStride = size->stride;
			video->nSliceHeight = size->slice_height;
			if (sink_private->layer != NULL) {
				oscl_disp_release_layer(sink_private->disp_hdl,
					&sink_private->layer);
			}
			sink_private->layer =
				oscl_disp_request_layer(sink_private->disp_hdl);
			if (sink_private->layer == NULL) {
				OSCL_LOGE("request display layer error!");
				ret = OMX_ErrorInsufficientResources;
				break;
			}
		}
		break;
	}
	case omx_index_vrender_win_top:
		ret = oscl_disp_set_layer_top(sink_private->disp_hdl,
						sink_private->layer);
		break;
	case omx_index_vrender_win_bottom:
		ret = oscl_disp_set_layer_bottom(sink_private->disp_hdl,
						sink_private->layer);
		break;
	case omx_index_media_avsync:
		sink_private->avsync = *((OMX_BOOL *)paramData);
		OSCL_LOGI("avsync %d\n", sink_private->avsync);
		break;
	case omx_index_vrender_disable_display:
		oscl_disp_disable_layer(sink_private->disp_hdl, &sink_private->layer);
		break;
	case omx_index_vrender_enable_display:
		oscl_disp_enable_layer(sink_private->disp_hdl, &sink_private->layer);
		break;
	case omx_index_vrender_scale_mode:
		oscl_param_check((component->state != OMX_StateExecuting),
			OMX_ErrorBadParameter, NULL);
		sink_private->scale_mode = *((scale_mode_e *)paramData);
		break;
	default:
		ret = base_set_parameter(hComp, paramIndex, paramData);
		break;
	}
	OSCL_TRACE(" %d\n", ret);

	return ret;
}

OMX_ERRORTYPE vrender_set_state(OMX_HANDLETYPE hComp,
	OMX_U32 dest_state)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	lb_omx_component_t *component = NULL;
	vrender_private_t *sink_private = NULL;
	base_port_t *inport = NULL;
	OMX_U32 pre_state = 0;
	OMX_U32 plane_size[3] = {0};

	OSCL_TRACE(" %p, %x\n", hComp, dest_state);
	oscl_param_check(hComp != NULL, OMX_ErrorBadParameter, NULL);
	component = get_lb_component(hComp);
	sink_private = component->component_private;
	inport = &component->port[SINK_INPUT_PORT_INDEX];

	OSCL_LOGI("%s->%s", omx_state_as_string(component->state),
		omx_state_as_string(dest_state));

	if (dest_state == OMX_StateExecuting && component->state == OMX_StateIdle) {
		OMX_VIDEO_PORTDEFINITIONTYPE *video = &inport->port_param.format.video;

		sink_private->layer = oscl_disp_request_layer(sink_private->disp_hdl);
		oscl_param_check((NULL != sink_private->layer),
					OMX_ErrorInsufficientResources, NULL);
		if ((video->nFrameWidth > video->nStride) ||
			(video->nFrameHeight > video->nSliceHeight)) {
			OSCL_LOGE("Display size larger than stride size. (%d %d %d %d)",
				video->nFrameWidth,
				video->nFrameHeight,
				video->nStride,
				video->nSliceHeight);
			return OMX_ErrorInsufficientResources;
		}
		ret = calc_frame_size(video->eColorFormat,
				video->nStride,
				video->nSliceHeight,
				plane_size, inport->port_param.nBufferAlignment);
		OSCL_LOGI("nBufferAlignment:%d, ret:%d, (%d %d)(%d %d %d)",
			inport->port_param.nBufferAlignment, ret,
			video->nStride,
			video->nSliceHeight,
			plane_size[0], plane_size[1], plane_size[2]);
		oscl_param_check((OMX_ErrorNone == ret), ret, NULL);
		sink_private->y_size = plane_size[0];
		sink_private->cb_size = plane_size[1];
		sink_private->cr_size = plane_size[2];
	}
	pre_state = component->state;
	ret = base_component_set_state(hComp, dest_state);
	if ((dest_state == OMX_StatePause) && (pre_state == OMX_StateExecuting)) {
		if (sink_private->sync_wait) {
			OSCL_LOGD("wait pause");
			pthread_mutex_lock(&sink_private->lock);
			pthread_cond_signal(&sink_private->cond);
			pthread_mutex_unlock(&sink_private->lock);
		}
		while (sink_private->wait_stat != OMX_StatePause)
			oscl_mdelay(10);
	}
	if (dest_state == OMX_StateIdle && (pre_state == OMX_StateExecuting ||
		pre_state == OMX_StatePause)) {
		oscl_disp_release_layer(sink_private->disp_hdl, &sink_private->layer);
		sink_private->layer = NULL;
		scale_out_buffer_free(sink_private);
		sink_private->first_frame = 0;
		oscl_close_scale_submodule(sink_private->disp_hdl);
	}
	return ret;
}

OMX_ERRORTYPE vrender_component_deinit(OMX_IN OMX_HANDLETYPE hComponent)
{
	lb_omx_component_t *component = NULL;
	vrender_private_t *sink_private = NULL;

	OSCL_TRACE("base_cmp_handle:%p\n", hComponent);
	oscl_param_check(hComponent != NULL, OMX_ErrorBadParameter, NULL);
	component = get_lb_component(hComponent);
	sink_private = component->component_private;

	pthread_cond_destroy(&sink_private->cond);
	pthread_mutex_destroy(&sink_private->lock);
	clk_buf_deinit(component);

	base_port_deinit(&component->port[SINK_INPUT_PORT_INDEX]);
	clock_port_deinit(&component->port[SINK_CLOCK_PORT_INDEX]);
	oscl_queue_deinit(&sink_private->release_queue);
	oscl_close_disp_engine(sink_private->disp_hdl);
	oscl_free(sink_private);
	base_component_deinit(hComponent);
	OSCL_LOGD("vrender deinit complete.");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE vrender_component_init(lb_omx_component_t *cmp_handle)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	vrender_private_t *sink_private = NULL;

	sink_private = oscl_zalloc(sizeof(vrender_private_t));
	oscl_param_check_exit((NULL != sink_private),
				OMX_ErrorInsufficientResources, NULL);
	oscl_queue_init(&sink_private->release_queue);
	ret = base_component_init(cmp_handle);
	oscl_param_check_exit((ret == OMX_ErrorNone), ret, NULL);
	cmp_handle->name = "OMX.LB.SINK.VRENDER";
	cmp_handle->component_private = sink_private;
	cmp_handle->buf_manager = vrender_buffer_manager;
	cmp_handle->buf_handle = vrender_buffer_handle;
	cmp_handle->base_comp.ComponentDeInit = vrender_component_deinit;
	cmp_handle->base_comp.SetParameter = vrender_set_parameter;
	cmp_handle->do_state_set = vrender_set_state;

	cmp_handle->num_ports = 2;
	ret = base_port_init(cmp_handle, &cmp_handle->port[SINK_INPUT_PORT_INDEX],
			SINK_INPUT_PORT_INDEX,
			OMX_DirInput);
	if (ret != OMX_ErrorNone)
		goto EXIT;
	ret = clock_port_init(cmp_handle, &cmp_handle->port[SINK_CLOCK_PORT_INDEX],
		SINK_CLOCK_PORT_INDEX, OMX_DirInput);
	if (ret != OMX_ErrorNone) {
		OSCL_LOGE("init clock port error!\n");
		base_port_deinit(&cmp_handle->port[SINK_INPUT_PORT_INDEX]);
		goto EXIT;
	}
	cmp_handle->port[SINK_INPUT_PORT_INDEX].port_param.nBufferSize =
		SINK_INPUT_DEFAULT_SIZE;
	cmp_handle->port[SINK_INPUT_PORT_INDEX].port_param.eDomain =
		OMX_PortDomainVideo;
	pthread_attr_setstacksize(&cmp_handle->buf_thread_attr, 0x1000);
	sink_private->avsync = OMX_FALSE;
	sink_private->scale_mode = SCALE_MODE_ONLINE;
	sink_private->scale_out_format = OMX_COLOR_Format16bitRGB565;
	sink_private->xscale  = 1 << 16;

	clk_buf_init(cmp_handle);
	pthread_cond_init(&sink_private->cond, NULL);
	pthread_mutex_init(&sink_private->lock, NULL);

	return OMX_ErrorNone;
EXIT:
	oscl_queue_deinit(&sink_private->release_queue);
	base_component_deinit(cmp_handle);
	if (sink_private != NULL)
		oscl_free(sink_private);
	OSCL_TRACE("%d vrender:%p\n", ret, cmp_handle);

	return ret;
}

#ifdef OMX_DYNAMIC_LOADING
void *omx_component_init = vrender_component_init;
#endif

