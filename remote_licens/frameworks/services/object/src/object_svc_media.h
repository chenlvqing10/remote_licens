/*
 * object_svc_media.c
 *
 * Copyright (C) 2019, LomboTech Co.Ltd.
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

#ifndef _OBJECT_SVC_MEDIA_H_
#define _OBJECT_SVC_MEDIA_H_

#include "object_svc.h"

/*
* return as fast as possible.
* return 0 if deal with frame successfully, otherwise return -1;
*/
typedef int (*p_object_frame_cb)(void *p, object_frame_t *frame);

void *object_media_create(void);
int object_media_start(void *hdl);
int object_media_stop(void *hdl);
int object_media_destroy(void *hdl);
int object_media_set_cb(void *hdl, enum object_camera_id camera_id,
						void *p, p_object_frame_cb f_cb);
int object_media_return_frame(object_frame_t *f_frame);
int object_media_take_picture(void *hdl, enum object_camera_id camera_id,
						p_object_img_cb f_cb);
int object_media_get_camera_config(void *hdl, enum object_camera_id camera_id,
						camera_cfg_t *cfg);

#endif /* _OBJECT_SVC_MEDIA_H_ */
