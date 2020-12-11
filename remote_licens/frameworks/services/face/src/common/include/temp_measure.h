/*
 * temp_measure.h
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

#ifndef __TEMP_MEASURE_H__
#define __TEMP_MEASURE_H__

#include "queue.h"
#include "list.h"

typedef struct _temp_svc_s {
	/* face temp measure */
	int temp_measure;

	pthread_t temp_measure_thrd;
	pthread_mutex_t temp_measure_lock;
	pthread_cond_t temp_measure_cond;
	pthread_mutex_t temp_list_lock;

	list_head_t temp_measure_list;
} temp_svc_t;

typedef struct _temp_measure_s {
	list_head_t node;
	int id;
	int rect[4];
	float *temp_value;
	float value;
	int cnt;
} temp_measure_t;

int temp_measure_init(void);
int temp_measure_deinit(void);
int temp_measure_start(void);
int temp_measure_stop(void);
int temp_measure_reset(int id);
void temp_measure_update(nna_faces_t *rects);
temp_measure_t *temp_measure_get(int id);

#endif