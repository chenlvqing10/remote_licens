/*
 * vsrc_manager.c - Standard functionality for video source manager.
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
#include "oscl.h"
#include "omx_api.h"
#include "base_component.h"
#include "video_stream.h"
#include "vsrc_camera.h"

#define MAX_CAM_OPENED 4

typedef struct _vsrc_manager {
	char *dev_name[MAX_CAM_OPENED];
	void *vsrc_hdl[MAX_CAM_OPENED];
	int refcnt[MAX_CAM_OPENED];
} vsrc_manager_t;

vsrc_manager_t vsrc_manager;
static pthread_mutex_t _manager_lock = PTHREAD_MUTEX_INITIALIZER;

void *vsrc_manager_get_device(char *name)
{
	int i;
	void *hdl = NULL;
	oscl_param_check(name != NULL, NULL, NULL);

	pthread_mutex_lock(&_manager_lock);
	for (i = 0; (i < MAX_CAM_OPENED) && (hdl == NULL); i++) {
		if (vsrc_manager.vsrc_hdl[i] && vsrc_manager.dev_name[i]) {
			if (strcmp(vsrc_manager.dev_name[i], name) == 0) {
				hdl = vsrc_manager.vsrc_hdl[i];
				vsrc_manager.refcnt[i]++;
			}
		}
	}
	if (hdl == NULL) {
		OSCL_LOGI("device %s is not in vsrc manager!", name);
		for (i = 0; i < MAX_CAM_OPENED; i++) {
			if (vsrc_manager.vsrc_hdl[i] == NULL)
				break;
		}
		if (i < MAX_CAM_OPENED) {
			hdl = vsrc_camera_creat();
			if (hdl)
				vsrc_manager.dev_name[i] = oscl_strdup(name);
			vsrc_camera_set_device(hdl, vsrc_manager.dev_name[i]);
			vsrc_manager.vsrc_hdl[i] = hdl;
			vsrc_manager.refcnt[i] = 1;
		} else
			OSCL_LOGE("opened camera > MAX_CAM_OPENED, check it!");
	}
	OSCL_LOGI("device %s hdl:%p!", name, hdl);
	pthread_mutex_unlock(&_manager_lock);
	return hdl;
}

int vsrc_manager_put_device(void *hdl)
{
	int i;
	int ret = -1;

	if (hdl == NULL)
		return 0;
	pthread_mutex_lock(&_manager_lock);
	for (i = 0; i < MAX_CAM_OPENED; i++) {
		if (vsrc_manager.vsrc_hdl[i] == hdl) {
			ret = 0;
			vsrc_manager.refcnt[i]--;
			if (vsrc_manager.refcnt[i] == 0) {
				ret = vsrc_camera_release(hdl);
				if (ret == 0) {
					vsrc_manager.vsrc_hdl[i] = NULL;
					oscl_free(vsrc_manager.dev_name[i]);
				} else {
					vsrc_manager.refcnt[i] = 1;
					OSCL_LOGE("close %s when dev is using",
						vsrc_manager.dev_name[i]);
				}
			}
			break;
		}
	}
	if (i == MAX_CAM_OPENED)
		OSCL_LOGE("removed hdl cannot find in camera manager!");
	pthread_mutex_unlock(&_manager_lock);
	return ret;
}


