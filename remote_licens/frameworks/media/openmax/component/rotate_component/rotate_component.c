/*
 * rotate_component.c - code for rotate component.
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
#define LOG_TAG			"rot_component"

#include <oscl.h>
#include <base_component.h>
#include "rotate_component.h"
#include "display/oscl_display.h"
#include "omx_vendor_lb.h"

#define DEFAULT_VIDEO_WIDTH		(1280)
#define DEFAULT_VIDEO_HEIGHT		(720)
#define DEFAULT_INBUFFER_NUM		(4)
#define DEFAULT_OUTBUFFER_NUM		(3)
#define DEFAULT_BUFFER_ALIGN_SIZE	(1024)

#define SW_ROATE_THREAD_PRIO		(15)

typedef enum rotate_type {
	HW_ROTATE,
	SW_ROTATE,
} rotate_type_e;

typedef struct channel_rotate {
	OMX_S32			ch_index;
	void			*handle;
	rotate_type_e		rot_type;
	OMX_CONFIG_ROTATIONTYPE rot_mode;
	rotate_buffer_t		input;
	rotate_buffer_t		output;
	sem_t			*wait_sem;
	sem_t			*complete_sem;
	OMX_S32			stop;
	OMX_S32			wast;
} channel_rotate_t;

typedef struct rotate_private {
	void			*disp_hdl;	/* display device handle */
	OMX_CONFIG_ROTATIONTYPE rot_mode; /* rot_mode.nRotation: rotate_mode_e */
	rotate_buffer_t		input;
	rotate_buffer_t		output;
	OMX_U32			inbuf_size[3];
	OMX_U32			outbuf_size[3];

	/* all channel */
	rot_para_t		ch_para;
	OMX_S32			channel_num;
	channel_rotate_t	ch_roate[MAX_ROT_CHANNEL];
	OMX_S32			ch_thread_id[MAX_ROT_CHANNEL];
	pthread_t		ch_handler_thread[MAX_ROT_CHANNEL];
} rotate_private_t;

static OMX_ERRORTYPE rotate_multi_init(rotate_private_t *rot_private,
			rot_para_t *para);
static void rotate_multi_deinit(rotate_private_t *rot_private);

static OMX_ERRORTYPE get_rotate_rect(vdisp_rotate_mode_e mode,
				     rotate_buffer_t *src,
				     rotate_buffer_t *dst)
{
	OMX_U32 offset_tail_x, offset_tail_y;
	disp_rect_t src_rect;

	src_rect.left = 0;
	src_rect.top = 0;
	src_rect.width = src->crop_rect.width;
	src_rect.height = src->crop_rect.height;
	OSCL_LOGI("mode:%d src[%d %d %d %d] src_rect[%d %d %d %d] crop_rect[%d %d %d %d]",
		   mode,
		   src_rect.left,
		   src_rect.top,
		   src_rect.width,
		   src_rect.height,
		   src->src_rect.left,
		   src->src_rect.top,
		   src->src_rect.width,
		   src->src_rect.height,
		   src->crop_rect.left,
		   src->crop_rect.top,
		   src->crop_rect.width,
		   src->crop_rect.height);

	offset_tail_x = src->src_rect.width -
		(src->crop_rect.width + src->crop_rect.left);
	offset_tail_y = 0;
	if (src_rect.height > (src->crop_rect.height + src->crop_rect.top)) {
		offset_tail_y = src_rect.height -
			(src->crop_rect.height + src->crop_rect.top);
	} else if (src_rect.height > src->crop_rect.height) {
		offset_tail_y = 0;
		src->crop_rect.height = src_rect.height - src->crop_rect.top;
	}

	if (VDISP_ROTATE_90 == mode ||
	    VDISP_FLIP_H_ROT_90 == mode ||
	    VDISP_FLIP_V_ROT_90 == mode ||
	    VDISP_SW_ROTATE_90 == mode ||
	    VDISP_SW_FLIP_H_ROT_90 == mode ||
	    VDISP_SW_FLIP_V_ROT_90 == mode) {
		dst->src_rect.width = src_rect.height;
		dst->src_rect.height = src_rect.width;
		dst->src_rect.left = src_rect.left;
		dst->src_rect.top = src_rect.top;

		dst->crop_rect.width = src->crop_rect.height;
		dst->crop_rect.height = src->crop_rect.width;
		dst->crop_rect.left = offset_tail_y;
		dst->crop_rect.top = src->crop_rect.left;
	} else if (VDISP_ROTATE_270 == mode ||
		VDISP_SW_ROTATE_270 == mode) {
		dst->src_rect.width = src_rect.height;
		dst->src_rect.height = src_rect.width;
		dst->src_rect.left = src_rect.left;
		dst->src_rect.top = src_rect.top;

		dst->crop_rect.width = src_rect.height -
				offset_tail_y - src->crop_rect.top;
		dst->crop_rect.height = src->crop_rect.width;
		dst->crop_rect.left = src->crop_rect.top;
		dst->crop_rect.top = offset_tail_x;
	} else if (VDISP_ROTATE_180 == mode ||
		VDISP_SW_ROTATE_270 == mode) {
		dst->src_rect.width = src_rect.width;
		dst->src_rect.height = src_rect.height;
		dst->src_rect.left = src_rect.left;
		dst->src_rect.top = src_rect.top;

		dst->crop_rect.width = src->crop_rect.width;
		dst->crop_rect.height = src_rect.height -
				offset_tail_y - src->crop_rect.top;
		dst->crop_rect.left = offset_tail_x;
		dst->crop_rect.top = src->crop_rect.top;
	} else if (VDISP_ROTATE_NONE == mode ||
		VDISP_FLIP_H == mode ||
		VDISP_FLIP_V == mode ||
		VDISP_SW_ROTATE_NONE == mode ||
		VDISP_SW_FLIP_H == mode ||
		VDISP_SW_FLIP_V == mode) {
		dst->src_rect.width = src_rect.width;
		dst->src_rect.height = src_rect.height;
		dst->src_rect.left = src_rect.left;
		dst->src_rect.top = src_rect.top;

		dst->crop_rect.width = src->crop_rect.width;
		dst->crop_rect.height = src->crop_rect.height;
		dst->crop_rect.left = src->crop_rect.left;
		dst->crop_rect.top = src->crop_rect.top;
	} else {
		OSCL_LOGE("%d mode not support.", mode);
		return OMX_ErrorBadParameter;
	}

#if 0
	src->src_rect.height = src_rect.height;
	src->src_rect.width = src_rect.width;
#endif

	dst->crop_rect.left = 0;
	dst->crop_rect.top = 0;
	OSCL_LOGI("rotate(%d)>> dst[%d %d %d %d] crop[%d %d %d %d]",
		   mode,
		   dst->src_rect.left, dst->src_rect.top,
		   dst->src_rect.width, dst->src_rect.height,
		   dst->crop_rect.left, dst->crop_rect.top,
		   dst->crop_rect.width, dst->crop_rect.height);

	return OMX_ErrorNone;
}

#if 0
static void save_video_frame(char *prefix, rotate_buffer_t *buffer, OMX_S32 align)
{
	OMX_S32 fd = 0;
	OMX_S32 size[3] = {0};
	char path[64] = {0};
	static OMX_S32 frm_count;

	if (NULL == buffer) {
		OSCL_LOGE("Input para error.");
		return;
	}
	rt_snprintf(path, sizeof(path), "/%s_%d.yuv", prefix, frm_count);
	fd = open(path, O_WRONLY | O_CREAT);
	if (fd < 0) {
		OSCL_LOGE("fail to open file: %s", path);
		return;
	}

	calc_frame_size(buffer->eColorFormat,
		buffer->src_rect.width, buffer->src_rect.height,
		size, align);
	OSCL_LOGD("Save>> w:%d h:%d(%d %d)",
		buffer->src_rect.width, buffer->src_rect.height,
		size[0], size[1]);

	if (OMX_COLOR_FormatYUV420SemiPlanar == buffer->eColorFormat) {
		write(fd, (void *)buffer->addr[0], size[0]);
		write(fd, (void *)buffer->addr[1], size[1]);
	}
	close(fd);

	frm_count++;
}
#endif

OMX_ERRORTYPE rotate_set_parameter(OMX_IN OMX_HANDLETYPE hComp,
				   OMX_IN OMX_INDEXTYPE paramIndex,
				   OMX_IN OMX_PTR paramData)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	lb_omx_component_t *component = NULL;
	rotate_private_t *rot_private = NULL;
	OMX_PARAM_PORTDEFINITIONTYPE *port_def = NULL;
	OMX_VIDEO_PORTDEFINITIONTYPE *video = NULL;
	base_port_t *inport = NULL;
	base_port_t *outport = NULL;

	OSCL_TRACE(" %p, %p , %d\n", hComp, paramData, paramIndex);

	oscl_param_check((hComp != NULL) && (paramData != NULL),
				OMX_ErrorBadParameter, NULL);
	component = get_lb_component(hComp);

	oscl_param_check((component != NULL), OMX_ErrorBadParameter, NULL);
	rot_private = component->component_private;

	switch (paramIndex) {
	case OMX_IndexParamPortDefinition: {
		port_def = (OMX_PARAM_PORTDEFINITIONTYPE *)paramData;

		ret = base_set_parameter(hComp, paramIndex, paramData);
		oscl_param_check((OMX_ErrorNone == ret), ret, NULL);

		if (ROT_INPUT_PORT_INDEX == port_def->nPortIndex) {
			inport = &component->port[ROT_INPUT_PORT_INDEX];
			video = &inport->port_param.format.video;
			ret = calc_frame_size(video->eColorFormat,
					      video->nStride,
					      video->nSliceHeight,
					      rot_private->inbuf_size,
					      inport->port_param.nBufferAlignment);
			oscl_param_check((OMX_ErrorNone == ret), ret, NULL);

			rot_private->input.eColorFormat = video->eColorFormat;

			rot_private->input.src_rect.top = 0;
			rot_private->input.src_rect.left = 0;
			rot_private->input.src_rect.width = video->nStride;
			rot_private->input.src_rect.height = video->nSliceHeight;

			/**
			 * default crop rectangle size ie equal
			 * to the original display size
			 */
			rot_private->input.crop_rect.top = 0;
			rot_private->input.crop_rect.left = 0;
			rot_private->input.crop_rect.width = video->nFrameWidth &
						~(ROT_W_STRIDE_ALIGN - 1);
			rot_private->input.crop_rect.height = video->nFrameHeight &
						~(ROT_H_STRIDE_ALIGN - 1);

			inport->port_param.nBufferSize = rot_private->inbuf_size[0] +
							 rot_private->inbuf_size[1] +
							 rot_private->inbuf_size[2];
			inport->port_param.format.video.xFramerate = video->xFramerate;
			OSCL_LOGI("inbuf_size[%d %d %d] src[%d %d %d %d] format:%x",
				rot_private->inbuf_size[0],
				rot_private->inbuf_size[1],
				rot_private->inbuf_size[2],
				video->nFrameWidth,
				video->nFrameHeight,
				video->nStride,
				video->nSliceHeight,
				rot_private->input.eColorFormat);
		}
		break;
	}

	/* this command is set after OMX_IndexParamPortDefinition */
	case OMX_IndexConfigCommonInputCrop: {
		OMX_CONFIG_RECTTYPE *rect = (OMX_CONFIG_RECTTYPE *)paramData;

		rot_private->input.crop_rect.top = rect->nTop;
		rot_private->input.crop_rect.left = rect->nLeft;
		rot_private->input.crop_rect.width = rect->nWidth &
						~(ROT_W_STRIDE_ALIGN - 1);
		rot_private->input.crop_rect.height = rect->nHeight &
						~(ROT_H_STRIDE_ALIGN - 1);
		break;
	}

	/**
	 * this command is set after
	 * OMX_IndexParamPortDefinition or OMX_IndexConfigCommonInputCrop
	 */
	case OMX_IndexConfigCommonRotate: {
		oscl_param_check((NULL != paramData), OMX_ErrorBadParameter, NULL);
		OMX_CONFIG_ROTATIONTYPE *rot_mode = (OMX_CONFIG_ROTATIONTYPE *)paramData;

		rot_private->rot_mode = *rot_mode;
		outport = &component->port[ROT_OUTPUT_PORT_INDEX];
		video = &outport->port_param.format.video;
		if (rot_private->rot_mode.nRotation != VDISP_MULTI_CHANNEL) {
			ret = get_rotate_rect(rot_private->rot_mode.nRotation,
					      &rot_private->input,
					      &rot_private->output);
			video->nStride = rot_private->output.src_rect.width;
			video->nSliceHeight = rot_private->output.src_rect.height;
			video->nFrameWidth = video->nStride;
			video->nFrameHeight = video->nSliceHeight;
			if (rot_private->output.crop_rect.width < video->nStride)
				video->nFrameWidth = rot_private->output.crop_rect.width;
			if (rot_private->output.crop_rect.height < video->nSliceHeight)
				video->nFrameHeight =
					rot_private->output.crop_rect.height;
		} else {
			ret = rotate_multi_init(rot_private, &rot_private->ch_para);
			video->nStride = rot_private->output.src_rect.width;
			video->nSliceHeight = rot_private->output.src_rect.height;
			video->nFrameWidth = video->nStride;
			video->nFrameHeight = video->nSliceHeight;
		}
		ret = calc_frame_size(video->eColorFormat,
				      video->nStride,
				      video->nSliceHeight,
				      rot_private->outbuf_size,
				      outport->port_param.nBufferAlignment);
		rot_private->output.eColorFormat = video->eColorFormat;
		outport->port_param.nBufferSize = rot_private->outbuf_size[0] +
						rot_private->outbuf_size[1] +
						rot_private->outbuf_size[2];
		outport->port_param.nBufferSize += sizeof(omx_extra_data_t);

		OSCL_LOGI("outbuf_size[%d %d %d] src[%d %d %d %d] format:%x",
			rot_private->outbuf_size[0],
			rot_private->outbuf_size[1],
			rot_private->outbuf_size[2],
			video->nFrameWidth,
			video->nFrameHeight,
			video->nStride,
			video->nSliceHeight,
			video->eColorFormat);

		break;
	}
	case omx_index_rotate_para: {
		oscl_param_check((NULL != paramData), OMX_ErrorBadParameter, NULL);
		rot_para_t *para = (rot_para_t *)paramData;
		memcpy(&rot_private->ch_para, para, sizeof(rot_para_t));
		break;
	}
	default:
		OSCL_LOGI("default base set params index %d\n", paramIndex);
		ret = base_set_parameter(hComp, paramIndex, paramData);
		break;
	}

	OSCL_TRACE(" %d\n", ret);

	return ret;
}

OMX_ERRORTYPE rotate_get_parameter(OMX_IN OMX_HANDLETYPE hComp,
				   OMX_IN OMX_INDEXTYPE paramIndex,
				   OMX_IN OMX_PTR paramData)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	lb_omx_component_t *component = NULL;
	rotate_private_t *rot_private = NULL;

	component = get_lb_component(hComp);
	oscl_param_check((component != NULL), OMX_ErrorBadParameter, NULL);
	rot_private = component->component_private;

	switch (paramIndex) {
	case OMX_IndexConfigCommonOutputCrop: {
		OMX_CONFIG_RECTTYPE *rect = (OMX_CONFIG_RECTTYPE *)paramData;

		rect->nLeft = rot_private->output.crop_rect.left;
		rect->nTop = rot_private->output.crop_rect.top;
		rect->nWidth = rot_private->output.crop_rect.width;
		rect->nHeight = rot_private->output.crop_rect.height;
		break;
	}
	case omx_index_lombo_video_size: {
		omx_size_t *size = (omx_size_t *)paramData;
		base_port_t *outport = &component->port[ROT_OUTPUT_PORT_INDEX];
		OMX_VIDEO_PORTDEFINITIONTYPE *video = &outport->port_param.format.video;
		size->width = video->nFrameWidth;
		size->height = video->nFrameHeight;
		size->stride = video->nStride;
		size->slice_height = video->nSliceHeight;
		break;
	}
	default:
		ret = base_get_parameter(hComp, paramIndex, paramData);
		break;
	}

	return ret;
}

OMX_ERRORTYPE rotate_set_state(OMX_HANDLETYPE hComp,
			OMX_U32 dest_state)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	lb_omx_component_t *component = NULL;
	rotate_private_t *rot_private = NULL;
	OMX_STATETYPE pre_state;

	OSCL_TRACE(" %p, %x\n", hComp, dest_state);
	oscl_param_check(hComp != NULL, OMX_ErrorBadParameter, NULL);
	component = get_lb_component(hComp);
	rot_private = component->component_private;

	if (dest_state == OMX_StateExecuting && (component->state == OMX_StateIdle ||
		component->state == OMX_StatePause)) {
		if (rot_private->disp_hdl != NULL) {
			oscl_close_rot_submodule(rot_private->disp_hdl);
			oscl_close_disp_engine(rot_private->disp_hdl);
		}
		rot_private->disp_hdl = oscl_open_disp_engine();
		if (rot_private->disp_hdl == NULL) {
			OSCL_LOGE("open disp engine error!");
			return OMX_ErrorInsufficientResources;
		}
		ret = oscl_open_rot_submodule(rot_private->disp_hdl);
		if (ret != OMX_ErrorNone)
			OSCL_LOGE("open rotate submodule error!");
	}
	pre_state = component->state;
	ret = base_component_set_state(hComp, dest_state);
	if (dest_state == OMX_StateIdle && (pre_state == OMX_StateExecuting ||
		pre_state == OMX_StatePause)) {
		if (rot_private->disp_hdl != NULL) {
			rotate_multi_deinit(rot_private);
			oscl_close_rot_submodule(rot_private->disp_hdl);
			oscl_close_disp_engine(rot_private->disp_hdl);
			rot_private->disp_hdl = NULL;
		}
	}
	return ret;
}

OMX_ERRORTYPE roate_multi_channel(rotate_private_t *rot_private,
		       OMX_BUFFERHEADERTYPE *inbuf,
		       OMX_BUFFERHEADERTYPE *outbuf)
{
	OMX_U8 *iy_base_addr = NULL;
	OMX_U8 *icb_base_addr = NULL;
	OMX_U8 *icr_base_addr = NULL;
	OMX_U8 *oy_base_addr = NULL;
	OMX_U8 *ocb_base_addr = NULL;
	OMX_U8 *ocr_base_addr = NULL;
	OMX_S32 i = 0;
	channel_rotate_t *ch_rot = NULL;
	OMX_TICKS end, start;

	if (rot_private->inbuf_size[0])
		iy_base_addr = inbuf->pBuffer;
	if (rot_private->inbuf_size[1])
		icb_base_addr = inbuf->pBuffer + rot_private->inbuf_size[0];
	if (rot_private->inbuf_size[2])
		icr_base_addr = inbuf->pBuffer + rot_private->inbuf_size[0] +
				rot_private->inbuf_size[0];

	if (rot_private->outbuf_size[0])
		oy_base_addr = outbuf->pBuffer;
	if (rot_private->outbuf_size[1])
		ocb_base_addr = outbuf->pBuffer + rot_private->outbuf_size[0];
	if (rot_private->outbuf_size[2])
		ocr_base_addr = outbuf->pBuffer + rot_private->outbuf_size[0] +
				rot_private->outbuf_size[0];

	/* soft rotate */
	for (i = rot_private->channel_num - 1; i >= 0; i--) {
		ch_rot = &rot_private->ch_roate[i];
		if (ch_rot->rot_type == SW_ROTATE) {
			ch_rot->input.addr[0] = iy_base_addr;
			ch_rot->input.addr[1] = icb_base_addr;
			ch_rot->input.addr[2] = icr_base_addr;

			ch_rot->output.addr[0] = oy_base_addr;
			ch_rot->output.addr[1] = ocb_base_addr;
			ch_rot->output.addr[2] = ocr_base_addr;
			oscl_sem_post(ch_rot->wait_sem);
		}
	}

	/* hardware rotate */
	for (i = rot_private->channel_num - 1; i >= 0; i--) {
		ch_rot = &rot_private->ch_roate[i];
		if (ch_rot->rot_type == HW_ROTATE) {
			ch_rot->input.addr[0] = iy_base_addr;
			ch_rot->input.addr[1] = icb_base_addr;
			ch_rot->input.addr[2] = icr_base_addr;
			ch_rot->output.addr[0] = oy_base_addr;
			ch_rot->output.addr[1] = ocb_base_addr;
			ch_rot->output.addr[2] = ocr_base_addr;
			start = oscl_get_msec();
			oscl_disp_hw_rotate(rot_private->disp_hdl,
					  ch_rot->rot_mode.nRotation,
					  &ch_rot->input,
					  &ch_rot->output);
			end = oscl_get_msec();
			ch_rot->wast = end - start;
		}
	}
	for (i = rot_private->channel_num - 1; i >= 0; i--) {
		ch_rot = &rot_private->ch_roate[i];
		if (ch_rot->rot_type == SW_ROTATE)
			oscl_sem_wait_debug(ch_rot->complete_sem);
	}

	return OMX_ErrorNone;
}

static void *sw_rotate_thread(void *param)
{
	channel_rotate_t *ch_rot = (channel_rotate_t *)param;
	OMX_TICKS end, start;

	while (1) {
		oscl_sem_wait_debug(ch_rot->wait_sem);
		if (ch_rot->stop) {
			OSCL_LOGI("exit from %d sw rotate handle\n", ch_rot->ch_index);
			break;
		}
		start = oscl_get_msec();
		if (ch_rot->handle) {
			oscl_disp_sw_rotate(ch_rot->handle, ch_rot->rot_mode.nRotation,
					&ch_rot->input, &ch_rot->output);
		} else {
			OSCL_LOGE("sw rotate handle is null.");
		}
		end = oscl_get_msec();
		oscl_sem_post(ch_rot->complete_sem);
		ch_rot->wast = end - start;
	}

	return NULL;
}

static OMX_ERRORTYPE rotate_multi_para_check(rot_para_t *para)
{
	OMX_S32 i = 0;
	OMX_S32 rotate;

	rotate = para->rot_mode[0].nRotation;
	for (i = 1; i < para->channel_num; i++) {
		rotate = para->rot_mode[0].nRotation;
		if ((rotate == VDISP_SW_FLIP_V) || (rotate == VDISP_SW_ROTATE_NONE) ||
			(rotate == VDISP_ROTATE_NONE) || (rotate == VDISP_SW_FLIP_H) ||
			(rotate == VDISP_FLIP_H) || (rotate == VDISP_SW_ROTATE_180) ||
			(rotate == VDISP_ROTATE_180) || (rotate == VDISP_FLIP_V)) {
			if ((para->rot_mode[i].nRotation != VDISP_FLIP_H) &&
				(para->rot_mode[i].nRotation != VDISP_FLIP_V) &&
				(para->rot_mode[i].nRotation != VDISP_ROTATE_NONE) &&
				(para->rot_mode[i].nRotation != VDISP_SW_FLIP_H) &&
				(para->rot_mode[i].nRotation != VDISP_SW_FLIP_V) &&
				(para->rot_mode[i].nRotation != VDISP_SW_ROTATE_NONE) &&
				(para->rot_mode[i].nRotation != VDISP_ROTATE_180) &&
				(para->rot_mode[i].nRotation != VDISP_SW_ROTATE_180)) {
				OSCL_LOGE("multi channel no support other mode. ");
				return OMX_ErrorInsufficientResources;
			}
		} else if ((rotate == VDISP_ROTATE_90) ||
			(rotate == VDISP_SW_ROTATE_90) ||
			(rotate == VDISP_ROTATE_270) ||
			(rotate == VDISP_SW_ROTATE_270) ||
			(rotate == VDISP_FLIP_H_ROT_90) ||
			(rotate == VDISP_SW_FLIP_H_ROT_90) ||
			(rotate == VDISP_FLIP_V_ROT_90) ||
			(rotate == VDISP_SW_FLIP_V_ROT_90))
			if ((para->rot_mode[i].nRotation != VDISP_ROTATE_90) &&
				(para->rot_mode[i].nRotation != VDISP_SW_ROTATE_90) &&
				(para->rot_mode[i].nRotation != VDISP_ROTATE_270) &&
				(para->rot_mode[i].nRotation != VDISP_SW_ROTATE_270) &&
				(para->rot_mode[i].nRotation != VDISP_FLIP_H_ROT_90) &&
				(para->rot_mode[i].nRotation != VDISP_SW_FLIP_H_ROT_90) &&
				(para->rot_mode[i].nRotation != VDISP_FLIP_V_ROT_90) &&
				(para->rot_mode[i].nRotation != VDISP_SW_FLIP_V_ROT_90)) {
				OSCL_LOGE("multi channel no support other mode. ");
				return OMX_ErrorInsufficientResources;
		}
	}
	return OMX_ErrorNone;
}

static OMX_ERRORTYPE rotate_multi_outbuffer_size(rot_para_t *rot_para,
		OMX_S32 *width, OMX_S32 *height)
{
	OMX_S32 out_buf_w, out_buf_h;
	OMX_S32 min = 0, max = 0;
	OMX_S32 j = 0, tmp = 0;
	rot_rect_info_t *ch_info_tmp = NULL;
	rot_rect_info_t *ch_info = NULL;

	oscl_param_check(rot_para, OMX_ErrorBadParameter, NULL);
	oscl_param_check((width && height), OMX_ErrorBadParameter, NULL);

	/* calc buffer width */
	ch_info = &rot_para->dst_rot_rect[0];
	min = ch_info->x;
	max = ch_info->x;
	tmp = max + ch_info->width;
	out_buf_w = ch_info->width;
	for (j = 1; j < rot_para->channel_num; j++) {
		ch_info_tmp = &rot_para->dst_rot_rect[j];
		if (ch_info_tmp->y == ch_info->y) {
			out_buf_w += ch_info_tmp->width;
			if (ch_info_tmp->x > max) {
				max = ch_info_tmp->x;
				tmp = max + ch_info_tmp->width;
			}
			if (ch_info_tmp->x < min)
				min = ch_info_tmp->x;
		}
	}
	if ((tmp - min) != out_buf_w) {
		OSCL_LOGE("rotate parar config error.(w:%d, tmp_w:%d min:%d max:%d)",
			out_buf_w, tmp, min, max);
		return OMX_ErrorBadParameter;
	}

	/* calc buffer height */
	ch_info = &rot_para->dst_rot_rect[0];
	min = ch_info->y;
	max = ch_info->y;
	tmp = max + ch_info->height;
	out_buf_h = ch_info->height;
	for (j = 1; j < rot_para->channel_num; j++) {
		ch_info_tmp = &rot_para->dst_rot_rect[j];
		if (ch_info_tmp->x == ch_info->x) {
			out_buf_h += ch_info_tmp->height;
			if (ch_info_tmp->y > max) {
				max = ch_info_tmp->y;
				tmp = max + ch_info_tmp->height;
			}
			if (ch_info_tmp->y < min)
				min = ch_info_tmp->y;
		}
	}
	if ((tmp - min) != out_buf_h) {
		OSCL_LOGE("rotate parar config error.(h:%d, tmp_h:%d min:%d max:%d)",
			out_buf_h, tmp, min, max);
		return OMX_ErrorBadParameter;
	}
	*width = out_buf_w;
	*height = out_buf_h;
	OSCL_LOGI("outbuf size>> w:%d h:%d", out_buf_w, out_buf_h);

	return OMX_ErrorNone;
}

static OMX_ERRORTYPE rotate_multi_init(rotate_private_t *rot_private, rot_para_t *para)
{
	OMX_S32 i = 0;
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	channel_rotate_t *ch_rot = NULL;
	OMX_S32 src_height, dst_height;
	OMX_S32 src_width, dst_width;
	OMX_S32 img_width, img_height;
	OMX_S32 dst_buf_w, dst_buf_h;

	oscl_param_check((para->channel_num > 0) &&
		(para->channel_num <= MAX_ROT_CHANNEL),	OMX_ErrorBadParameter, NULL);
	ret = rotate_multi_para_check(para);
	oscl_param_check(ret == OMX_ErrorNone, OMX_ErrorBadParameter, NULL);
	ret = rotate_multi_outbuffer_size(para, &dst_buf_w, &dst_buf_h);
	oscl_param_check(ret == OMX_ErrorNone, OMX_ErrorBadParameter, NULL);
	for (i = 0; i < MAX_ROT_CHANNEL; i++) {
		rot_private->ch_roate[i].stop = 0;
		rot_private->ch_thread_id[i] = -1;
	}
	rot_private->output.src_rect.width = dst_buf_w;
	rot_private->output.src_rect.height = dst_buf_h;
	img_width = rot_private->input.src_rect.width;
	img_height = rot_private->input.src_rect.height;
	for (i = 0; i < para->channel_num; i++) {
		ch_rot = &rot_private->ch_roate[i];
		ch_rot->handle = rot_private->disp_hdl;
		ch_rot->ch_index = i;
		if ((para->rot_mode[i].nRotation >= VDISP_SW_ROTATE_NONE) &&
			(para->rot_mode[i].nRotation <= VDISP_SW_FLIP_V_ROT_90))
			ch_rot->rot_type = SW_ROTATE;
		else
			ch_rot->rot_type = HW_ROTATE;
		ch_rot->rot_mode.nRotation = para->rot_mode[i].nRotation;
		ch_rot->wait_sem = (sem_t *)oscl_zalloc(sizeof(sem_t));
		oscl_param_check(ch_rot->wait_sem != NULL,
				OMX_ErrorInsufficientResources, NULL);
		sem_init(ch_rot->wait_sem, 0, 0);
		ch_rot->complete_sem = (sem_t *)oscl_zalloc(sizeof(sem_t));
		oscl_param_check(ch_rot->complete_sem != NULL,
				OMX_ErrorInsufficientResources, NULL);
		sem_init(ch_rot->complete_sem, 0, 0);

		if (ch_rot->rot_type == SW_ROTATE) {
#ifdef __EOS__
			pthread_attr_t msg_thread_attr;
			struct sched_param shed_param = {0};

			pthread_attr_init(&msg_thread_attr);
			pthread_attr_setstacksize(&msg_thread_attr, 0x1000);
			shed_param.sched_priority = SW_ROATE_THREAD_PRIO;
			pthread_attr_setschedparam(&msg_thread_attr, &shed_param);
			rot_private->ch_thread_id[i] = pthread_create(
					&rot_private->ch_handler_thread[i],
					&msg_thread_attr,
					sw_rotate_thread,
					(void *)ch_rot);
#else
			rot_private->ch_thread_id[i] = pthread_create(
					&rot_private->ch_handler_thread[i],
					NULL,
					sw_rotate_thread,
					(void *)ch_rot);
#endif
			if (rot_private->ch_thread_id[i] != 0) {
				OSCL_LOGE("Create thread fail.");
				return OMX_ErrorInsufficientResources;
			}
		}
		if (para->src_rot_rect[i].width != 0 &&
			para->src_rot_rect[i].height != 0 &&
			para->dst_rot_rect[i].width != 0 &&
			para->dst_rot_rect[i].height != 0) {
			src_width = para->src_rot_rect[i].width &
						~(ROT_W_STRIDE_ALIGN - 1);
			src_height = para->src_rot_rect[i].height &
						~(ROT_H_STRIDE_ALIGN - 1);
			dst_width = para->dst_rot_rect[i].width &
						~(ROT_W_STRIDE_ALIGN - 1);
			dst_height = para->dst_rot_rect[i].height &
						~(ROT_H_STRIDE_ALIGN - 1);
			ch_rot->input.eColorFormat = rot_private->input.eColorFormat;
			ch_rot->input.addr[0] = NULL;
			ch_rot->input.addr[1] = NULL;
			ch_rot->input.addr[2] = NULL;
			ch_rot->input.src_rect.left = 0;
			ch_rot->input.src_rect.top = 0;
			ch_rot->input.src_rect.width = img_width;
			ch_rot->input.src_rect.height = img_height;
			ch_rot->input.crop_rect.left = para->src_rot_rect[i].x;
			ch_rot->input.crop_rect.top = para->src_rot_rect[i].y;
			ch_rot->input.crop_rect.width = src_width;
			ch_rot->input.crop_rect.height = src_height;

			ch_rot->output.eColorFormat = rot_private->input.eColorFormat;
			ch_rot->output.addr[0] = NULL;
			ch_rot->output.addr[1] = NULL;
			ch_rot->output.addr[2] = NULL;
			ch_rot->output.src_rect.left = 0;
			ch_rot->output.src_rect.top = 0;
			ch_rot->output.src_rect.width = dst_buf_w;
			ch_rot->output.src_rect.height = dst_buf_h;
			ch_rot->output.crop_rect.left = para->dst_rot_rect[i].x;
			ch_rot->output.crop_rect.top = para->dst_rot_rect[i].y;
			ch_rot->output.crop_rect.width = dst_width;
			ch_rot->output.crop_rect.height = dst_height;

			OSCL_LOGI("CH%d crop>> in(%d %d %d %d) -> out(%d %d %d %d)",
			i, ch_rot->input.crop_rect.left, ch_rot->input.crop_rect.top,
			ch_rot->input.crop_rect.width, ch_rot->input.crop_rect.height,
			ch_rot->output.crop_rect.left, ch_rot->output.crop_rect.top,
			ch_rot->output.crop_rect.width, ch_rot->output.crop_rect.height);
		} else {
			OSCL_LOGE("rect para error. %dx%d -> %dx%d",
						para->src_rot_rect[i].width,
						para->src_rot_rect[i].height,
						para->dst_rot_rect[i].width,
						para->dst_rot_rect[i].height);

			return OMX_ErrorInsufficientResources;
		}
	}
	rot_private->channel_num = para->channel_num;

	return OMX_ErrorNone;
}

static void rotate_multi_deinit(rotate_private_t *rot_private)
{
	OMX_S32 i = 0;

	for (i = 0; i < rot_private->channel_num; i++)
		rot_private->ch_roate[i].stop = 1;
	for (i = 0; i < rot_private->channel_num; i++) {
		if (rot_private->ch_roate[i].wait_sem)
			sem_post(rot_private->ch_roate[i].wait_sem);
		if (rot_private->ch_roate[i].complete_sem)
			sem_post(rot_private->ch_roate[i].complete_sem);
		if (rot_private->ch_thread_id[i] >= 0)
			pthread_join(rot_private->ch_handler_thread[i] , NULL);
		rot_private->ch_thread_id[i] = -1;
		if (rot_private->ch_roate[i].wait_sem) {
			sem_destroy(rot_private->ch_roate[i].wait_sem);
			oscl_free(rot_private->ch_roate[i].wait_sem);
			rot_private->ch_roate[i].wait_sem = NULL;
		}
		if (rot_private->ch_roate[i].complete_sem) {
			sem_destroy(rot_private->ch_roate[i].complete_sem);
			oscl_free(rot_private->ch_roate[i].complete_sem);
			rot_private->ch_roate[i].complete_sem = NULL;
		}
	}
}

void rotate_buf_handle(OMX_HANDLETYPE hComp,
		       OMX_BUFFERHEADERTYPE *inbuf,
		       OMX_BUFFERHEADERTYPE *outbuf)
{
	lb_omx_component_t *component = NULL;
	rotate_private_t *rot_private = NULL;
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	OMX_S32 video_size = 0;
	OMX_S32 rotate = 0;

	component = get_lb_component(hComp);
	rot_private = component->component_private;

	/* rot buffer transfer */
	rot_private->input.addr[0] = inbuf->pBuffer;
	rot_private->input.addr[1] = inbuf->pBuffer +
				rot_private->inbuf_size[0];
	rot_private->input.addr[2] = inbuf->pBuffer +
				rot_private->inbuf_size[0] +
				rot_private->inbuf_size[1];
	rot_private->input.size[0] = rot_private->inbuf_size[0];
	rot_private->input.size[1] = rot_private->inbuf_size[1];
	rot_private->input.size[2] = rot_private->inbuf_size[2];

	rot_private->output.addr[0] = outbuf->pBuffer;
	rot_private->output.addr[1] = outbuf->pBuffer +
				rot_private->outbuf_size[0];
	rot_private->output.addr[2] = outbuf->pBuffer +
				rot_private->outbuf_size[0] +
				rot_private->outbuf_size[1];
	rot_private->output.size[0] = rot_private->outbuf_size[0];
	rot_private->output.size[1] = rot_private->outbuf_size[1];
	rot_private->output.size[2] = rot_private->outbuf_size[2];

	OSCL_LOGD("Rot(%d)>> inbuf:%p %p, outbuf:%p %p",
				rot_private->rot_mode.nRotation,
				inbuf,
				inbuf->pBuffer,
				outbuf,
				outbuf->pBuffer);
	OMX_TICKS end, start = oscl_get_msec();
	rotate = rot_private->rot_mode.nRotation;
	if (rotate != VDISP_MULTI_CHANNEL) {
		if ((rotate >= VDISP_SW_ROTATE_NONE) &&
			(rotate <= VDISP_SW_FLIP_V_ROT_90)) {
			ret = oscl_disp_sw_rotate(rot_private->disp_hdl,
					  rot_private->rot_mode.nRotation,
					  &rot_private->input,
					  &rot_private->output);
		} else {
			ret = oscl_disp_hw_rotate(rot_private->disp_hdl,
				  rot_private->rot_mode.nRotation,
				  &rot_private->input,
				  &rot_private->output);
		}
	} else {
		ret = roate_multi_channel(rot_private, inbuf, outbuf);
	}
	end = oscl_get_msec();
	if ((end - start) > 40) {
		OSCL_LOGW("rotate one frame wast: %lld, %d %d %d %d",
			(end - start),
			rot_private->ch_roate[0].wast,
			rot_private->ch_roate[1].wast,
			rot_private->ch_roate[2].wast,
			rot_private->ch_roate[3].wast);
	}
	if (OMX_ErrorNone == ret) {
		outbuf->nFilledLen = rot_private->outbuf_size[0] +
				     rot_private->outbuf_size[1] +
				     rot_private->outbuf_size[2];
	} else {
		outbuf->nFilledLen = 0;
	}
	video_size = rot_private->inbuf_size[0] +
			 rot_private->inbuf_size[1] +
			 rot_private->inbuf_size[2];
	if ((video_size + sizeof(omx_extra_data_t)) <= inbuf->nAllocLen) {
		memcpy(outbuf->pBuffer + outbuf->nAllocLen - sizeof(omx_extra_data_t),
			inbuf->pBuffer + inbuf->nAllocLen - sizeof(omx_extra_data_t),
			sizeof(omx_extra_data_t));
	} else {
		OSCL_LOGD("No extra data, video_size:%d, inbuf->nAllocLen:%d, struct:%d",
			video_size, inbuf->nAllocLen, (int)sizeof(omx_extra_data_t));
	}

	outbuf->nTimeStamp = inbuf->nTimeStamp;
	inbuf->nFilledLen = 0;
	/* save_video_frame("after", &rot_private->output, 1); */
}

OMX_ERRORTYPE rotate_component_deinit(OMX_IN OMX_HANDLETYPE hComp)
{
	lb_omx_component_t *component;
	rotate_private_t *rot_private = NULL;

	oscl_param_check(hComp != NULL, OMX_ErrorBadParameter, NULL);

	OSCL_TRACE("base_cmp_handle:%p\n", hComp);
	oscl_param_check(hComp != NULL, OMX_ErrorBadParameter, NULL);
	component = get_lb_component(hComp);
	rot_private = component->component_private;

	base_port_deinit(&component->port[ROT_INPUT_PORT_INDEX]);
	base_port_deinit(&component->port[ROT_OUTPUT_PORT_INDEX]);

	oscl_free(rot_private);

	base_component_deinit(hComp);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE rotate_component_init(lb_omx_component_t *cmp_handle)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	OMX_COMPONENTTYPE *base_cmp = &(cmp_handle->base_comp);
	rotate_private_t *rot_private = NULL;
	OMX_PARAM_PORTDEFINITIONTYPE *port_param = NULL;
	OMX_U32 plane_size[3] = {0};

	rot_private = oscl_zalloc(sizeof(rotate_private_t));
	oscl_param_check_exit((rot_private != NULL),
		OMX_ErrorInsufficientResources, NULL);
	rot_private->rot_mode.nRotation = -1;

	ret = base_component_init(cmp_handle);
	if (ret != OMX_ErrorNone) {
		oscl_free(rot_private);
		return ret;
	}
	cmp_handle->name = "OMX.LB.VIDEO.ROT";
	cmp_handle->component_private = rot_private;
	cmp_handle->base_comp.ComponentDeInit = rotate_component_deinit;
	cmp_handle->buf_handle = rotate_buf_handle;
	cmp_handle->do_state_set = rotate_set_state;
	cmp_handle->num_ports = 2;

	/* input port */
	ret = base_port_init(cmp_handle,
			     &cmp_handle->port[ROT_INPUT_PORT_INDEX],
			     ROT_INPUT_PORT_INDEX,
			     OMX_DirInput);
	if (OMX_ErrorNone != ret)
		goto EXIT;

	port_param = &cmp_handle->port[ROT_INPUT_PORT_INDEX].port_param;
	port_param->eDomain = OMX_PortDomainVideo;
	port_param->nBufferAlignment = DEFAULT_BUFFER_ALIGN_SIZE;
	port_param->nBufferCountActual = DEFAULT_INBUFFER_NUM;
	port_param->bBuffersContiguous = 1;

	port_param->format.video.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
	port_param->format.video.nFrameWidth = DEFAULT_VIDEO_WIDTH;
	port_param->format.video.nFrameHeight = DEFAULT_VIDEO_HEIGHT;
	ret = calc_frame_size(port_param->format.video.eColorFormat,
			      port_param->format.video.nFrameWidth,
			      port_param->format.video.nFrameHeight,
			      plane_size,
			      port_param->nBufferAlignment);
	port_param->nBufferSize = plane_size[0] + plane_size[1] + plane_size[2];

	/* output port */
	ret = base_port_init(cmp_handle, &cmp_handle->port[ROT_OUTPUT_PORT_INDEX],
			ROT_OUTPUT_PORT_INDEX,
			OMX_DirOutput);
	if (ret != OMX_ErrorNone)
		goto EXIT1;
	port_param = &cmp_handle->port[ROT_OUTPUT_PORT_INDEX].port_param;
	port_param->eDomain = OMX_PortDomainVideo;
	port_param->nBufferAlignment = DEFAULT_BUFFER_ALIGN_SIZE;
	port_param->nBufferCountActual = DEFAULT_OUTBUFFER_NUM;
	port_param->bBuffersContiguous = 1;

	port_param->format.video.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
	port_param->format.video.nFrameWidth = DEFAULT_VIDEO_HEIGHT;
	port_param->format.video.nFrameHeight = DEFAULT_VIDEO_WIDTH;

	ret = calc_frame_size(port_param->format.video.eColorFormat,
			      port_param->format.video.nFrameWidth,
			      port_param->format.video.nFrameHeight,
			      plane_size,
			      port_param->nBufferAlignment);
	port_param->nBufferSize = plane_size[0] + plane_size[1] + plane_size[2];

	base_cmp->SetParameter = rotate_set_parameter;
	base_cmp->GetParameter = rotate_get_parameter;


	OSCL_TRACE("rotate_component_init:%d\n", ret);

	return ret;

EXIT1:
	base_port_deinit(&cmp_handle->port[ROT_INPUT_PORT_INDEX]);
EXIT:
	base_component_deinit(cmp_handle);
	oscl_free(rot_private);

	return OMX_ErrorInsufficientResources;
}

#ifdef OMX_DYNAMIC_LOADING
void *omx_component_init = rotate_component_init;
#endif


