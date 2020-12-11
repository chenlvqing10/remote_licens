/*
 * omx_video_stream.c - Standard functionality for video stream.
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

#define LOG_TAG		"video_stream"

#include <string.h>
#include <log/log.h>
#include <oscl.h>
#include "omx_api.h"
#include "vrender_component.h"
#include "video_stream.h"
#include "vsrc_camera.h"
#include "vsrc_manager.h"

typedef struct vsink_info {
	comp_info_t al_comp;
	port_info_t *vin;
	vdisp_para_t disp_para;
	int rotate;
} vsink_info_t;

typedef struct video_stream {
	pthread_mutex_t lock;
	unsigned int ref_cnt;
	port_info_t *src_vsink;
	port_info_t *src_vcb;
	port_info_t *src_cb_once;
	int rotate;

	/* camera */
	void *camera;

	vsink_info_t vsink_info;
	void *pic_private;
} video_stream_t;

/**
 * _video_stream_get_config_vsink - get default config for video sink.
 *
 * @vsink_info: video sink info
 *
 */
static void _get_default_config(vsink_info_t *vsink_info)
{
	disp_size_t size;
	void *disp_dev;

	/* to do: display operation should be done in components */
	disp_dev = oscl_open_disp_engine();
	if (disp_dev) {
		oscl_disp_get_screen_size(disp_dev, &size);
		vsink_info->disp_para.mode = VDISP_WINDOW_FULL_SCREEN_VIDEO_RATIO;
		vsink_info->disp_para.win_rect.width = size.width;
		vsink_info->disp_para.win_rect.height = size.height;
		vsink_info->disp_para.win_rect.left = 0;
		vsink_info->disp_para.win_rect.top = 0;
	}
	oscl_close_disp_engine(disp_dev);
	vsink_info->rotate = VDISP_ROTATE_NONE;

}

/**
 * video_stream_inc_refcnt - increase ref count of video stream handle.
 *
 * @hdl: video stream handle
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int video_stream_inc_refcnt(void *handle)
{
	video_stream_t *stream = handle;
	int ret = 0;

	OSCL_TRACE("start");
	oscl_param_check(stream != NULL, -1, NULL);
	pthread_mutex_lock(&stream->lock);
	stream->ref_cnt++;
	pthread_mutex_unlock(&stream->lock);

	OSCL_TRACE("end");
	return ret;
}

/**
 * video_stream_get_source - get source handle of video stream handle.
 *
 * @hdl: video stream handle
 *
 * Returns video source handle on success, NULL otherwise..
 */
void *video_stream_get_camera(void *handle)
{
	video_stream_t *stream = handle;

	OSCL_TRACE("start");
	oscl_param_check(stream != NULL, NULL, NULL);

	return stream->camera;
}

/**
 * video_stream_create - get a video stream handle.
 * This function will load all components video stream needs.
 *
 * Returns video stream handle on success, NULL otherwise..
 */
void *video_stream_create(void)
{
	video_stream_t *stream;
	int index, ret = -1;

	OSCL_TRACE("start");
	stream = oscl_zalloc(sizeof(video_stream_t));
	oscl_param_check_exit(stream != NULL, -1, NULL);
	pthread_mutex_init(&stream->lock, NULL);
	stream->ref_cnt = 1;

	/* init video sink component */
	ret = component_init(&stream->vsink_info.al_comp, "OMX.LB.SINK.VRENDER",
			&untunnel_common_callbacks);
	oscl_param_check_exit(ret == 0, ret, NULL);
	index = component_get_port_index(&stream->vsink_info.al_comp, OMX_DirInput,
			OMX_PortDomainVideo);
	oscl_param_check_exit(index >= 0, -1, NULL);
	stream->vsink_info.vin = &stream->vsink_info.al_comp.port_info[index];

	_get_default_config(&stream->vsink_info);
	stream->rotate = VDISP_ROTATE_90;

#if defined(__EOS__)
{
	OMX_PRIORITYMGMTTYPE priority;
	priority.nGroupPriority = LB_RECORDER_SINK_PRIO;
	OMX_SetParameter(stream->vsink_info.al_comp.cmp_hdl,
		OMX_IndexParamPriorityMgmt, &priority);
}
#endif

EXIT:
	if (ret != 0) {
		video_stream_release(stream);
		stream = NULL;
	}

	OSCL_TRACE("video_stream hdl:%p", stream);
	return stream;
}

/**
 * video_stream_release - release resource video stream used.
 *
 * @hdl: video stream handle
 *
 * Returns 0 on success, -EERROR otherwise..
 */
void video_stream_release(void *handle)
{
	video_stream_t *stream = (video_stream_t *)handle;
	int ret = 0;

	OSCL_TRACE("stream:%p", stream);
	if (stream == NULL)
		return;
	pthread_mutex_lock(&stream->lock);
	stream->ref_cnt--;
	if (stream->ref_cnt != 0) {
		OSCL_TRACE("stream->ref_cnt:%d", stream->ref_cnt);
		pthread_mutex_unlock(&stream->lock);
		return;
	}
	pthread_mutex_unlock(&stream->lock);

	ret = video_stream_stop_camera(stream);
	if (ret != 0)
		OSCL_LOGE("video_stream_stop_camera failed in video_stream_release");
	vsrc_manager_put_device(stream->camera);

	pthread_mutex_lock(&stream->lock);
	component_deinit(&stream->vsink_info.al_comp);
	pthread_mutex_unlock(&stream->lock);
	pthread_mutex_destroy(&stream->lock);

	memset(stream, 0, sizeof(video_stream_t));
	oscl_free(stream);
	OSCL_TRACE("exit");
}

/**
 * video_stream_set_camera_source - set source device of video_stream.
 *
 * @handle: video stream handle
 * @src_name: device name
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int video_stream_set_camera_source(void *handle, char *src_name)
{
	video_stream_t *stream = (video_stream_t *)handle;
	void *new_camera;
	int ret = 0;

	OSCL_TRACE("");
	oscl_param_check(((stream != NULL) && (src_name != NULL)), -1, NULL);

	pthread_mutex_lock(&stream->lock);
	/* get device before put old device
	 * to avoid close dev when src_name is the same as current openned
	 */
	new_camera = vsrc_manager_get_device(src_name);
	if (stream->camera != NULL) {
		ret = vsrc_manager_put_device(stream->camera);
		if (ret == 0)
			stream->camera = NULL;
	}
	if (stream->camera != NULL) {
		vsrc_manager_put_device(new_camera);
		new_camera = NULL;
	} else
		stream->camera = new_camera;
	if (stream->camera == NULL) {
		OSCL_LOGE("open dev:%s failed!", src_name);
		ret = -1;
	}
	pthread_mutex_unlock(&stream->lock);

	OSCL_TRACE("");
	return ret;
}

int video_stream_set_isp_inf_source(void *handle, char *src_name)
{
	video_stream_t *stream = (video_stream_t *)handle;
	int ret = 0;

	OSCL_TRACE("");
	oscl_param_check(((stream != NULL) && (src_name != NULL)), -1, NULL);

	pthread_mutex_lock(&stream->lock);
	ret = vsrc_camera_set_isp_inf_device(stream->camera, src_name);
	pthread_mutex_unlock(&stream->lock);

	OSCL_TRACE("");
	return ret;
}

/**
 * video_stream_set_layer_level - set layer level of display.
 *
 * @level  0: top, 1: bottom
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int video_stream_set_layer_level(void *handle, video_layer_level_t level)
{
	int ret = 0;
	video_stream_t *stream = handle;
	comp_info_t *al_comp = NULL;

	oscl_param_check((NULL != stream), -1, NULL);
	al_comp = &stream->vsink_info.al_comp;
	OSCL_LOGI("Set video layer level:%x", level);

	if (VIDEO_LAYER_TOP == level)
		ret = OMX_SetParameter(al_comp->cmp_hdl, omx_index_vrender_win_top,
					NULL);
	else
		ret = OMX_SetParameter(al_comp->cmp_hdl, omx_index_vrender_win_bottom,
					NULL);

	return ret;
}


/**
 * video_stream_set_srcpara - set video source para.
 *
 * @handle: video stream handle
 * @para: video source para
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int video_stream_set_src_para(void *handle, vsrc_param_t *para)
{
	video_stream_t *stream = (video_stream_t *)handle;
	int ret;

	OSCL_TRACE("handle:%p", handle);
	oscl_param_check((stream != NULL), OMX_ErrorBadParameter, NULL);
	oscl_param_check((para != NULL), OMX_ErrorBadParameter, NULL);

	pthread_mutex_lock(&stream->lock);
	ret = vsrc_camera_set_para(stream->camera, para);
	pthread_mutex_unlock(&stream->lock);

	OSCL_TRACE("");
	return ret;
}

/**
 * video_stream_get_srcpara - get video source para.
 *
 * @handle: video stream handle
 * @para: video source para
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int video_stream_get_src_para(void *handle, vsrc_param_t *para)
{
	video_stream_t *stream = (video_stream_t *)handle;
	int ret;

	OSCL_TRACE("");
	oscl_param_check((stream != NULL), OMX_ErrorBadParameter, NULL);
	oscl_param_check((para != NULL), OMX_ErrorBadParameter, NULL);

	pthread_mutex_lock(&stream->lock);
	ret = vsrc_camera_get_para(stream->camera, para);
	pthread_mutex_unlock(&stream->lock);

	OSCL_TRACE("");
	return ret;
}


int video_stream_set_camera_size(void *handle, int width, int height)
{
	video_stream_t *stream = (video_stream_t *)handle;
	vsrc_param_t src_para;

	oscl_param_check(stream != NULL, -1, NULL);
	memset(&src_para, 0, sizeof(src_para));
	src_para.height = height;
	src_para.width = width;

	return video_stream_set_src_para(handle, &src_para);
}

int video_stream_set_rotate(void *handle, vdisp_rotate_mode_e rotation)
{
	video_stream_t *stream = (video_stream_t *)handle;

	oscl_param_check(stream != NULL, -1, NULL);
	stream->rotate = rotation;

	return 0;
}

/**
 * video_stream_start_camera - start video stream.
 *
 * @handle: video stream handle
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int video_stream_start_camera(void *handle)
{
	int ret = 0;
	video_stream_t *stream = (video_stream_t *)handle;

	OSCL_TRACE("video_stream_start_camera");
	oscl_param_check(stream != NULL, -1, NULL);

	pthread_mutex_lock(&stream->lock);
	ret = vsrc_camera_prepare(stream->camera);
	pthread_mutex_unlock(&stream->lock);

	OSCL_TRACE("video_stream_start_camera end! ret:%x", ret);
	return ret;
}


/**
 * video_stream_stop_camera - stop video stream.
 *
 * @handle: video stream handle
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int video_stream_stop_camera(void *handle)
{
	video_stream_t *stream = (video_stream_t *)handle;
	int ret = 0;

	OSCL_TRACE("");
	oscl_param_check((stream != NULL), OMX_ErrorBadParameter, NULL);

	if (stream->src_vsink != NULL) {
		OSCL_LOGE("please stop preview befor stop camera!");
		ret = -1;
		goto EXIT;
	}
	if (stream->src_cb_once != NULL)
		video_stream_take_picture(stream, NULL);

	if (stream->src_vcb != NULL) {
		OSCL_LOGE("stop camera will set_frame_cb to NULL!");
		video_stream_set_frame_cb(stream, NULL);
	}

	pthread_mutex_lock(&stream->lock);
	vsrc_camera_reset(stream->camera);
	pthread_mutex_unlock(&stream->lock);

EXIT:
	OSCL_TRACE("ret:%d", ret);
	return ret;
}



/**
 * video_stream_set_disp_para - set display para.
 *
 * @handle: video stream handle
 * @disp_para: display para
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int video_stream_set_disp_para(void *handle, win_para_t *disp_para, int rotate)
{
	video_stream_t *stream = (video_stream_t *)handle;
	comp_info_t *sink_comp = NULL;
	int ret = 0;

	OSCL_TRACE("%p %p", handle, disp_para);
	oscl_param_check((stream != NULL), OMX_ErrorBadParameter, NULL);
	oscl_param_check((disp_para != NULL), OMX_ErrorBadParameter, NULL);

	pthread_mutex_lock(&stream->lock);
	sink_comp = &stream->vsink_info.al_comp;

	if (disp_para->mode > VDISP_WINDOW_USERDEF) {
		pthread_mutex_unlock(&stream->lock);
		OSCL_LOGE("Display mode set error.");
		return OMX_ErrorBadParameter;
	}
	stream->vsink_info.disp_para.mode = disp_para->mode;
	stream->vsink_info.disp_para.win_rect.top = disp_para->rect.y;
	stream->vsink_info.disp_para.win_rect.left = disp_para->rect.x;
	stream->vsink_info.disp_para.win_rect.width = disp_para->rect.width;
	stream->vsink_info.disp_para.win_rect.height = disp_para->rect.height;

	if ((disp_para->crop.width > 0) && (disp_para->crop.height > 0)) {
		stream->vsink_info.disp_para.crop_rect.top = disp_para->crop.y;
		stream->vsink_info.disp_para.crop_rect.left = disp_para->crop.x;
		stream->vsink_info.disp_para.crop_rect.width = disp_para->crop.width;
		stream->vsink_info.disp_para.crop_rect.height = disp_para->crop.height;
	}
	ret = OMX_SetParameter(sink_comp->cmp_hdl, omx_index_vrender_mode,
				&stream->vsink_info.disp_para);
	stream->vsink_info.rotate = rotate;
	pthread_mutex_unlock(&stream->lock);

	OSCL_TRACE("ret:%d", ret);
	return ret;
}

/**
 * video_stream_set_scale_mode - set scale mode
 *
 * @handle: video stream handle
 * @scale_mode_e: scale mode
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int video_stream_set_scale_mode(void *handle, scale_mode_e mode)
{
	video_stream_t *stream = (video_stream_t *)handle;
	comp_info_t *sink_comp = NULL;
	int ret = 0;
	scale_mode_e scale_mode = mode;
	oscl_param_check((stream != NULL), OMX_ErrorBadParameter, NULL);

	pthread_mutex_lock(&stream->lock);
	sink_comp = &stream->vsink_info.al_comp;
	ret = OMX_SetParameter(sink_comp->cmp_hdl, omx_index_vrender_scale_mode,
				(OMX_PTR)&scale_mode);
	pthread_mutex_unlock(&stream->lock);
	return ret;
}

/**
 * video_stream_disable_display - use to turn off display before deepsleep
 *
 * @handle: video stream handle
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int video_stream_disable_display(void *handle)
{
	video_stream_t *stream = (video_stream_t *)handle;
	comp_info_t *sink_comp = NULL;
	int ret = 0;

	oscl_param_check((stream != NULL), OMX_ErrorBadParameter, NULL);

	pthread_mutex_lock(&stream->lock);
	sink_comp = &stream->vsink_info.al_comp;
	ret = OMX_SetParameter(sink_comp->cmp_hdl,
				omx_index_vrender_disable_display, NULL);
	pthread_mutex_unlock(&stream->lock);
	return ret;
}

/**
 * video_stream_enable_display - use to turn on display after wakeup
 *
 * @handle: video stream handle
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int video_stream_enable_display(void *handle)
{
	video_stream_t *stream = (video_stream_t *)handle;
	comp_info_t *sink_comp = NULL;
	int ret = 0;

	oscl_param_check((stream != NULL), OMX_ErrorBadParameter, NULL);

	pthread_mutex_lock(&stream->lock);
	sink_comp = &stream->vsink_info.al_comp;
	ret = OMX_SetParameter(sink_comp->cmp_hdl,
				omx_index_vrender_enable_display, NULL);
	pthread_mutex_unlock(&stream->lock);

	return ret;
}

/**
 * video_stream_pause - pause video streaming
 *
 * @handle: video stream handle
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int video_stream_pause(void *handle)
{
	video_stream_t *stream = (video_stream_t *)handle;
	int ret = 0;

	oscl_param_check((stream != NULL), OMX_ErrorBadParameter, NULL);
	pthread_mutex_lock(&stream->lock);
	ret = vsrc_camera_pause(stream->camera);
	if (stream->src_vsink && stream->vsink_info.al_comp.state == OMX_StateExecuting)
		component_setstate(&stream->vsink_info.al_comp, OMX_StatePause);
	pthread_mutex_unlock(&stream->lock);

	return ret;
}

/**
 * video_stream_resume - resume video streaming
 *
 * @handle: video stream handle
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int video_stream_resume(void *handle)
{
	video_stream_t *stream = (video_stream_t *)handle;
	int ret = 0;

	oscl_param_check((stream != NULL), OMX_ErrorBadParameter, NULL);
	pthread_mutex_lock(&stream->lock);
	if (stream->src_vsink && stream->vsink_info.al_comp.state == OMX_StatePause)
		component_setstate(&stream->vsink_info.al_comp, OMX_StateExecuting);
	ret = vsrc_camera_resume(stream->camera);
	pthread_mutex_unlock(&stream->lock);

	return ret;
}

/**
 * video_stream_start_preview - start preview.
 *
 * @handle: video stream handle
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int video_stream_start_preview(void *handle)
{
	video_stream_t *stream = (video_stream_t *)handle;
	int ret = 0;

	OSCL_TRACE("start");
	oscl_param_check((stream != NULL), OMX_ErrorBadParameter, NULL);

	pthread_mutex_lock(&stream->lock);
	/* current state is preview, return with no err */
	if (stream->src_vsink) {
		OSCL_LOGW("preview is enabled, do nothing");
		goto EXIT;
	}

	pthread_mutex_unlock(&stream->lock);
	video_stream_start_camera(handle);
	pthread_mutex_lock(&stream->lock);

	ret = OMX_SetParameter(stream->vsink_info.al_comp.cmp_hdl,
			omx_index_vrender_mode,
			&stream->vsink_info.disp_para);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);

	/* get port and setup */
	stream->src_vsink = vsrc_camera_getport(stream->camera,
						stream->vsink_info.rotate);
	oscl_param_check_exit(stream->src_vsink != NULL, -1, NULL);
	ret = untunnel_setup_ports(stream->src_vsink, stream->vsink_info.vin);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);
	ret = component_setstate(&stream->vsink_info.al_comp, OMX_StateIdle);
	oscl_param_check_exit(ret == 0, ret, NULL);

	/* start preview */
	component_setstate(&stream->vsink_info.al_comp, OMX_StateExecuting);
	ret = vsrc_camera_enable_port(stream->camera, stream->src_vsink);

EXIT:
	pthread_mutex_unlock(&stream->lock);

	if (ret != OMX_ErrorNone) {
		OSCL_LOGE("video_stream_start_preview failed!");
		video_stream_stop_preview(handle);
	}
	OSCL_TRACE("end!");
	return 0;
}


/**
 * video_stream_stop_preview - stop preview.
 *
 * @handle: video stream handle
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int video_stream_stop_preview(void *handle)
{
	video_stream_t *stream = (video_stream_t *)handle;

	OSCL_TRACE("start");
	oscl_param_check((stream != NULL), OMX_ErrorBadParameter, NULL);

	pthread_mutex_lock(&stream->lock);

	/* preview is already stopped, do nothing */
	if (stream->src_vsink == NULL) {
		OSCL_LOGW("preview is stop, do nothing");
		goto EXIT;
	}

	/* change to idle state for vsink components*/
	component_setstate(&stream->vsink_info.al_comp, OMX_StateIdle);

	/* return port to video source */
	vsrc_camera_disable_port(stream->camera, stream->src_vsink);
	untunnel_unset_ports(stream->src_vsink, stream->vsink_info.vin);
	vsrc_camera_putport(stream->camera, stream->src_vsink);
	stream->src_vsink = NULL;

	/* change to load state for vsink components*/
	component_setstate(&stream->vsink_info.al_comp, OMX_StateLoaded);

EXIT:
	pthread_mutex_unlock(&stream->lock);
	OSCL_TRACE("end!");
	return 0;
}

/**
 * video_stream_set_frame_cb - set callback for a video stream and start the stream.
 *
 * @handle: video stream handle
 *
 * @cb: callback to video stream frame
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int video_stream_set_frame_cb(void *handle, app_frame_cb_t *cb)
{
	int ret = 0;
	video_stream_t *stream = (video_stream_t *)handle;
	int rot = VDISP_ROTATE_NONE;

	OSCL_TRACE("");
	oscl_param_check((stream != NULL), OMX_ErrorBadParameter, NULL);

	pthread_mutex_lock(&stream->lock);
	if (stream->src_vcb && stream->src_vcb->priv_data &&
		stream->src_vcb->cb_async.buf_handle != NULL) {
		vsrc_camera_disable_port(stream->camera, stream->src_vcb);
		untunnel_unset_cb(stream->src_vcb);
		vsrc_camera_putport(stream->camera, stream->src_vcb);
		stream->src_vcb = NULL;
	}
	if (cb && cb->buf_handle) {
		if (cb->type == VS_VIDEO_ROT_FRAME)
			rot = stream->rotate;
		stream->src_vcb = vsrc_camera_getport(stream->camera, rot);
		oscl_param_check_exit(stream->src_vcb, -1, NULL);
		untunnel_setup_cb(stream->src_vcb, cb);
		ret = vsrc_camera_enable_port(stream->camera, stream->src_vcb);
	}

EXIT:
	pthread_mutex_unlock(&stream->lock);
	return ret;
}

/**
 * video_stream_take_picture - set take picture callback.
 *
 * @handle: video stream handle
 *
 * @cb: callback (only for one frame) to video stream
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int video_stream_take_picture(void *handle, app_frame_cb_t *cb)
{
	video_stream_t *stream = (video_stream_t *)handle;
	int ret = 0;
	int rot = VDISP_ROTATE_NONE;

	OSCL_LOGI("");
	oscl_param_check((stream != NULL), OMX_ErrorBadParameter, NULL);

	pthread_mutex_lock(&stream->lock);
	if (stream->src_cb_once && cb == NULL) {
		vsrc_camera_disable_port(stream->camera, stream->src_cb_once);
		untunnel_unset_cb(stream->src_cb_once);
		vsrc_camera_putport(stream->camera, stream->src_cb_once);
		stream->src_cb_once = NULL;
	}
	if (cb == NULL)
		goto EXIT;

	/* image type is required */
	if ((cb->type != VS_IMAGE_FRAME) && (cb->type != VS_IMAGE_ROT_FRAME)) {
		ret = -1;
		goto EXIT;
	}

	if (stream->src_cb_once == NULL) {
		if (cb->type == VS_IMAGE_ROT_FRAME)
			rot = stream->rotate;
		stream->src_cb_once = vsrc_camera_getport(stream->camera, rot);
		oscl_param_check_exit(stream->src_cb_once, -1, NULL);
		/* setup and enable image outport, default callback == NULL */
		untunnel_setup_cb(stream->src_cb_once, cb);
		ret = vsrc_camera_enable_port(stream->camera, stream->src_cb_once);
	}
	memcpy(&stream->src_cb_once->cb_async, cb, sizeof(app_frame_cb_t));
EXIT:
	pthread_mutex_unlock(&stream->lock);
	return ret;
}

/**
 * video_stream_return_frame - return video frame get by video callback.
 *
 * @frame: frame tobe returned
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int video_stream_return_frame(frame_t *frame)
{
	return app_empty_buffer_done(frame);
}

int video_stream_set_port_cb(void *handle, app_port_filter_t *cb)
{
	int ret = 0;
	video_stream_t *stream = (video_stream_t *)handle;

	OSCL_TRACE("");
	oscl_param_check((stream && cb), OMX_ErrorBadParameter, NULL);
	oscl_param_check((stream->camera), OMX_ErrorBadParameter, NULL);

	pthread_mutex_lock(&stream->lock);
	if (cb->type == CB_VCAM_OUT || cb->type == CB_VROT_OUT)
		vsrc_camera_set_port_cb(stream->camera, cb);
	pthread_mutex_unlock(&stream->lock);
	return ret;
}

int video_stream_set_res(void *handle, vstream_buf_cfg_t *res)
{
	video_stream_t *stream = (video_stream_t *)handle;

	oscl_param_check((stream && res), OMX_ErrorBadParameter, NULL);
	return vsrc_camera_set_res(stream->camera, res);
}

