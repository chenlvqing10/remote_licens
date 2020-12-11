/*
 * untunnel_common.c - Standard functionality for utunnel mode.
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
#define LOG_TAG			"component_ops"

#include "oscl.h"
#include "base_component.h"
#include "lb_omx_core.h"
#include "omx_api.h"
#include "media_debug.h"
/*
static omx_component_tbl_t omx_vrec_component = {
	"OMX.LB.SOURCE.VREC",
	1,
	vrec_component_init, NULL, NULL,
	{
		"source.vreceive",
		NULL
	}
};

static omx_component_tbl_t omx_vrender_component = {
	"OMX.LB.SINK.VRENDER",
	1,
	vrender_component_init, NULL, NULL,
	{
		"sink.vrender",
		NULL
	}
};

static omx_component_tbl_t omx_splitter_component = {
	"OMX.LB.SPLITTER.BASE",
	1,
	splitter_component_init, NULL, NULL,
	{
		"splitter",
		NULL
	}
};

static omx_component_tbl_t omx_rot_component = {
	"OMX.LB.VIDEO.ROT",
	1,
	rotate_component_init, NULL, NULL,
	{
		"video.rot",
		NULL
	}
};

static omx_component_tbl_t omx_venc_component = {
	"OMX.LB.VIDEO.ENCODECOMPONENT",
	1,
	video_encoder_component_init, NULL, NULL,
	{
		"video_encoder.jpeg",
		"video_encoder.h264",
		NULL
	}
};

static omx_component_tbl_t omx_mux_component = {
	"OMX.LB.SINK.MUXER",
	1,
	muxer_component_init, NULL, NULL,
	{
		"muxer.mp4",
		"muxer.raw",
		NULL
	}
};
*/

long media_init(void)
{
	int ret;

	ret = OMX_Init();
	if (ret) {
		OSCL_LOGE("OMX init failed");
		return ret;
	}
	media_debug_init();

/*
	omx_component_register(&omx_vrender_component);
	omx_component_register(&omx_vrec_component);
	omx_component_register(&omx_splitter_component);
	omx_component_register(&omx_rot_component);
	omx_component_register(&omx_venc_component);
	omx_component_register(&omx_mux_component);
*/
	return 0;
}

void media_deinit(void)
{
	media_debug_deinit();
	OMX_Deinit();
}

int component_init(comp_info_t *comp_info, char *name,
	OMX_CALLBACKTYPE *callbacks)
{
	int ret = OMX_ErrorNone;
	int i;
	OMX_PARAM_PORTDEFINITIONTYPE port_def;

	OSCL_TRACE("%p, %p, %p", comp_info, name, callbacks);
	oscl_param_check_exit((comp_info != NULL) && (name != NULL), -1, NULL);

	pthread_mutex_init(&comp_info->state_lock, NULL);
	comp_info->sem_cmd = oscl_zalloc(sizeof(sem_t));
	oscl_param_check_exit((comp_info->sem_cmd != NULL), -1, NULL);
	sem_init(comp_info->sem_cmd, 0, 0);

	comp_info->name = oscl_strdup(name);
	oscl_param_check_exit((comp_info->name != NULL), -1, NULL);

	ret = OMX_GetHandle((void **)&comp_info->cmp_hdl, name, comp_info, callbacks);
	oscl_param_check_exit((comp_info->cmp_hdl != NULL && ret == OMX_ErrorNone),
		ret, "get handle failed!");
	comp_info->state = OMX_StateLoaded;

	ret = OMX_GetParameter(comp_info->cmp_hdl,
			omx_index_vendor_get_port_number, &comp_info->num_port);
	oscl_param_check_exit((ret == OMX_ErrorNone), ret, NULL);
	oscl_param_check_exit((comp_info->num_port <= OMX_PORT_NUMBER_SUPPORTED),
		OMX_ErrorBadParameter, NULL);

	comp_info->port_info = oscl_zalloc(comp_info->num_port * sizeof(port_info_t));
	for (i = 0; i < comp_info->num_port; i++) {
		comp_info->port_info[i].index = -1;
		port_def.nVersion.nVersion = OMX_VERSION;
		port_def.nPortIndex = i;
		ret = OMX_GetParameter(comp_info->cmp_hdl,
				OMX_IndexParamPortDefinition, &port_def);
		if (ret != OMX_ErrorNone) {
			OSCL_LOGW("(%s)get port(%d) info err!", comp_info->name, i);
			continue;
		}
		comp_info->port_info[i].nbuffer = port_def.nBufferCountActual;
		comp_info->port_info[i].index = i;
		comp_info->port_info[i].state = PORT_STATE_INIT;
		comp_info->port_info[i].edir = port_def.eDir;
		comp_info->port_info[i].domain = port_def.eDomain;
		comp_info->port_info[i].comp_info = comp_info;
		comp_info->port_info[i].buf_size = port_def.nBufferSize;
		OSCL_LOGI("(%s)get port(%d) info sucess!", comp_info->name, i);
		OSCL_LOGI("nbuffer(%d) edir(%d) domain(%d) size(%d)!",
			comp_info->port_info[i].nbuffer,
			comp_info->port_info[i].edir,
			comp_info->port_info[i].domain,
			comp_info->port_info[i].buf_size);
	}
	mdebug_add_component(comp_info);

EXIT:
	if (ret != 0)
		component_deinit(comp_info);
	OSCL_TRACE("%x", ret);
	return ret;
}

void component_deinit(comp_info_t *comp_info)
{
	OSCL_TRACE("comp info:%p", comp_info);

	mdebug_rm_component(comp_info);
	comp_info->state = OMX_StateInvalid;
	if (comp_info->port_info) {
		int i;
		for (i = 0; i < comp_info->num_port; i++) {
			if (comp_info->port_info[i].hold_map != NULL) {
				oscl_free(comp_info->port_info[i].hold_map);
				comp_info->port_info[i].hold_map = NULL;
			}
		}
		oscl_free(comp_info->port_info);
		comp_info->port_info = NULL;
	}
	if (comp_info->cmp_hdl) {
		OMX_FreeHandle(comp_info->cmp_hdl);
		comp_info->cmp_hdl = NULL;
	}
	if (comp_info->sem_cmd) {
		sem_destroy(comp_info->sem_cmd);
		oscl_free(comp_info->sem_cmd);
	}
	if (comp_info->name)
		oscl_free(comp_info->name);
	pthread_mutex_destroy(&comp_info->state_lock);
	memset(comp_info, 0, sizeof(comp_info_t));

	OSCL_TRACE("exit");
}

/* note: only return first index fufill the quotions */
int component_get_port_index(comp_info_t *comp_info,
			OMX_DIRTYPE edir, OMX_PORTDOMAINTYPE domain)
{
	int i, ret = -1;

	for (i = 0; i < comp_info->num_port; i++) {
		if (comp_info->port_info[i].edir == edir
			&& comp_info->port_info[i].domain == domain) {
			ret = i;
			break;
		}
	}

	return ret;
}

OMX_ERRORTYPE component_setstate(comp_info_t *comp_info, OMX_STATETYPE s)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;
	OMX_STATETYPE cur_state;
	oscl_param_check(comp_info && comp_info->cmp_hdl, -1, NULL);
	OMX_GetState(comp_info->cmp_hdl, &cur_state);
	if (cur_state != s) {
		pthread_mutex_lock(&comp_info->state_lock);
		if (s == OMX_StateExecuting && cur_state == OMX_StateIdle)
			untunnel_queue_buffers(comp_info);
		comp_info->state = s;
		pthread_mutex_unlock(&comp_info->state_lock);
		OSCL_LOGI("%s component_setstate 0x%x!", comp_info->name, s);
		ret = OMX_SendCommand(comp_info->cmp_hdl, OMX_CommandStateSet, s, NULL);
		while (oscl_sem_timedwait_ms(comp_info->sem_cmd, 1000)) {
			OSCL_LOGE("%s wait state %x(%d) timeout",
				  comp_info->name, s, cur_state);
		}
		OSCL_LOGI("%s component_setstate 0x%x!", comp_info->name, s);
	}
	return ret;
}

OMX_ERRORTYPE component_sendcom(comp_info_t *comp_info,
	OMX_COMMANDTYPE cmd,
	OMX_U32 param,
	OMX_PTR cmd_data)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;

	OSCL_LOGI("%s send command 0x%x!", comp_info->name, cmd);
	ret = OMX_SendCommand(comp_info->cmp_hdl, cmd, param, cmd_data);
	if (ret != OMX_ErrorNone)
		OSCL_LOGE("%s send command err:%x!", comp_info->name, ret);
	while (oscl_sem_timedwait_ms(comp_info->sem_cmd, 1000))
		OSCL_LOGE("%s wait cmd %x compolete timeout", comp_info->name, cmd);
	return ret;
}

OMX_ERRORTYPE component_setparam(comp_info_t *comp_info,
				OMX_U32 param, OMX_PTR param_data)
{
	OMX_ERRORTYPE ret = OMX_ErrorNone;

	OSCL_LOGI("%s set parammeter 0x%x!", comp_info->name, param);
	ret = OMX_SetParameter(comp_info->cmp_hdl, param, param_data);
	if (ret)
		OSCL_LOGE("%s set parameter failed err:%x", comp_info->name, ret);

	return ret;
}


