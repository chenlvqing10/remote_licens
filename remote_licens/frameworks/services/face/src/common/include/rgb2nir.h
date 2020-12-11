/*
 * rgb2nir.h - code for rotate component.
 *
 * Copyright (C) 2019-2020, LomboTech Co.Ltd.
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

#ifndef __RGB2NIR_H__
#define __RGB2NIR_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct _map {
  double x_map[5];
  double y_map[5];
} map_t;

typedef struct _get_mod_ord_map {
	map_t *ord_map;
	char *mod_path;
	double mod_grid_face_scale;
	int mod_width;
	int mod_height;
	double lr;
	int lr_cnt_thr;
	int flip;
	map_t *mod_map;
} get_mod_ord_map_t;

typedef struct _get_ord_map {
	char *ord_path;
	double ord_grid_face_scale;
	int ord_width;
	int ord_hight;
	int flip;
	map_t *ord_map;
} get_ord_map_t;

typedef struct _get_map {
	char *ord_path;
	double ord_grid_face_scale;
	int ord_width;
	int ord_hight;
	char *mod_path;
	double mod_grid_face_scale;
	int mod_width;
	int mod_height;
	double lr;
	int lr_cnt_thr;
	int flip;
	map_t *mod_map;
} get_map_t;

int get_modify_ord_map_params(get_mod_ord_map_t *get_mod_ord_map);

int get_ord_map_params(get_ord_map_t *get_ord_map);

int get_map_params(get_map_t *get_map);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

