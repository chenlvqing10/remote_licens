/*
 * zbar_svc.h
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

#ifndef __ZBAR_SVC_H__
#define __ZBAR_SVC_H__

#include "queue.h"
#include "list.h"

typedef int(*f_zbar_scan_cb)(void *arg, char *data);

void *zbar_svc_init(void);
void zbar_svc_deinit(void);
int zbar_register_det_cb(int mode, f_zbar_scan_cb cb,  void *arg);
void zbar_det_frame(face_frame_t *f_frame);
void zbar_async_det_frame(face_frame_t *f_frame);

#endif /* __ZBAR_SVC_H__ */
