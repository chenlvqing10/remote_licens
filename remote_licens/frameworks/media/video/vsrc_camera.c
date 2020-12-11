/*
 * recorder_camera.c - Standard functionality for video stream recorder.
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
#include <oscl.h>
#include <base_component.h>
#include "vrender_component.h"
#include "vrec_component.h"
#include "omx_api.h"
#include "video_stream.h"
#include "omx_vendor_lb.h"
#include "vsrc_camera.h"

#define CAM_PRINT_VIDEO_INFO(video) \
OSCL_LOGI("eColorFormat:%d, w-h(%d, %d), (%d, %d)", (video).eColorFormat,\
	(video).nFrameWidth, (video).nFrameHeight, (video).nStride, (video).nSliceHeight);

#define REC_SRC_FRONT_CAMERA   "vic"
#define REC_SRC_BACK_CAMERA   "mcsi"
#define REC_DEFAULT_VIDEO_SRC	REC_SRC_FRONT_CAMERA

#define REC_INPUT_CHANNEL_NUM	1
#ifdef __linux__
#define CAMERA_DEFAULT_BUF_NUM	(8)
#define ROTATE_DEFAULT_BUF_NUM	(8)
#else
#define CAMERA_DEFAULT_BUF_NUM	(6)
#define ROTATE_DEFAULT_BUF_NUM	(3)
#endif

#define REC_DEF_VIDEO_COLOR_FMT OMX_COLOR_FormatYUV420SemiPlanar
#define REC_VIDEO_SIZE_VGA	(480)  /* 640*480 */
#define REC_VIDEO_SIZE_720P	(720)  /* 1280*720 */
#define REC_VIDEO_SIZE_1080P	(1080) /* 1920*1080 */
#define REC_VIDEO_SIZE_DEFAULT	REC_VIDEO_SIZE_720P

typedef struct vsrc_info {
	comp_info_t al_comp;
	OMX_VIDEO_PORTDEFINITIONTYPE video;
	char *dev_name;
	char *isp_name;
	int channel;
	port_info_t *vout;
} vsrc_info_t;

typedef struct _vsplt_info {
	comp_info_t al_comp;
	port_info_t *in;
	int map;
} vcam_splt_t;

typedef struct vrot_info {
	comp_info_t al_comp;
	port_info_t *vin;
	port_info_t *vout;
	app_port_filter_t cb_vout;
	OMX_CONFIG_ROTATIONTYPE mode;
	int nbuf;
} vrot_info_t;

typedef struct vsrc_camera {
	pthread_mutex_t *lock;
	vsrc_info_t vsrc_info;
	vcam_splt_t vsplt_info;
	vrot_info_t vrot_info;
	port_info_t *splt_rot;
	vcam_splt_t vrot_splt;
} vsrc_camera_t;

static port_info_t *_camera_get_spltport(vcam_splt_t *splt);
static int _camera_enable_spltport(vcam_splt_t *vsplt_info, port_info_t *port);

/**
 * _camera_get_config - get default config for camera source.
 *
 * @vsrc_info: video source handle
 *
 */
static int _camera_get_config(vsrc_camera_t *camera)
{
	vsrc_info_t *vsrc_info;
	int ret = 0;

	OSCL_TRACE("==");
	oscl_param_check_exit(camera != NULL, -1, NULL);

	/* default config for rotate component */
	camera->vrot_info.mode.nRotation = VDISP_ROTATE_90;
	camera->vrot_info.nbuf = ROTATE_DEFAULT_BUF_NUM;

	/* default config for rotate camera */
	vsrc_info = &camera->vsrc_info;
	vsrc_info->video.eColorFormat = REC_DEF_VIDEO_COLOR_FMT;
	if (REC_VIDEO_SIZE_DEFAULT == REC_VIDEO_SIZE_VGA) {
		vsrc_info->video.nFrameWidth = 640;
		vsrc_info->video.nFrameHeight = 480;
	} else if (REC_VIDEO_SIZE_DEFAULT == REC_VIDEO_SIZE_720P) {
		vsrc_info->video.nFrameWidth = 1280;
		vsrc_info->video.nFrameHeight = 720;
	} else if (REC_VIDEO_SIZE_DEFAULT == REC_VIDEO_SIZE_1080P) {
		vsrc_info->video.nFrameWidth = 1920;
		vsrc_info->video.nFrameHeight = 1080;
	} else {
		OSCL_LOGE("Do not supprot format.");
		vsrc_info->video.nFrameWidth = 1280;
		vsrc_info->video.nFrameHeight = 720;
	}
	vsrc_info->video.nStride = vsrc_info->video.nFrameWidth;
	vsrc_info->video.nSliceHeight = vsrc_info->video.nFrameHeight;
	vsrc_info->video.xFramerate = 30000;
	vsrc_info->dev_name = oscl_strdup(REC_DEFAULT_VIDEO_SRC);
	vsrc_info->channel = REC_INPUT_CHANNEL_NUM;
	vsrc_info->vout->nbuffer = CAMERA_DEFAULT_BUF_NUM;

	CAM_PRINT_VIDEO_INFO(vsrc_info->video);
EXIT:
	OSCL_TRACE("==");
	return ret;
}


/**
 * _camera_creat_component - rotate component.
 *
 * @camera: camera handle
 *
 */
static int _camera_creat_component(vsrc_camera_t *camera)
{
	int ret = 0;
	int index;
	oscl_param_check_exit(camera != NULL, -1, NULL);

	OSCL_TRACE("==");
	/* init video source component */
	ret = component_init(&camera->vsrc_info.al_comp, "OMX.LB.SOURCE.VREC",
			&untunnel_common_callbacks);
	oscl_param_check_exit(ret == 0, ret, NULL);
	index = component_get_port_index(&camera->vsrc_info.al_comp, OMX_DirOutput,
			OMX_PortDomainVideo);
	oscl_param_check_exit(index >= 0, -1, NULL);
	camera->vsrc_info.vout = &camera->vsrc_info.al_comp.port_info[index];

	/* init video splitter component */
	ret = component_init(&camera->vsplt_info.al_comp, "OMX.LB.SPLITTER.BASE",
			&untunnel_common_callbacks);
	oscl_param_check_exit(ret == 0, ret, NULL);
	camera->vsplt_info.in = &camera->vsplt_info.al_comp.port_info[0];

EXIT:
	OSCL_TRACE("==");
	return ret;
}

/**
 * _camera_creat_component - rotate component.
 *
 * @camera: camera handle
 *
 */
void _camera_release_component(vsrc_camera_t *camera)
{
	OSCL_TRACE("==");
	component_deinit(&camera->vsrc_info.al_comp);
	component_deinit(&camera->vsplt_info.al_comp);
	OSCL_TRACE("==");
}

static int _vsrc_prepare(vsrc_info_t *vsrc_info)
{
	OMX_ERRORTYPE ret = 0;
	comp_info_t *al_comp;
	int index;
	OMX_PARAM_PORTDEFINITIONTYPE port_para;
	port_info_t *port_cfg;
	OMX_COMPONENTTYPE *cmp_hdl;
	OMX_PARAM_U32TYPE para;

	OSCL_TRACE("==");
	/* set config to component */
	al_comp = &vsrc_info->al_comp;
	cmp_hdl = al_comp->cmp_hdl;
	ret = OMX_SetParameter(cmp_hdl, OMX_IndexParamVideoInit,
			(OMX_PTR)vsrc_info->dev_name);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);

	if (vsrc_info->isp_name && (strlen(vsrc_info->isp_name) > 0)) {
		ret = OMX_SetParameter(cmp_hdl, OMX_IndexParamOtherInit,
				(OMX_PTR)vsrc_info->isp_name);
		oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);
	}

	OMX_GetParameter(cmp_hdl, OMX_IndexParamNumAvailableStreams, (OMX_PTR)&para);
	oscl_param_check_exit((para.nU32 > 0), ret, NULL);

	index = component_get_port_index(al_comp, OMX_DirOutput, OMX_PortDomainVideo);
	oscl_param_check_exit(index >= 0, -1, NULL);
	port_cfg = &al_comp->port_info[index];

	/* set outport info, buffersize = 0 to uncare */
	memset(&port_para, 0, sizeof(port_para));
	port_para.nVersion.nVersion = OMX_VERSION;
	port_para.nPortIndex = index;
	ret = OMX_GetParameter(cmp_hdl, OMX_IndexParamPortDefinition,
			&port_para);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);
	/* if (port_cfg->nbuffer > port_para.nBufferCountActual) */
	if (port_cfg->nbuffer < port_para.nBufferCountMin)
		port_cfg->nbuffer = port_para.nBufferCountMin;
	port_para.nBufferCountActual = port_cfg->nbuffer;
	memcpy(&port_para.format.video, &vsrc_info->video,
		sizeof(port_para.format.video));
	CAM_PRINT_VIDEO_INFO(port_para.format.video);

	ret = OMX_SetParameter(cmp_hdl, OMX_IndexParamPortDefinition, &port_para);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);

	ret = OMX_GetParameter(cmp_hdl, OMX_IndexParamPortDefinition, &port_para);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);
	memcpy(&vsrc_info->video, &port_para.format.video,
		sizeof(port_para.format.video));
	port_cfg->nbuffer = port_para.nBufferCountActual;
	port_cfg->buf_size = port_para.nBufferSize;
	CAM_PRINT_VIDEO_INFO(vsrc_info->video);

EXIT:
	OSCL_TRACE("==");
	return ret;
}

static int _splt_prepare(vcam_splt_t *splt_info, OMX_PARAM_PORTDEFINITIONTYPE *port)
{
	int i = 0;
	int ret = 0;
	port_info_t *al_port;
	OMX_PARAM_PORTDEFINITIONTYPE tmp_port;

	OSCL_TRACE("==");
	memcpy(&tmp_port, port, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
	for (i = 0; i < splt_info->al_comp.num_port; i++) {
		al_port = &splt_info->al_comp.port_info[i];
		al_port->domain = tmp_port.eDomain;
		OSCL_LOGI("nbuffer(%d) edir(%d) domain(%d) size(%d)!",
			al_port->nbuffer,
			al_port->edir,
			al_port->domain,
			al_port->buf_size);
		if (al_port->edir == OMX_DirInput) {
			tmp_port.eDir = OMX_DirInput;
			tmp_port.nPortIndex = i;
			ret = OMX_SetParameter(splt_info->al_comp.cmp_hdl,
					OMX_IndexParamPortDefinition, &tmp_port);
			OSCL_LOGI("%d buf:%d", i, tmp_port.nBufferCountActual);
			CAM_PRINT_VIDEO_INFO(tmp_port.format.video);
			break;
		}
	}
	for (i = 0; i < splt_info->al_comp.num_port; i++) {
		al_port = &splt_info->al_comp.port_info[i];
		al_port->domain = tmp_port.eDomain;
		tmp_port.nPortIndex = i;
		ret = OMX_GetParameter(splt_info->al_comp.cmp_hdl,
				OMX_IndexParamPortDefinition, &tmp_port);
		al_port->nbuffer = tmp_port.nBufferCountActual;
		al_port->buf_size = tmp_port.nBufferSize;
		if (al_port->edir == OMX_DirOutput)
			al_port->is_shared_buffer = 1;
		OSCL_LOGI("splt_info->al_comp.cmp_hdl:%p, %d",
			splt_info->al_comp.cmp_hdl,
			tmp_port.nPortIndex);
		OSCL_LOGI("%d buf:%d", i, tmp_port.nBufferCountActual);
		CAM_PRINT_VIDEO_INFO(tmp_port.format.video);
	}
	OSCL_TRACE("==");
	return ret;
}

static int _rot_prepare(vsrc_camera_t *camera, vdisp_rotate_mode_e rot)
{
	OMX_PARAM_PORTDEFINITIONTYPE omx_port;
	int ret = 0;
	int index;

	OSCL_TRACE("==");
	/* init video rotate component */
	ret = component_init(&camera->vrot_info.al_comp, "OMX.LB.VIDEO.ROT",
			&untunnel_common_callbacks);
	oscl_param_check_exit(ret == 0, ret, NULL);
	index = component_get_port_index(&camera->vrot_info.al_comp, OMX_DirInput,
			OMX_PortDomainVideo);
	oscl_param_check_exit(index >= 0, -1, NULL);
	camera->vrot_info.vin = &camera->vrot_info.al_comp.port_info[index];
	index = component_get_port_index(&camera->vrot_info.al_comp, OMX_DirOutput,
			OMX_PortDomainVideo);
	oscl_param_check_exit(index >= 0, -1, NULL);
	camera->vrot_info.vout = &camera->vrot_info.al_comp.port_info[index];
	untunnel_set_filter(camera->vrot_info.vout, &camera->vrot_info.cb_vout);

	/* init video rotation splitter component */
	ret = component_init(&camera->vrot_splt.al_comp, "OMX.LB.SPLITTER.BASE",
			&untunnel_common_callbacks);
	oscl_param_check_exit(ret == 0, ret, NULL);
	camera->vrot_splt.in = &camera->vrot_splt.al_comp.port_info[0];

	camera->splt_rot = _camera_get_spltport(&camera->vsplt_info);
	oscl_param_check_exit(camera->splt_rot != NULL, -1, NULL);
	ret = untunnel_setup_ports(camera->splt_rot, camera->vrot_info.vin);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);

	camera->vrot_info.mode.nRotation = rot;
	/* command OMX_IndexConfigCommonRotate must be set after setup_ports,
	 * because setup_ports will set portdefinition to inport and
	 * OMX_IndexConfigCommonRotate will caculate output para dependson inport para*/
	OMX_SetParameter(camera->vrot_info.al_comp.cmp_hdl,
			OMX_IndexConfigCommonRotate,
			&camera->vrot_info.mode);

	omx_port.nPortIndex = camera->vrot_info.vout->index;
	omx_port.nVersion.nVersion = OMX_VERSION;
	ret = OMX_GetParameter(camera->vrot_info.al_comp.cmp_hdl,
			OMX_IndexParamPortDefinition, &omx_port);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);

	/* set rotation component buffer number */
	if (camera->vrot_info.nbuf > 0) {
		omx_port.nBufferCountActual = camera->vrot_info.nbuf;
		ret = OMX_SetParameter(camera->vrot_info.al_comp.cmp_hdl,
				OMX_IndexParamPortDefinition, &omx_port);
		oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);
		ret = OMX_GetParameter(camera->vrot_info.al_comp.cmp_hdl,
				OMX_IndexParamPortDefinition, &omx_port);
		oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);
		camera->vrot_info.vout->nbuffer = omx_port.nBufferCountActual;
	}


	_splt_prepare(&camera->vrot_splt, &omx_port);

	/* connect video source with video splitter */
	ret = untunnel_setup_ports(camera->vrot_info.vout,
			camera->vrot_splt.in);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);

	component_setstate(&camera->vrot_splt.al_comp, OMX_StateIdle);
	component_setstate(&camera->vrot_info.al_comp, OMX_StateIdle);
	component_setstate(&camera->vrot_splt.al_comp, OMX_StateExecuting);
	component_setstate(&camera->vrot_info.al_comp, OMX_StateExecuting);
	_camera_enable_spltport(&camera->vsplt_info, camera->splt_rot);

EXIT:
	if (ret != 0)
		OSCL_LOGE("vsrc_camera_prepare error! ret:%x", ret);
	OSCL_TRACE("==");
	return ret;

}

static int _is_splt_port(vcam_splt_t *splt, port_info_t *port)
{
	if (port == NULL)
		return 0;

	if (port == &splt->al_comp.port_info[port->index])
		return 1;
	return 0;
}

int _camera_prepare(vsrc_camera_t *camera)
{
	OMX_PARAM_PORTDEFINITIONTYPE omx_port;
	int ret = 0;

	OSCL_TRACE("==");
	if (camera->vsrc_info.al_comp.state != OMX_StateLoaded)
		return 0;

	/* setup para for recorder components*/
	ret = _vsrc_prepare(&camera->vsrc_info);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);

	omx_port.nPortIndex = camera->vsrc_info.vout->index;
	omx_port.nVersion.nVersion = OMX_VERSION;
	ret = OMX_GetParameter(camera->vsrc_info.al_comp.cmp_hdl,
			OMX_IndexParamPortDefinition, &omx_port);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);

	/* set port definition to splitter component */
	_splt_prepare(&camera->vsplt_info, &omx_port);

	/* connect video source with video splitter */
	ret = untunnel_setup_ports(camera->vsrc_info.vout,
			camera->vsplt_info.in);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);

	component_setstate(&camera->vsplt_info.al_comp, OMX_StateIdle);
	component_setstate(&camera->vsrc_info.al_comp, OMX_StateIdle);
	component_setstate(&camera->vsplt_info.al_comp, OMX_StateExecuting);
	component_setstate(&camera->vsrc_info.al_comp, OMX_StateExecuting);

EXIT:
	if (ret != 0)
		OSCL_LOGE("vsrc_camera_prepare error! ret:%x", ret);
	OSCL_TRACE("prepare end! ret:%x", ret);
	return ret;

}

int _camera_reset(vsrc_camera_t *camera)
{
	int ret = 0;
	OSCL_TRACE("==");
	if (camera->vsplt_info.map != 0 || camera->vrot_splt.map != 0) {
		ret = -1;
		goto EXIT;
	}
	if (camera->vsplt_info.al_comp.state == OMX_StateExecuting) {
		component_setstate(&camera->vsplt_info.al_comp, OMX_StateIdle);
		component_setstate(&camera->vsrc_info.al_comp, OMX_StateIdle);
		untunnel_unset_ports(camera->vsrc_info.vout,
				camera->vsplt_info.in);
	}
	if (camera->vsplt_info.al_comp.state == OMX_StateIdle) {
		component_setstate(&camera->vsplt_info.al_comp, OMX_StateLoaded);
		component_setstate(&camera->vsrc_info.al_comp, OMX_StateLoaded);
	}
EXIT:
	OSCL_TRACE("exit");
	return ret;

}

static void _camera_set_prio(vsrc_camera_t *camera)
{
#if defined(__EOS__)
	OMX_PRIORITYMGMTTYPE priority;

	OSCL_TRACE("==");
	priority.nVersion.nVersion = OMX_VERSION;
	priority.nGroupPriority = LB_RECORDER_VSRC_PRIO;
	OMX_SetParameter(camera->vsrc_info.al_comp.cmp_hdl,
		OMX_IndexParamPriorityMgmt, &priority);

	priority.nGroupPriority = LB_RECORDER_VSPLIT_PRIO;
	OMX_SetParameter(camera->vsplt_info.al_comp.cmp_hdl,
		OMX_IndexParamPriorityMgmt, &priority);

	if (camera->vrot_info.al_comp.cmp_hdl) {
		priority.nGroupPriority = LB_RECORDER_VIDEO_PRIO;
		OMX_SetParameter(camera->vrot_info.al_comp.cmp_hdl,
			OMX_IndexParamPriorityMgmt, &priority);

		priority.nGroupPriority = LB_RECORDER_VSPLIT_PRIO;
		OMX_SetParameter(camera->vrot_splt.al_comp.cmp_hdl,
			OMX_IndexParamPriorityMgmt, &priority);
	}

	OSCL_TRACE("==");
#endif
}

static port_info_t *_camera_get_spltport(vcam_splt_t *splt)
{
	port_info_t *port = NULL;
	port_info_t *splt_port = NULL;
	int nports;
	int index;

	OSCL_TRACE("==");
	oscl_param_check(splt != NULL, NULL, NULL);
	splt_port = splt->al_comp.port_info;
	nports = splt->al_comp.num_port;
	for (index = 0; index < nports; index++) {
		if (splt_port[index].edir == OMX_DirInput)
			continue;
		if ((splt->map & (1 << index)) == 0) {
			splt->map |= (1 << index);
			port = &splt_port[index];
			break;
		}
	}
	OSCL_TRACE("=index:%d, port:%p=", index, port);
	return port;
}

int _camera_put_spltport(vcam_splt_t *splt, port_info_t *port)
{
	port_info_t *splt_port = NULL;

	OSCL_TRACE("==");
	oscl_param_check(splt != NULL, -1, NULL);
	oscl_param_check(port != NULL, -1, NULL);

	splt_port = splt->al_comp.port_info;
	if (port == &splt_port[port->index]) {
		if ((splt->map & (1 << port->index)) == 0)
			OSCL_LOGE("free a port already freed");
		else
			splt->map &= ~(1 << port->index);
	}
	OSCL_TRACE("==");
	return 0;
}

static int _camera_enable_spltport(vcam_splt_t *vsplt_info, port_info_t *port)
{
	int ret;

	OSCL_TRACE("==");
	oscl_param_check(vsplt_info != NULL, -1, NULL);
	oscl_param_check(port != NULL, -1, NULL);

	ret = component_sendcom(&vsplt_info->al_comp, OMX_CommandPortEnable,
				port->index, NULL);
	OSCL_TRACE("==");
	return ret;
}

static int _camera_disable_spltport(vcam_splt_t *vsplt_info, port_info_t *port)
{
	int ret;

	OSCL_TRACE("==");
	oscl_param_check(vsplt_info != NULL, -1, NULL);
	oscl_param_check(port != NULL, -1, NULL);

	ret = component_sendcom(&vsplt_info->al_comp, OMX_CommandPortDisable,
		port->index, NULL);
	OSCL_TRACE("==");
	return ret;
}

int vsrc_camera_set_res(void *handle, vstream_buf_cfg_t *res)
{
	vsrc_camera_t *camera = handle;

	oscl_param_check(camera != NULL, -1, NULL);
	oscl_param_check(res != NULL, -1, NULL);
	if (camera->vsrc_info.al_comp.state != OMX_StateLoaded) {
		OSCL_LOGE("err state!");
		return -1;
	}
	if (res->nbuf_cam <= 0)
		res->nbuf_cam = CAMERA_DEFAULT_BUF_NUM;
	if (res->nbuf_rot <= 0)
		res->nbuf_rot = ROTATE_DEFAULT_BUF_NUM;
	OSCL_LOGE("src:%d-%d, rot:%d-%d", res->nbuf_cam, camera->vsrc_info.vout->nbuffer,
		res->nbuf_rot, camera->vrot_info.nbuf);
	camera->vsrc_info.vout->nbuffer = res->nbuf_cam;
	camera->vrot_info.nbuf = res->nbuf_rot;
	return 0;
}

/**
 * vsrc_camera_putport - get a free port from camera.
 *
 * @camera: camera handle
 *
 * @mode: rotate mode of port to get
 *
 * Returns port handle on success, NULL otherwise..
 */
port_info_t *vsrc_camera_getport(void *handle, vdisp_rotate_mode_e mode)
{
	port_info_t *port = NULL;
	vsrc_camera_t *camera = handle;
	int ret = 0;

	OSCL_TRACE("==");
	oscl_param_check(camera != NULL, NULL, NULL);

	pthread_mutex_lock(camera->lock);
	if (mode == VDISP_ROTATE_NONE) {
		port = _camera_get_spltport(&camera->vsplt_info);
		goto EXIT;
	}
	if (camera->vrot_splt.map != 0 && (mode != camera->vrot_info.mode.nRotation)) {
		OSCL_LOGE("current only support one rotate, cur rot:%d, setting rot:%d!",
			camera->vrot_info.mode.nRotation, mode);
		port = NULL;
		goto EXIT;
	}
	if (camera->vrot_splt.map == 0) {
		OSCL_LOGI("handle:%p _rot_prepare===", handle);
		_rot_prepare(camera, mode);
	}
	port = _camera_get_spltport(&camera->vrot_splt);

EXIT:
	pthread_mutex_unlock(camera->lock);
	if (ret != 0)
		port = NULL;
	OSCL_LOGI("get port:%p, index:%d", port, port->index);
	return port;
}

/**
 * vsrc_camera_putport - free a port get from camera.
 *
 * @camera: camera handle
 *
 * @port: port to be freed
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int vsrc_camera_putport(void *handle, port_info_t *port)
{
	int ret = 0;
	vsrc_camera_t *camera = handle;
	int is_rot_port = 0;

	OSCL_TRACE("==");
	oscl_param_check(camera != NULL, -1, NULL);
	oscl_param_check(port != NULL, -1, NULL);

	pthread_mutex_lock(camera->lock);
	is_rot_port = _is_splt_port(&camera->vrot_splt, port);
	if (is_rot_port)
		ret = _camera_put_spltport(&camera->vrot_splt, port);
	else
		ret = _camera_put_spltport(&camera->vsplt_info, port);

	if (is_rot_port && camera->vrot_splt.map == 0 && camera->splt_rot) {
		ret = _camera_disable_spltport(&camera->vsplt_info, camera->splt_rot);
		component_setstate(&camera->vrot_splt.al_comp, OMX_StateIdle);
		component_setstate(&camera->vrot_info.al_comp, OMX_StateIdle);
		untunnel_unset_ports(camera->vrot_info.vout, camera->vrot_splt.in);
		untunnel_unset_ports(camera->splt_rot, camera->vrot_info.vin);
		component_setstate(&camera->vrot_splt.al_comp, OMX_StateLoaded);
		component_setstate(&camera->vrot_info.al_comp, OMX_StateLoaded);
		component_deinit(&camera->vrot_info.al_comp);
		component_deinit(&camera->vrot_splt.al_comp);

		_camera_put_spltport(&camera->vsplt_info, camera->splt_rot);
		camera->splt_rot = NULL;
	}
	pthread_mutex_unlock(camera->lock);

	OSCL_LOGI("camera put ports, %d", port->index);
	return ret;
}

/**
 * vsrc_camera_getpara_portinfo - get port info from camera.
 *
 * @camera: camera handle
 * @port: port handle
 * @port: output port info
 *
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int vsrc_camera_getpara_portinfo(void *handle,
	port_info_t *port, OMX_PARAM_PORTDEFINITIONTYPE *portinfo)
{
	int ret = 0;
	vsrc_camera_t *camera = handle;
	OMX_COMPONENTTYPE *cmp_hdl = NULL;

	OSCL_TRACE("==");
	oscl_param_check(camera != NULL, -1, NULL);
	oscl_param_check(port != NULL, -1, NULL);
	oscl_param_check(portinfo != NULL, -1, NULL);

	memset(portinfo, 0, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
	pthread_mutex_lock(camera->lock);
	if (_is_splt_port(&camera->vrot_splt, port))
		cmp_hdl = camera->vrot_splt.al_comp.cmp_hdl;
	else if (_is_splt_port(&camera->vsplt_info, port))
		cmp_hdl = camera->vsplt_info.al_comp.cmp_hdl;
	else
		OSCL_LOGE("invalid port!");
	oscl_param_check_exit(cmp_hdl != NULL, -1, NULL);
	portinfo->nPortIndex = port->index;
	portinfo->nVersion.nVersion = OMX_VERSION;
	ret = OMX_GetParameter(cmp_hdl, OMX_IndexParamPortDefinition, portinfo);

EXIT:
	pthread_mutex_unlock(camera->lock);

	OSCL_TRACE("==");
	return ret;
}


/**
 * vsrc_camera_enable_port - enable a port get from camera.
 *
 * @camera: camera handle
 *
 * @port: port to be enabled
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int vsrc_camera_enable_port(void *handle, port_info_t *port)
{
	int ret = -1;
	vsrc_camera_t *camera = handle;

	OSCL_TRACE("==");
	oscl_param_check(camera != NULL, -1, NULL);

	pthread_mutex_lock(camera->lock);
	if (_is_splt_port(&camera->vrot_splt, port))
		ret = _camera_enable_spltport(&camera->vrot_splt, port);
	else
		ret = _camera_enable_spltport(&camera->vsplt_info, port);
	pthread_mutex_unlock(camera->lock);

	OSCL_TRACE("==");
	return ret;
}

/**
 * vsrc_camera_disable_port - disable a port get from camera.
 *
 * @camera: camera handle
 *
 * @port: port to be disabled
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int vsrc_camera_disable_port(void *handle, port_info_t *port)
{
	int ret = 0;
	vsrc_camera_t *camera = handle;

	OSCL_TRACE("==");
	oscl_param_check(camera != NULL, -1, NULL);
	oscl_param_check(port != NULL, -1, NULL);

	pthread_mutex_lock(camera->lock);
	if (_is_splt_port(&camera->vrot_splt, port))
		ret = _camera_disable_spltport(&camera->vrot_splt, port);
	else
		ret = _camera_disable_spltport(&camera->vsplt_info, port);
	pthread_mutex_unlock(camera->lock);

	OSCL_TRACE("==");
	return ret;
}

/**
 * video_rec_set_video_source - set source device of recorder.
 *
 * @video_rec: video recorder handle
 * @src_name: device name
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int vsrc_camera_set_device(void *handle, char *src_name)
{
	int ret = 0;
	vsrc_camera_t *camera = handle;

	OSCL_TRACE("==");
	oscl_param_check(camera != NULL, -1, NULL);
	oscl_param_check(src_name != NULL, -1, NULL);

	pthread_mutex_lock(camera->lock);
	if (camera->vsplt_info.map != 0 || camera->vrot_splt.map != 0) {
		OSCL_LOGE("ports busy! %x-%x",
			  camera->vsplt_info.map, camera->vrot_splt.map);
		goto EXIT;
	}
	if (strcmp(camera->vsrc_info.dev_name, src_name) == 0) {
		OSCL_LOGE("dev name not change! %s", src_name);
		goto EXIT;
	}

	if (camera->vsrc_info.dev_name) {
		OSCL_LOGW("switch video source:%s to %s",
			camera->vsrc_info.dev_name, src_name);
		oscl_free(camera->vsrc_info.dev_name);
	}
	camera->vsrc_info.dev_name = oscl_strdup(src_name);
	oscl_param_check_exit(camera->vsrc_info.dev_name != NULL,
		OMX_ErrorInsufficientResources, NULL);
	if (camera->vsrc_info.al_comp.state == OMX_StateLoaded)
		goto EXIT;
	ret = _camera_reset(camera);
	oscl_param_check_exit(ret == 0, -1, NULL);
EXIT:
	OSCL_TRACE("==");
	pthread_mutex_unlock(camera->lock);
	return ret;
}

int vsrc_camera_set_isp_inf_device(void *handle, char *src_name)
{
	int ret = 0;
	vsrc_camera_t *camera = handle;

	OSCL_TRACE("==");
	oscl_param_check(camera != NULL, -1, NULL);
	oscl_param_check(src_name != NULL, -1, NULL);

	pthread_mutex_lock(camera->lock);
	if (camera->vsplt_info.map != 0 || camera->vrot_splt.map != 0) {
		OSCL_LOGE("ports busy! %x-%x",
			  camera->vsplt_info.map, camera->vrot_splt.map);
		goto EXIT;
	}
	if (camera->vsrc_info.isp_name &&
		strcmp(camera->vsrc_info.isp_name, src_name) == 0) {
		OSCL_LOGE("dev name not change! %s", src_name);
		goto EXIT;
	}

	if (camera->vsrc_info.isp_name) {
		OSCL_LOGW("switch isp source:%s to %s",
			camera->vsrc_info.isp_name, src_name);
		oscl_free(camera->vsrc_info.isp_name);
	}
	camera->vsrc_info.isp_name = oscl_strdup(src_name);
	oscl_param_check_exit(camera->vsrc_info.isp_name != NULL,
		OMX_ErrorInsufficientResources, NULL);
	if (camera->vsrc_info.al_comp.state == OMX_StateLoaded)
		goto EXIT;
	ret = _camera_reset(camera);
	oscl_param_check_exit(ret == 0, -1, NULL);
EXIT:
	OSCL_TRACE("==");
	pthread_mutex_unlock(camera->lock);
	return ret;
}

/**
 * vsrc_camera_set_para - set para to camera.
 *
 * @camera: camera handle
 * @para: camera para
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int vsrc_camera_set_para(void *handle, vsrc_param_t *para)
{
	vsrc_camera_t *camera = handle;

	OSCL_TRACE("start");
	oscl_param_check(camera != NULL, -1, NULL);
	oscl_param_check(para != NULL, -1, NULL);

	pthread_mutex_lock(camera->lock);
	if (para->height) {
		camera->vsrc_info.video.nFrameHeight = para->height;
		camera->vsrc_info.video.nSliceHeight = para->height;
	}
	if (para->width) {
		camera->vsrc_info.video.nFrameWidth = para->width;
		camera->vsrc_info.video.nStride = para->width;
	}
	if (para->frame_rate)
		camera->vsrc_info.video.xFramerate = para->frame_rate;
	pthread_mutex_unlock(camera->lock);

	OSCL_TRACE("end!");
	return 0;
}


int vsrc_camera_set_port_cb(void *handle, app_port_filter_t *cb)
{
	int ret = 0;
	vsrc_camera_t *camera = handle;

	OSCL_TRACE("==");
	oscl_param_check((camera != NULL) && (cb != NULL), -1, NULL);

	pthread_mutex_lock(camera->lock);
	if (cb->type == CB_VCAM_OUT)
		untunnel_set_filter(camera->vsrc_info.vout, cb);
	if (cb->type == CB_VROT_OUT) {
		memcpy(&camera->vrot_info.cb_vout, cb, sizeof(app_port_filter_t));
		if (camera->vrot_info.vout)
			untunnel_set_filter(camera->vrot_info.vout, cb);
	}
	pthread_mutex_unlock(camera->lock);
	OSCL_TRACE("==");
	return ret;
}


/**
 * vsrc_camera_get_time - get current time.
 *
 * @camera: camera handle
 *
 * Returns 0 on success, -EERROR otherwise..
 */
signed long long vsrc_camera_get_time(void *handle)
{
	vsrc_camera_t *camera = handle;

	OMX_TIME_CONFIG_TIMESTAMPTYPE time;
	OSCL_TRACE("==");
	oscl_param_check(camera != NULL, -1, NULL);

	pthread_mutex_lock(camera->lock);
	time.nTimestamp = -1;
	OMX_GetConfig(camera->vsrc_info.al_comp.cmp_hdl,
			omx_index_lombo_config_cur_time, &time);
	OSCL_LOGI("start time:%lld", time.nTimestamp);
	if (camera->vsrc_info.al_comp.state != OMX_StateExecuting)
		OSCL_LOGE("set start time while video source not excuting");
	pthread_mutex_unlock(camera->lock);
	OSCL_TRACE("==");
	return time.nTimestamp;
}

/**
 * vsrc_camera_get_para - get para of camera.
 *
 * @camera: camera handle
 * @para: camera para
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int vsrc_camera_get_para(void *handle, vsrc_param_t *para)
{
	vsrc_camera_t *camera = handle;

	OSCL_TRACE("start");
	oscl_param_check(camera != NULL, -1, NULL);
	oscl_param_check(para != NULL, -1, NULL);

	pthread_mutex_lock(camera->lock);
	para->height = camera->vsrc_info.video.nFrameHeight;
	para->width = camera->vsrc_info.video.nFrameWidth;
	para->frame_rate = camera->vsrc_info.video.xFramerate;
	pthread_mutex_unlock(camera->lock);

	OSCL_TRACE("end!");
	return 0;
}


/**
 * vsrc_camera_prepare - set para to components.
 *
 * @camera: camera handle
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int vsrc_camera_prepare(void *handle)
{
	int ret = 0;
	vsrc_camera_t *camera = handle;

	OSCL_TRACE("==");
	oscl_param_check_exit(camera != NULL, -1, NULL);

	pthread_mutex_lock(camera->lock);
	ret = _camera_prepare(camera);
	pthread_mutex_unlock(camera->lock);
EXIT:
	if (ret != 0)
		OSCL_LOGE("vsrc_camera_prepare error! ret:%x", ret);

	OSCL_TRACE("prepare end! ret:%x", ret);
	OSCL_TRACE("==");
	return ret;
}


/**
 * vsrc_camera_reset - reset a camera source.
 *
 * @camera: camera handle
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int vsrc_camera_reset(void *handle)
{
	int ret = -1;
	vsrc_camera_t *camera = handle;

	OSCL_TRACE("==");
	if (camera != NULL) {
		pthread_mutex_lock(camera->lock);
		ret = _camera_reset(camera);
		pthread_mutex_unlock(camera->lock);
	}

	OSCL_TRACE("=ret:%x=", ret);
	return ret;
}


/**
 * vsrc_camera_release - release a camera source.
 *
 * @camera: camera handle
 *
 * Returns 0 on success, -EERROR otherwise..
 */
int vsrc_camera_release(void *handle)
{
	int ret = 0;
	vsrc_camera_t *camera = handle;

	OSCL_TRACE("==");
	if (camera == NULL)
		goto EXIT;
	if (camera->lock == NULL) {
		oscl_free(camera);
		goto EXIT;
	}
	pthread_mutex_lock(camera->lock);
	ret = _camera_reset(camera);
	if (ret != 0) {
		OSCL_LOGE("release camera failed!");
		pthread_mutex_unlock(camera->lock);
		goto EXIT;
	}
	_camera_release_component(camera);
	pthread_mutex_unlock(camera->lock);

	if (camera->vsrc_info.dev_name != NULL)
		oscl_free(camera->vsrc_info.dev_name);
	if (camera->lock) {
		pthread_mutex_destroy(camera->lock);
		oscl_free(camera->lock);
		camera->lock = NULL;
	}
	oscl_free(camera);
EXIT:
	OSCL_TRACE("==");
	return ret;
}

/**
 * vsrc_camera_creat - create a camera source.
 *
 * @camera: camera handle
 *
 * Returns handle on success, NULL otherwise..
 */
void *vsrc_camera_creat(void)
{
	int ret = -1;
	vsrc_camera_t *camera = NULL;
	OSCL_TRACE("==");

	camera = oscl_zalloc(sizeof(vsrc_camera_t));
	oscl_param_check_exit(camera != NULL, -1, NULL);

	camera->lock = oscl_zalloc(sizeof(pthread_mutex_t));
	oscl_param_check_exit(camera->lock != NULL, -1, NULL);
	pthread_mutex_init(camera->lock, NULL);

	pthread_mutex_lock(camera->lock);
	ret = _camera_creat_component(camera);
	_camera_get_config(camera);
	_camera_set_prio(camera);
	pthread_mutex_unlock(camera->lock);
EXIT:
	if (ret != 0) {
		OSCL_LOGE("vsrc_camera_creat create failed");
		vsrc_camera_release(camera);
		camera = NULL;
	}
	OSCL_TRACE("==");
	return camera;
}

/**
 * vsrc_camera_pause - pause camera streaming.
 *
 * @camera: camera handle
 *
 * Returns handle on success, NULL otherwise..
 */
int vsrc_camera_pause(void *handle)
{
	vsrc_camera_t *camera = handle;

	oscl_param_check(camera != NULL, -1, NULL);
	OSCL_LOGE("");
	if (camera->vsplt_info.al_comp.state == OMX_StateExecuting) {
		component_setstate(&camera->vsrc_info.al_comp, OMX_StatePause);
		component_setstate(&camera->vsplt_info.al_comp, OMX_StatePause);
	}
	if (camera->vrot_splt.map != 0 &&
		camera->vrot_info.al_comp.state == OMX_StateExecuting) {
		component_setstate(&camera->vrot_info.al_comp, OMX_StatePause);
		component_setstate(&camera->vrot_splt.al_comp, OMX_StatePause);
	}
	return OMX_EffectNone;
}

/**
 * vsrc_camera_resume - resume camera streaming.
 *
 * @camera: camera handle
 *
 * Returns handle on success, NULL otherwise..
 */
int vsrc_camera_resume(void *handle)
{
	vsrc_camera_t *camera = handle;

	oscl_param_check(camera != NULL, -1, NULL);
	if (camera->vrot_splt.map != 0 &&
		camera->vrot_info.al_comp.state == OMX_StatePause) {
		component_setstate(&camera->vrot_splt.al_comp, OMX_StateExecuting);
		component_setstate(&camera->vrot_info.al_comp, OMX_StateExecuting);
	}
	if (camera->vsplt_info.al_comp.state == OMX_StatePause) {
		component_setstate(&camera->vsplt_info.al_comp, OMX_StateExecuting);
		component_setstate(&camera->vsrc_info.al_comp, OMX_StateExecuting);
	}

	return OMX_EffectNone;
}

