/*
 * vrec_vrender.c - test vrec and vrender mod of omx component.
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
#define LOG_TAG			"vrec_vrend_test"

#include <oscl.h>
#include <base_component.h>
#include "vrender_component.h"
#include "vrec_component.h"

#define INPUT_BUF_NUM		8
#define OUTPUT_BUF_NUM		8

#define INPUT_CHANNEL_NUM	1
#define BUFFER_ALIGN_SIZE	64

#define CAM_SIZE_VGA		(480)  /* 640*480 */
#define CAM_SIZE_720P		(720)  /* 1280*720 */
#define CAM_SIZE_1080P		(1080) /* 1920*1080 */

#define FRONT_INPUT_DEVICE_NAME	"/dev/video3"
#define BACK_INPUT_DEVICE_NAME	"/dev/video1"

#define FRONT_LAYER_WIN_INDEX	DPU_DC_WIN_INDEX2
#define BACK_LAYER_WIN_INDEX	DPU_DC_WIN_INDEX3
#define FRONT_DISP_LAYER_NAME	"fdisp"
#define BACK_DISP_LAYER_NAME	"bdisp"

#define DISPLAY_DEVICE_NAME	"disp"

typedef enum cam_out_size {
	CAM_OUTPUT_SIZE_VGA,
	CAM_OUTPUT_SIZE_720P,
	CAM_OUTPUT_SIZE_1080P,
} cam_out_size_e;

typedef enum preview_mode {
	PREVIEW_MOD_FRONT_AND_BACK,
	PREVIEW_MOD_FRONT,
	PREVIEW_MOD_BACK,
} preview_mode_e;

typedef enum stream_id {
	FRONT_STREAM_ID,
	BACK_STREAM_ID,
} stream_id_e;

typedef struct vrec_vrender_test {
	stream_id_e id;
	OMX_COMPONENTTYPE *source;	/* video receive component */
	OMX_COMPONENTTYPE *sink;	/* video display component */
	OMX_S32 plane_size[3];		/* video size */
	OMX_PARAM_PORTDEFINITIONTYPE port_para;
	cam_out_size_e out_size;	/* sensor out video size */
	preview_mode_e pre_mode;	/* preview mode*/

	vdisp_para_t disp_para;		/* window display parameter */

	int port_src_out;		/* video receive component out port */
	int port_sink_in;		/* video display component input port */

	sem_t sem_event;
	sem_t sem_eos;

	OMX_S32 buf_num;		/* buffer number to sensor */
	OMX_BUFFERHEADERTYPE **buffer;
	OMX_U32 error_value;
} vrec_vrender_test_t;

static preview_mode_e prev_mode = PREVIEW_MOD_FRONT;

/* Callbacks implementation */
static OMX_ERRORTYPE vrec_event_handler(
	OMX_HANDLETYPE hComponent,
	OMX_PTR pAppData,
	OMX_EVENTTYPE eEvent,
	OMX_U32 Data1,
	OMX_U32 Data2,
	OMX_PTR pEventData)
{
	vrec_vrender_test_t *private = (vrec_vrender_test_t *)pAppData;
	char *name;
	OSCL_TRACE("hComponent:%p", hComponent);

	if (hComponent == private->sink)
		name = "sink";
	if (hComponent == private->source)
		name = "source";
	if (eEvent == OMX_EventCmdComplete) {
		if (Data1 == OMX_CommandStateSet) {
			switch ((int)Data2) {
			case OMX_StateInvalid:
				OSCL_LOGI("%s StateSet OMX_StateInvalid", name);
				break;
			case OMX_StateLoaded:
				OSCL_LOGI("%s StateSet OMX_StateLoaded", name);
				break;
			case OMX_StateIdle:
				OSCL_LOGI("%s StateSet OMX_StateIdle", name);
				break;
			case OMX_StateExecuting:
				OSCL_LOGI("%s StateSet OMX_StateExecuting", name);
				break;
			case OMX_StatePause:
				OSCL_LOGI("%s StateSet OMX_StatePause", name);
				break;
			case OMX_StateWaitForResources:
				OSCL_LOGI("%s StateSet WaitForResources", name);
				break;
			default:
				OSCL_LOGI("%s StateSet unkown state", name);
				break;
			}
			sem_post(&private->sem_event);
		} else if (Data1 == OMX_CommandPortEnable) {
			OSCL_LOGI("%s CmdComplete OMX_CommandPortEnable", name);
			sem_post(&private->sem_event);
		} else if (Data1 == OMX_CommandPortDisable) {
			OSCL_LOGI("%s CmdComplete OMX_CommandPortDisable", name);
			sem_post(&private->sem_event);
		}
	} else if (eEvent == OMX_EventBufferFlag) {
		if ((int)Data2 == OMX_BUFFERFLAG_EOS) {
			OSCL_LOGI("%s BufferFlag OMX_BUFFERFLAG_EOS", name);
			if (hComponent == private->sink) {
				OSCL_LOGE("end of tunnel");
				sem_post(&private->sem_eos);
			}
		} else
			OSCL_LOGI("%s OMX_EventBufferFlag %x", name, Data2);
	}  else if (eEvent == OMX_EventError) {
		private->error_value = Data1;
		OSCL_LOGE("Receive error event. value:%x", private->error_value);
		sem_post(&private->sem_event);
	} else {
		OSCL_LOGI("%s parm:%x %x", name, Data1, Data2);
	}

	return OMX_ErrorNone;
}

static void tunnel_exit(vrec_vrender_test_t *priv)
{
	if (priv->source)
		OMX_FreeHandle(priv->source);
	if (priv->sink)
		OMX_FreeHandle(priv->sink);

	priv->source = priv->sink = NULL;

	sem_destroy(&priv->sem_event);
	sem_destroy(&priv->sem_eos);

	OSCL_LOGD("here");
}

static int get_port_index(OMX_COMPONENTTYPE *comp, OMX_DIRTYPE dir,
	OMX_PORTDOMAINTYPE domain, int start)
{
	int i;
	int ret = -1;
	OMX_PARAM_PORTDEFINITIONTYPE port_def;

	port_def.nVersion.nVersion = OMX_VERSION;
	for (i = start; i < OMX_PORT_NUMBER_SUPPORTED; i++) {
		port_def.nPortIndex = i;
		ret = OMX_GetParameter(comp, OMX_IndexParamPortDefinition, &port_def);
		if (ret == OMX_ErrorNone && port_def.eDir == dir
			&& port_def.eDomain == domain) {
			ret = i;
			break;
		}
	}
	OSCL_LOGE("index:%d", i);

	if (i == OMX_PORT_NUMBER_SUPPORTED)
		OSCL_LOGE("can not get port, dir:%d, domain:%d, start:%d",
			dir, domain, start);

	return ret;
}

static OMX_ERRORTYPE get_screen_size(disp_size_t *size)
{
	void *hdl = NULL;

	oscl_param_check((size != NULL), OMX_ErrorBadParameter, NULL);
	hdl = oscl_open_disp_engine();
	if (hdl) {
		oscl_disp_get_screen_size(hdl, size);
		oscl_open_disp_engine(hdl);
	} else {
		return OMX_ErrorResourcesLost;
	}

	return OMX_ErrorNone;
}

static int tunnel_config_source_component(vrec_vrender_test_t *priv)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	char *name = NULL;
	OMX_PARAM_U32TYPE para;

	if (FRONT_STREAM_ID == priv->id) {
		name = FRONT_INPUT_DEVICE_NAME;
		priv->out_size = CAM_OUTPUT_SIZE_1080P;
	} else if (BACK_STREAM_ID == priv->id) {
		name = BACK_INPUT_DEVICE_NAME;
		priv->out_size = CAM_OUTPUT_SIZE_720P;
	}

	ret = OMX_SetParameter(priv->source, OMX_IndexParamVideoInit, (OMX_PTR)name);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);

	OMX_GetParameter(priv->source, OMX_IndexParamNumAvailableStreams, (OMX_PTR)&para);
	oscl_param_check_exit((para.nU32 > 0), ret, NULL);

	memset(&priv->port_para, 0, sizeof(priv->port_para));
	priv->port_para.nVersion.nVersion = OMX_VERSION;
	priv->port_para.nPortIndex = SOURCE_OUTPUT_PORT_INDEX;

	ret = OMX_GetParameter(priv->source, OMX_IndexParamPortDefinition,
			&priv->port_para);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);

	priv->port_para.nBufferCountActual = para.nU32 * INPUT_BUF_NUM;
	priv->port_para.format.video.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
	if (CAM_OUTPUT_SIZE_VGA == priv->out_size) {
		priv->port_para.format.video.nFrameWidth = 640;
		priv->port_para.format.video.nFrameHeight = 480;
		priv->port_para.format.video.nStride = 640;
		priv->port_para.format.video.nSliceHeight = 480;
	} else if (CAM_OUTPUT_SIZE_720P == priv->out_size) {
		priv->port_para.format.video.nFrameWidth = 1280;
		priv->port_para.format.video.nFrameHeight = 720;
		priv->port_para.format.video.nStride = 1280;
		priv->port_para.format.video.nSliceHeight = 720;
	} else if (CAM_OUTPUT_SIZE_1080P == priv->out_size) {
		priv->port_para.format.video.nFrameWidth = 1920;
		priv->port_para.format.video.nFrameHeight = 1080;
		priv->port_para.format.video.nStride = 1920;
		priv->port_para.format.video.nSliceHeight = 1080;
	} else {
		OSCL_LOGE("Do not supprot format.");
		return OMX_ErrorBadParameter;
	}

	priv->port_para.format.video.xFramerate = 30000; /* 25fps */
	OSCL_LOGD("source priv->port_para>> align:%d, w:%d h:%d, fps:%d",
		priv->port_para.nBufferAlignment,
		priv->port_para.format.video.nFrameWidth,
		priv->port_para.format.video.nFrameHeight,
		priv->port_para.format.video.xFramerate);

	ret = OMX_SetParameter(priv->source, OMX_IndexParamPortDefinition,
			&priv->port_para);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);

EXIT:
	return ret;
}

static OMX_ERRORTYPE tunnel_config_sink_component(vrec_vrender_test_t *priv)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	OMX_CONFIG_ROTATIONTYPE rot_mode;
	disp_size_t scn_size;

	memset(&priv->disp_para, 0, sizeof(priv->disp_para));
	memset(&scn_size, 0, sizeof(scn_size));
	ret = get_screen_size(&scn_size);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);

	if (FRONT_STREAM_ID == priv->id) {
		priv->disp_para.mode = VDISP_WINDOW_FULL_SCREEN_VIDEO_RATIO;
		priv->disp_para.win_rect.width = scn_size.width;
		priv->disp_para.win_rect.height = scn_size.height;
		priv->disp_para.win_rect.left =
			scn_size.width - priv->disp_para.win_rect.width;
		priv->disp_para.win_rect.top = 0;
	} else if (BACK_STREAM_ID == priv->id) {
		priv->disp_para.mode = VDISP_WINDOW_FULL_SCREEN_VIDEO_RATIO;
		priv->disp_para.win_rect.width = (scn_size.width * 3) / 8;
		priv->disp_para.win_rect.height = scn_size.height >> 1;
		priv->disp_para.win_rect.left =
			scn_size.width - priv->disp_para.win_rect.width;
		priv->disp_para.win_rect.top = 0;
	}

	ret = OMX_SetParameter(priv->sink, omx_index_vrender_mode,
			&priv->disp_para);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);

	rot_mode.nRotation = VDISP_ROTATE_NONE;
	ret = OMX_SetParameter(priv->sink, OMX_IndexConfigCommonRotate, &rot_mode);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);

	ret = OMX_SetParameter(priv->sink, OMX_IndexParamPortDefinition,
			&priv->port_para);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);

EXIT:
	return ret;
}

static OMX_CALLBACKTYPE tunnel_callbacks = {
	.EventHandler = vrec_event_handler,
	.EmptyBufferDone = NULL,
	.FillBufferDone = NULL,
};

static void *tunnel_stream_process_thread(void *param)
{
	int ret = OMX_ErrorNone;
	vrec_vrender_test_t *priv = NULL;

	priv = oscl_zalloc(sizeof(vrec_vrender_test_t));
	if (NULL == priv) {
		OSCL_LOGE("malloc fail.");
		return NULL;
	}
	priv->id = (stream_id_e)param;

	/* 1. get component handle */
	OMX_GetHandle((void **)&priv->source,
		"OMX.LB.SOURCE.VREC", priv, &tunnel_callbacks);
	OMX_GetHandle((void **)&priv->sink,
		"OMX.LB.SINK.VRENDER", priv, &tunnel_callbacks);
	OSCL_LOGD("priv->source:%p priv->sink:%p", priv->source, priv->sink);
	if (priv->source == NULL || priv->sink == NULL) {
		OSCL_LOGE("get handle failed! %p, %p", priv->source,
			priv->sink);
		OSCL_LOGE("here");
		goto EXIT;
	}
	OSCL_LOGD("here");

	sem_init(&priv->sem_event, 0, 0);
	sem_init(&priv->sem_eos, 0, 0);

	/* 2. get component port */
	priv->port_src_out = get_port_index(priv->source, OMX_DirOutput,
			OMX_PortDomainVideo, 0);
	priv->port_sink_in = get_port_index(priv->sink, OMX_DirInput,
			OMX_PortDomainVideo, 0);
	if (priv->port_src_out < 0 || priv->port_sink_in < 0)
		goto EXIT;

	/* 3. config source component */
	ret = tunnel_config_source_component(priv);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);

	/* 4. config sink component */
	ret = tunnel_config_sink_component(priv);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);

	/* 5. setup tunnel */
	OSCL_LOGD("OMX_SetupTunnel");
	ret = OMX_SetupTunnel(priv->source, priv->port_src_out,
			priv->sink, priv->port_sink_in);
	oscl_param_check_exit((ret == OMX_ErrorNone), ret, NULL);

	/* 6. change state to idle */
	OSCL_LOGD("source OMX_SendCommand OMX_StateIdle");
	ret = OMX_SendCommand(priv->source, OMX_CommandStateSet, OMX_StateIdle,
			NULL);
	oscl_param_check_exit((ret == OMX_ErrorNone), ret, NULL);

	sem_wait(&priv->sem_event);
	oscl_param_check_exit((priv->error_value == OMX_ErrorNone),
			priv->error_value, NULL);

	OSCL_LOGD("sink OMX_SendCommand OMX_StateIdle");
	ret = OMX_SendCommand(priv->sink, OMX_CommandStateSet, OMX_StateIdle,
			NULL);
	oscl_param_check_exit((ret == OMX_ErrorNone), ret, NULL);

	sem_wait(&priv->sem_event);
	oscl_param_check_exit((priv->error_value == OMX_ErrorNone),
			priv->error_value, NULL);

	/* 6. change state to executing */
	ret = OMX_SendCommand(priv->source, OMX_CommandStateSet,
			OMX_StateExecuting, NULL);
	sem_wait(&priv->sem_event);

	ret = OMX_SendCommand(priv->sink, OMX_CommandStateSet,
			OMX_StateExecuting, NULL);
	sem_wait(&priv->sem_event);

#if 1
	sem_wait(&priv->sem_eos);
#else
	mdelay(1500); /* delay */
	OSCL_LOGD("quit test...");
#endif
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);

	ret = OMX_SendCommand(priv->source, OMX_CommandStateSet, OMX_StateIdle,
			NULL);
	sem_wait(&priv->sem_event);

	ret = OMX_SendCommand(priv->sink, OMX_CommandStateSet, OMX_StateIdle,
			NULL);
	sem_wait(&priv->sem_event);

	OSCL_LOGD("here");

EXIT:
	if (priv->source) {
		ret = OMX_SendCommand(priv->source, OMX_CommandStateSet,
				OMX_StateLoaded, NULL);
		sem_wait(&priv->sem_event);
	}

	if (priv->sink) {
		ret = OMX_SendCommand(priv->sink, OMX_CommandStateSet,
				OMX_StateLoaded, NULL);
		sem_wait(&priv->sem_event);
	}

	tunnel_exit(priv);
	oscl_free(priv);

	pthread_exit(NULL);

	return NULL;
}

int openmax_test_vrec_vrender_tunnel(void)
{
	int ret = OMX_ErrorNone;
	pthread_t fthread_id = 0;
	pthread_t bthread_id = 0;

	OSCL_LOGD("test start...");
	if (PREVIEW_MOD_FRONT_AND_BACK == prev_mode) {
		pthread_create(&fthread_id,
				NULL,
				tunnel_stream_process_thread,
				(void *)FRONT_STREAM_ID);
		oscl_mdelay(1000);
		pthread_create(&bthread_id,
				NULL,
				tunnel_stream_process_thread,
				(void *)BACK_STREAM_ID);
	} else if (PREVIEW_MOD_FRONT == prev_mode) {
		pthread_create(&fthread_id,
				NULL,
				tunnel_stream_process_thread,
				(void *)FRONT_STREAM_ID);
	} else if (PREVIEW_MOD_BACK == prev_mode) {
		pthread_create(&bthread_id,
				NULL,
				tunnel_stream_process_thread,
				(void *)BACK_STREAM_ID);
	}

	OSCL_LOGD("Wait thread exit");
	if (fthread_id)
		pthread_join(fthread_id, NULL);
	if (bthread_id)
		pthread_join(bthread_id, NULL);
	OSCL_LOGD("Test complete....");

	return ret;
}

static OMX_ERRORTYPE alloc_buffer(vrec_vrender_test_t *priv, OMX_S32 num, OMX_S32 size)
{
	OMX_S32 i = 0;
	OMX_S32 port_index = SOURCE_OUTPUT_PORT_INDEX;
	OMX_ERRORTYPE ret = OMX_ErrorNone;

	priv->buffer = oscl_zalloc(num * sizeof(OMX_BUFFERHEADERTYPE *));
	if (NULL == priv->buffer)
		return OMX_ErrorBadParameter;

	for (i = 0; i < num; i++) {
		priv->buffer[i] = NULL;
		ret = OMX_AllocateBuffer(priv->source, &priv->buffer[i],
				port_index, priv, size);
		if (ret != OMX_ErrorNone) {
			OSCL_LOGE("Error on AllocateBuffer %p on port %i\n",
				&priv->buffer[i], port_index);
			break;
		}
		OSCL_LOGI("AllocateBuffer %p on port %i", priv->buffer[i], port_index);
		priv->buf_num++;
	}

	return ret;
}

static void free_buffer(vrec_vrender_test_t *priv)
{
	OMX_S32 i = 0;
	OMX_ERRORTYPE ret = OMX_ErrorNone;

	if (priv->buffer) {
		for (i = 0; i < priv->buf_num; i++) {
			if (priv->buffer[i]) {
				OSCL_LOGI("FreeBuffer %p, pBuffer:%p",
					priv->buffer[i], priv->buffer[i]->pBuffer);
				ret = OMX_FreeBuffer(priv->source,
						SOURCE_OUTPUT_PORT_INDEX,
						priv->buffer[i]);
				if (ret != OMX_ErrorNone)
					OSCL_LOGE("port %d ,freebuffer:%d failed",
						SOURCE_OUTPUT_PORT_INDEX, i);
			}
			priv->buffer[i] = NULL;
		}
		priv->buf_num = 0;
		oscl_free(priv->buffer);
		priv->buffer = NULL;
	}
}

static void untunel_exit(vrec_vrender_test_t *priv)
{
	OMX_FreeHandle(priv->source);
	priv->source = NULL;

	OMX_FreeHandle(priv->sink);
	priv->sink = NULL;

	sem_destroy(&priv->sem_event);
	sem_destroy(&priv->sem_eos);

	OSCL_LOGD("here");
}

static int untunnel_config_source_component(vrec_vrender_test_t *priv)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	char *name = NULL;
	OMX_PARAM_U32TYPE para;
	OMX_PARAM_PORTDEFINITIONTYPE port_def;

	if (FRONT_STREAM_ID == priv->id) {
		name = FRONT_INPUT_DEVICE_NAME;
		priv->out_size = CAM_OUTPUT_SIZE_1080P;
	} else if (BACK_STREAM_ID == priv->id) {
		name = BACK_INPUT_DEVICE_NAME;
		priv->out_size = CAM_OUTPUT_SIZE_720P;
	}

	ret = OMX_SetParameter(priv->source, OMX_IndexParamVideoInit, (OMX_PTR)name);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);

	OMX_GetParameter(priv->source, OMX_IndexParamNumAvailableStreams, (OMX_PTR)&para);
	oscl_param_check_exit((para.nU32 > 0), ret, NULL);

	memset(&priv->port_para, 0, sizeof(priv->port_para));
	ret = OMX_GetParameter(priv->source, OMX_IndexParamPortDefinition,
			&priv->port_para);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);

	priv->port_para.nBufferCountActual = para.nU32 * INPUT_BUF_NUM;
	priv->port_para.format.video.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
	if (CAM_OUTPUT_SIZE_VGA == priv->out_size) {
		priv->port_para.format.video.nFrameWidth = 640;
		priv->port_para.format.video.nFrameHeight = 480;
		priv->port_para.format.video.nStride = 640;
		priv->port_para.format.video.nSliceHeight = 480;
	} else if (CAM_OUTPUT_SIZE_720P == priv->out_size) {
		priv->port_para.format.video.nFrameWidth = 1280;
		priv->port_para.format.video.nFrameHeight = 720;
		priv->port_para.format.video.nStride = 1280;
		priv->port_para.format.video.nSliceHeight = 720;
	} else if (CAM_OUTPUT_SIZE_1080P == priv->out_size) {
		priv->port_para.format.video.nFrameWidth = 1920;
		priv->port_para.format.video.nFrameHeight = 1080;
		priv->port_para.format.video.nStride = 1920;
		priv->port_para.format.video.nSliceHeight = 1080;
	} else {
		OSCL_LOGE("Do not supprot format.");
		return OMX_ErrorBadParameter;
	}
	priv->port_para.nBufferAlignment = 1024;
	priv->port_para.format.video.xFramerate = 30000; /* 25fps */
	OSCL_LOGD("source priv->port_para>> align:%d, w:%d h:%d, fps:%d",
		priv->port_para.nBufferAlignment,
		priv->port_para.format.video.nFrameWidth,
		priv->port_para.format.video.nFrameHeight,
		priv->port_para.format.video.xFramerate);
	ret = OMX_SetParameter(priv->source, OMX_IndexParamPortDefinition,
			&priv->port_para);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);

	port_def.nVersion.nVersion = OMX_VERSION;
	port_def.nPortIndex = SOURCE_OUTPUT_PORT_INDEX;
	ret = OMX_GetParameter(priv->source, OMX_IndexParamPortDefinition, &port_def);
	if (ret != OMX_ErrorNone) {
		OSCL_LOGE("Error when getting OMX_PORT_PARAM_TYPE,%x\n", ret);
		goto EXIT;
	}

	if ((0 == port_def.nBufferSize) || (0 == port_def.nBufferCountActual)) {
		OSCL_LOGE("Error when getting OMX_PORT_PARAM_TYPE");
		goto EXIT;
	}

	OSCL_LOGI("port:%d, bufnum:%d", port_def.nPortIndex, port_def.nBufferCountActual);
	ret = alloc_buffer(priv, port_def.nBufferCountActual, port_def.nBufferSize);
EXIT:
	return ret;
}

static OMX_ERRORTYPE untunnel_config_sink_component(vrec_vrender_test_t *priv)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	OMX_PARAM_PORTDEFINITIONTYPE port_def;
	OMX_CONFIG_ROTATIONTYPE rot_mode;
	disp_size_t scn_size;

	memset(&priv->disp_para, 0, sizeof(priv->disp_para));
	memset(&scn_size, 0, sizeof(scn_size));

	ret = get_screen_size(&scn_size);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);

	OSCL_LOGD("screen, w:%d, h:%d", scn_size.width, scn_size.height);

	if (FRONT_STREAM_ID == priv->id) {
		priv->disp_para.mode = VDISP_WINDOW_FULL_SCREEN_VIDEO_RATIO;
		priv->disp_para.win_rect.width = scn_size.width;
		priv->disp_para.win_rect.height = scn_size.height;
		priv->disp_para.win_rect.left =
			scn_size.width - priv->disp_para.win_rect.width;
		priv->disp_para.win_rect.top = 0;
	} else if (BACK_STREAM_ID == priv->id) {
		priv->disp_para.mode = VDISP_WINDOW_FULL_SCREEN_VIDEO_RATIO;
		priv->disp_para.win_rect.width = (scn_size.width * 3) / 8;
		priv->disp_para.win_rect.height = scn_size.height >> 1;
		priv->disp_para.win_rect.left =
			scn_size.width - priv->disp_para.win_rect.width;
		priv->disp_para.win_rect.top = 0;
	}

	ret = OMX_SetParameter(priv->sink, omx_index_vrender_mode,
			&priv->disp_para);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);

	rot_mode.nRotation = VDISP_ROTATE_NONE;
	ret = OMX_SetParameter(priv->sink, OMX_IndexConfigCommonRotate, &rot_mode);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);

	ret = OMX_SetParameter(priv->sink, OMX_IndexParamPortDefinition,
			&priv->port_para);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);

	port_def.nVersion.nVersion = OMX_VERSION;
	port_def.nPortIndex = SINK_INPUT_PORT_INDEX;
	ret = OMX_GetParameter(priv->source, OMX_IndexParamPortDefinition, &port_def);
	if (ret != OMX_ErrorNone) {
		OSCL_LOGE("Error when getting OMX_PORT_PARAM_TYPE,%x\n", ret);
		goto EXIT;
	}

	if ((priv->port_para.nBufferSize != port_def.nBufferSize) ||
		(priv->port_para.nBufferCountActual != port_def.nBufferCountActual)) {
		OSCL_LOGE("Error when getting OMX_PORT_PARAM_TYPE");
		goto EXIT;
	}
	OSCL_LOGD("port:%d, bufnum:%d", port_def.nPortIndex, port_def.nBufferCountActual);

EXIT:
	return ret;
}

static OMX_ERRORTYPE vrec_empty_buffer_done(
	OMX_HANDLETYPE hComponent,
	OMX_PTR pAppData,
	OMX_BUFFERHEADERTYPE *pBuffer)
{
	OMX_ERRORTYPE err;
	vrec_vrender_test_t *priv = pAppData;

	if (pBuffer == NULL || pAppData == NULL) {
		OSCL_LOGE("err: buffer header is null");
		return OMX_ErrorBadParameter;
	}
	OSCL_LOGD("empty buffer done(%s)>> %p, %d, input:%d output:%d",
			get_component_name((get_lb_component(hComponent))),
			pBuffer->pBuffer,
			pBuffer->nFlags,
			pBuffer->nInputPortIndex,
			pBuffer->nOutputPortIndex);
	pBuffer->nOutputPortIndex = SOURCE_OUTPUT_PORT_INDEX;
	err = OMX_FillThisBuffer(priv->source, pBuffer);
	if (err != OMX_ErrorNone)
		OSCL_LOGE("OMX_FillThisBuffer err: %x", err);

	return err;
}

static OMX_ERRORTYPE vrec_fill_buffer_done(
	OMX_HANDLETYPE hComponent,
	OMX_PTR pAppData,
	OMX_BUFFERHEADERTYPE *pBuffer)
{
	OMX_ERRORTYPE err;
	vrec_vrender_test_t *priv = pAppData;

	if (pBuffer == NULL || pAppData == NULL) {
		OSCL_LOGE("err: buffer header is null");
		return OMX_ErrorBadParameter;
	}
	OSCL_LOGD("fill buffer done(%s)>> %p, %d, input:%d output:%d",
			get_component_name((get_lb_component(hComponent))),
			pBuffer->pBuffer,
			pBuffer->nFlags,
			pBuffer->nInputPortIndex,
			pBuffer->nOutputPortIndex);

	pBuffer->nInputPortIndex = SINK_INPUT_PORT_INDEX;
	err = OMX_EmptyThisBuffer(priv->sink, pBuffer);
	if (err != OMX_ErrorNone)
		OSCL_LOGE("OMX_EmptyThisBuffer err: %x", err);

	return err;
}

static OMX_CALLBACKTYPE vrec_untunnel_callbacks = {
	.EventHandler = vrec_event_handler,
	.EmptyBufferDone = vrec_empty_buffer_done,
	.FillBufferDone = vrec_fill_buffer_done,
};

static void *untunnel_stream_process_thread(void *param)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	vrec_vrender_test_t *priv = NULL;
	OMX_S32 i = 0;
	OMX_PARAM_PORTDEFINITIONTYPE port_def;

	priv = oscl_zalloc(sizeof(vrec_vrender_test_t));
	if (NULL == priv) {
		OSCL_LOGE("malloc fail.");
		return NULL;
	}
	priv->id = (stream_id_e)param;

	/* 1. get component handle */
	OMX_GetHandle((void **)&priv->source,
		"OMX.LB.SOURCE.VREC", priv, &vrec_untunnel_callbacks);
	OMX_GetHandle((void **)&priv->sink,
		"OMX.LB.SINK.VRENDER", priv, &vrec_untunnel_callbacks);
	OSCL_LOGD("here");

	if (priv->source == NULL || priv->sink == NULL) {
		OSCL_LOGE("get handle failed! %p, %p", priv->source,
			priv->sink);
		OSCL_LOGE("here");
		goto EXIT;
	}
	OSCL_LOGD("here");
again:
	sem_init(&priv->sem_event, 0, 0);
	sem_init(&priv->sem_eos, 0, 0);

	/* 2. config video receive component */
	ret = untunnel_config_source_component(priv);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);

	/* 3. config video receive component */
	ret = untunnel_config_sink_component(priv);
	oscl_param_check_exit(ret == OMX_ErrorNone, ret, NULL);

	/* 4. set component stat to idle */
	ret = OMX_SendCommand(priv->source, OMX_CommandStateSet, OMX_StateIdle, NULL);
	oscl_param_check_exit((ret == OMX_ErrorNone), ret, NULL);
	sem_wait(&priv->sem_event);

	ret = OMX_SendCommand(priv->sink, OMX_CommandStateSet, OMX_StateIdle, NULL);
	oscl_param_check_exit((ret == OMX_ErrorNone), ret, NULL);
	sem_wait(&priv->sem_event);

	/* 5. send buffer to source component queue */
	port_def.nVersion.nVersion = OMX_VERSION;
	port_def.nPortIndex = SOURCE_OUTPUT_PORT_INDEX;
	ret = OMX_GetParameter(priv->source, OMX_IndexParamPortDefinition, &port_def);
	if (ret != OMX_ErrorNone) {
		OSCL_LOGE("Error when getting OMX_PORT_PARAM_TYPE,%x\n", ret);
		goto EXIT;
	}
	OSCL_LOGD("nBufferCountActual:%d", port_def.nBufferCountActual);
	for (i = 0; i < port_def.nBufferCountActual; i++) {
		ret = OMX_FillThisBuffer(priv->source, priv->buffer[i]);
		OSCL_LOGI("OMX_FillThisBuffer %p on port %i", priv->buffer[i],
			priv->buffer[i]->nOutputPortIndex);
		oscl_param_check_exit((ret == OMX_ErrorNone), ret, NULL);
	}

	/* 6. set component stat to executing */
	ret = OMX_SendCommand(priv->source, OMX_CommandStateSet,
			OMX_StateExecuting, NULL);
	oscl_param_check_exit((ret == OMX_ErrorNone), ret, NULL);
	sem_wait(&priv->sem_event);

	ret = OMX_SendCommand(priv->sink, OMX_CommandStateSet, OMX_StateExecuting, NULL);
	oscl_param_check_exit((ret == OMX_ErrorNone), ret, NULL);
	sem_wait(&priv->sem_event);

#if 1
	sem_wait(&priv->sem_eos);
#else
	mdelay(5000); /* delay */
	OSCL_LOGD("quit test...");
#endif

	/* 6. set component stat to idle */
	ret = OMX_SendCommand(priv->sink, OMX_CommandStateSet, OMX_StateIdle, NULL);
	oscl_param_check_exit((ret == OMX_ErrorNone), ret, NULL);
	sem_wait(&priv->sem_event);

	ret = OMX_SendCommand(priv->source, OMX_CommandStateSet, OMX_StateIdle, NULL);
	oscl_param_check_exit((ret == OMX_ErrorNone), ret, NULL);
	sem_wait(&priv->sem_event);

	/* 7. free buffer */
	free_buffer(priv);

	/* 8. set component stat to loaded */
	ret = OMX_SendCommand(priv->source, OMX_CommandStateSet, OMX_StateLoaded, NULL);
	oscl_param_check_exit((ret == OMX_ErrorNone), ret, NULL);
	sem_wait(&priv->sem_event);

	ret = OMX_SendCommand(priv->sink, OMX_CommandStateSet, OMX_StateLoaded, NULL);
	oscl_param_check_exit((ret == OMX_ErrorNone), ret, NULL);
	sem_wait(&priv->sem_event);

	/* system("cat /tmp/ion/heaps/carvout"); */
	goto again;

EXIT:
	untunel_exit(priv);
	oscl_free(priv);
	pthread_exit(NULL);

	return NULL;
}


int openmax_test_vrec_vrender_untunnel(void)
{
	int ret = OMX_ErrorNone;
	pthread_t fthread_id = 0;
	pthread_t bthread_id = 0;

	OSCL_LOGD("untunnel test start...");
	if (PREVIEW_MOD_FRONT_AND_BACK == prev_mode) {
		OSCL_LOGD("untunnel_stream_process_thread FRONT_STREAM_ID");

		pthread_create(&fthread_id,
				NULL,
				untunnel_stream_process_thread,
				(void *)FRONT_STREAM_ID);

		/* ensure for FRONT_STREAM_ID thread having the disp plane */
		oscl_mdelay(1000 * 100);

		OSCL_LOGD("untunnel_stream_process_thread BACK_STREAM_ID");
		pthread_create(&bthread_id,
				NULL,
				untunnel_stream_process_thread,
				(void *)BACK_STREAM_ID);
	} else if (PREVIEW_MOD_FRONT == prev_mode) {
		pthread_create(&fthread_id,
				NULL,
				untunnel_stream_process_thread,
				(void *)FRONT_STREAM_ID);
	} else if (PREVIEW_MOD_BACK == prev_mode) {
		pthread_create(&bthread_id,
				NULL,
				untunnel_stream_process_thread,
				(void *)BACK_STREAM_ID);
	}

	OSCL_LOGD("Wait thread exit");
	if (fthread_id)
		pthread_join(fthread_id, NULL);

	if (bthread_id)
		pthread_join(bthread_id, NULL);

	OSCL_LOGD("test complete....");

	return ret;
}


