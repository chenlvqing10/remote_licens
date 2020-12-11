/*
 * zbar_svc_main.h
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

#ifndef _ZBAR_SVC_MAIN_H_
#define _ZBAR_SVC_MAIN_H_

#include "queue.h"
#include "list.h"
#include "zbar_svc.h"

typedef struct _zbar_svc
{
	int is_running;
	zbar_image_scanner_t *scanner;

	head_t zbar_scan_queue;
	pthread_mutex_t zbar_scan_lock;
	pthread_cond_t zbar_scan_cond;
	pthread_t zbar_scan_thrd;
	f_zbar_scan_cb cb;
	long long time_out;
	int mode;
	void *arg;

} zbar_svc_t;

typedef struct _zbar_frame {
	unsigned char *addr[3];
	int size[3];
	int width;
	int height;

	long long det_begin;
	long long det_end;

	void *priv;
} zbar_frame_t;

#endif /*_ZBAR_SVC_MAIN_H_*/
