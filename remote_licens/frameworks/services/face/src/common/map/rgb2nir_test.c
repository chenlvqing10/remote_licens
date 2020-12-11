/*
 * rgb2nir.cpp - code for rotate component.
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

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "rgb2nir.h"

void create_map_pararm(map_t *map)
{
	FILE *ini = NULL;

	ini = fopen("map_params", "wb");
	if (!ini) {
		printf("fopen map_params fail\n");
		return;
	}

	fprintf(ini,
		"[map]\n"
		"x_map_0=%lf\n"
		"x_map_1=%lf\n"
		"x_map_2=%lf\n"
		"x_map_3=%lf\n"
		"x_map_4=%lf\n"
		"y_map_0=%lf\n"
		"y_map_1=%lf\n"
		"y_map_2=%lf\n"
		"y_map_3=%lf\n"
		"y_map_4=%lf\n",
		map->x_map[0],
		map->x_map[1],
		map->x_map[2],
		map->x_map[3],
		map->x_map[4],
		map->y_map[0],
		map->y_map[1],
		map->y_map[2],
		map->y_map[3],
		map->y_map[4]
	);

	fclose(ini);
	return;
}

int main(int agrc, char **argv)
{
#if 0
	map_t map;
	map_t ord_map;
	map_t mod_map;
	memset(&map, 0, sizeof(map_t));
	memset(&ord_map, 0, sizeof(map_t));
	memset(&mod_map, 0, sizeof(map_t));

	#if 1 /*p2*/
	ord_map.x_map[0] = 0.662656;
	ord_map.x_map[1] = 0.005128;
	ord_map.x_map[2] = 53.256470;
	ord_map.x_map[3] = -0.039214;
	ord_map.x_map[4] = -0.034362;

	ord_map.y_map[0] = -0.003749;
	ord_map.y_map[1] = 0.668271;
	ord_map.y_map[2] = 34.203654;
	ord_map.y_map[3] = -0.004798;
	ord_map.y_map[4] = -0.002971;
	#endif

	#if 0 /*p1*/
	ord_map.x_map[0] = 0.687458;
	ord_map.x_map[1] = -0.015446;
	ord_map.x_map[2] = 19.762796;
	ord_map.x_map[3] = -0.092930;
	ord_map.x_map[4] = -0.097486;

	ord_map.y_map[0] = -0.018391;
	ord_map.y_map[1] = 0.711256;
	ord_map.y_map[2] = -88.623155;
	ord_map.y_map[3] = -0.015141;
	ord_map.y_map[4] = -0.020780;
	#endif

	get_mod_ord_map_t get_mod_ord_map;
	memset(&get_mod_ord_map, 0, sizeof(get_mod_ord_map_t));
	get_mod_ord_map.ord_map = &ord_map;
	get_mod_ord_map.mod_path = "./mod_img";
	get_mod_ord_map.mod_width = 6;
	get_mod_ord_map.mod_height = 9;
	get_mod_ord_map.mod_grid_face_scale = 5.8;
	get_mod_ord_map.lr = 0.005;
	get_mod_ord_map.lr_cnt_thr = 100;
	get_mod_ord_map.flip = 0;
	get_mod_ord_map.mod_map = &mod_map;
	get_modify_ord_map_params(&get_mod_ord_map);
	create_map_pararm(&mod_map);
#else
	map_t ord_map;
	memset(&ord_map, 0, sizeof(map_t));
	get_ord_map_t get_ord_map;
	memset(&get_ord_map, 0, sizeof(get_ord_map_t));
	get_ord_map.ord_map = &ord_map;
	get_ord_map.ord_path = "/data/ord_img";
	get_ord_map.ord_width = 6;
	get_ord_map.ord_hight = 9;
	get_ord_map.ord_grid_face_scale = 5.8;
	get_ord_map.flip = 0;
	get_ord_map_params(&get_ord_map);
	create_map_pararm(&ord_map);
#endif
	return 0;
}
