/*
 * private_test.c
 *
 * Copyright (C) 2016-2019, LomboTech Co.Ltd.
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

#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

#define LOG_TAG     "PRIVATE_TEST"
#include <log/log.h>

#include "private.h"

#define SN_OFFSET               0
#define SECRETNO_OFFSET         0x200
#define DEVICEID_OFFSET         0x400
#define QRCODE_OFFSET           0x600

#define RGBNIR_MAP_CHECK  0x5B
#define RGBNIR_MAP_OFFSET  0x1024
typedef struct map_params {
	int check;
	double x_map[5];
	double y_map[5];
} map_params_t;

int main(int argc, const char **argv)
{
	int ret = -1;
	ALOGD("private test");

	map_params_t w_map_params;
	memset(&w_map_params, 0, sizeof(map_params_t));
	w_map_params.x_map[0] = 0.687458;
	w_map_params.x_map[1] = -0.015446;
	w_map_params.x_map[2] = 19.762796;
	w_map_params.x_map[3] = -0.092930;
	w_map_params.x_map[4] = -0.097486;

	w_map_params.y_map[0] = -0.018391;
	w_map_params.y_map[1] = 0.711256;
	w_map_params.y_map[2] = -88.623155;
	w_map_params.y_map[3] = -0.015141;
	w_map_params.y_map[4] = -0.020780;

	ret = face_svc_set_private_data(RGBNIR_MAP_OFFSET,
			(char*)&w_map_params, sizeof(map_params_t));

	map_params_t r_map_params;
	memset(&r_map_params, 0, sizeof(map_params_t));
	ret = face_svc_get_private_data(RGBNIR_MAP_OFFSET,
			(char*)&r_map_params, sizeof(map_params_t));
	ALOGD("x_map:%lf %lf %lf %lf %lf", r_map_params.x_map[0],
			r_map_params.x_map[1],
			r_map_params.x_map[2],
			r_map_params.x_map[3],
			r_map_params.x_map[4]);
	ALOGD("y_map:%lf %lf %lf %lf %lf", r_map_params.y_map[0],
			r_map_params.y_map[1],
			r_map_params.y_map[2],
			r_map_params.y_map[3],
			r_map_params.y_map[4]);

	ALOGD("system test end");

	return ret;
}
