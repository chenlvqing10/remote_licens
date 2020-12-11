/*
 * vsrc_camera.h - Standard functionality for video source.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __VSRC_CAMERA_H_
#define __VSRC_CAMERA_H_

void *vsrc_camera_creat(void);
int vsrc_camera_release(void *camera);
int vsrc_camera_prepare(void *camera);
int vsrc_camera_set_para(void *camera, vsrc_param_t *para);
int vsrc_camera_set_device(void *camera, char *src_name);
int vsrc_camera_set_isp_inf_device(void *handle, char *src_name);
port_info_t *vsrc_camera_getport(void *camera, vdisp_rotate_mode_e mode);
int vsrc_camera_putport(void *camera, port_info_t *port);
int vsrc_camera_getpara_portinfo(void *camera,
	port_info_t *port, OMX_PARAM_PORTDEFINITIONTYPE *portinfo);
int vsrc_camera_enable_port(void *camera, port_info_t *port);
int vsrc_camera_disable_port(void *camera, port_info_t *port);
signed long long vsrc_camera_get_time(void *camera);
int vsrc_camera_get_para(void *camera, vsrc_param_t *para);
int vsrc_camera_reset(void *handle);
int vsrc_camera_set_port_cb(void *handle, app_port_filter_t *cb);
int vsrc_camera_pause(void *handle);
int vsrc_camera_resume(void *handle);
int vsrc_camera_set_res(void *handle, vstream_buf_cfg_t *res);

#endif

