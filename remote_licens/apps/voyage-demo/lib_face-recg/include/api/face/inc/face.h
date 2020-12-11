/*
 * face.h - export function face detect
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

#ifndef __FACE_H__
#define __FACE_H__

#include "ee_list.h"
#include "face_svc.h"

#define PERSON_NAME_MAX		32
#define PERSON_EMPLOYEE_NUM_MAX	32

typedef struct face {
	/* private */
	struct list_head node;

	/* same id for the same person */
	int id;

	/* face position [left, top, right, bottom] */
	int rect[4];

	/* the face position after track and smooth */
	int rect_smooth[4];

	int nir_rect[4];

	/* face quality */
	int quality;

	/* face feature point */
	float kpts[10];

	/* head gesture:head_pose[0]: yaw angle
				head_pose[1]: pitch angle
				head_pose[2]: roll angle
				*/
	float head_pose[3];

	face_attr_t attr;

	/* score of feature comparision */
	float score;

	int max_id;

	int sound_play;

	/* living face */
	int living;

	char name[PERSON_NAME_MAX];

	/* debug info */
	long long det_ts;
	long long recg_ts;
	long long cmp_ts;
	long long living_ts;

	/* for calculate the time of recg and live check */
	long long det_ts_begin;
	long long recg_ts_end;
	long long living_ts_end;
} face_t;

typedef struct faces {
	int face_num;
	face_t *faces;
} faces_t;

typedef void (*face_cb)(faces_t *);

int face_init(void);
void face_deinit(void);
int face_start(void);
int face_stop(void);
int face_pause(void);
int face_resume(void);
int face_pause_status(void);
int face_set_cb(face_cb cb);
int face_db_update(void);
int face_db_get_count(void);
int face_db_get_name(int index, char *name, int name_size);
int face_db_get_employee_num(int index, char *num, int num_size);
void *get_face_handle(void);
faces_t *faces_get(void);
void faces_put(faces_t *faces);
float face_recg_threshold(void);
int face_debug_switch(void);
int face_recg_time_debug(void);
int face_recg_timeout(void);
void *face_capture_start(float yaw, float pitch, float roll);
void face_capture_stop(void);
int face_live_detect(void);
void face_clear_face_list();
void face_clear_face_list_by_name(const char *name);
int face_take_mod_pic(int x, int y, int w, int h);
int face_cal_mod_map(map_params_t *map);
int face_reset_recg(void);

#endif /* __FACE_H__ */

