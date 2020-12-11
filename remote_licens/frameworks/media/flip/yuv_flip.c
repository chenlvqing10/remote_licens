/*
 * yuv_flip.c
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

#define LOG_TAG		"FLIP"

#include <log/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "flip.h"

static int yuv420sp_flip(struct flip_input *flip)
{
	unsigned char *y, *uv;
	int i, j, w, h;
	unsigned char tmp;

	if (flip->addr[0] == NULL || flip->addr[1] == NULL) {
		ALOGE("parameter error, null pointer");
		return -1;
	}

	w = flip->width;
	h = flip->height;
	y = flip->addr[0];
	uv = flip->addr[1];
	for (i = 0; i < h; i++) {
		for (j = 0; j < w / 2; j++) {
			/* flip y */
			tmp = y[i * w + j];
			y[i * w + j] = y[i * w + w - 1 - j];
			y[i * w + w - 1 - j] = tmp;

			/* flip uv */
			if ((i & 0x01) == 0 && j < w / 4) {
				/* u or v */
				tmp = uv[i * w / 2 + j * 2];
				uv[i * w / 2 + j * 2] =
					uv[i * w / 2 + w - (j * 2) - 2];
				uv[i * w / 2 + w - (j * 2) - 2] = tmp;

				/* v or u */
				tmp = uv[i * w / 2 + j * 2 + 1];
				uv[i * w / 2 + j * 2 + 1] =
					uv[i * w / 2 + w - (j * 2) - 2 + 1];
				uv[i * w / 2 + w - (j * 2) - 2 + 1] = tmp;
			}
		}
	}

	return 0;
}

int yuv_flip(struct flip_input *flip)
{
	int ret = 0;

	/* Check parameters */
	if (flip == NULL) {
		ALOGE("parameter error, null pointer");
		return -1;
	}

	if (flip->width <= 0 && flip->height <= 0) {
		ALOGE("input parameter (%d, %d) error",
						flip->width, flip->height);
		return -1;
	}

	if ((flip->width & 0x01) || (flip->height & 0x01)) {
		ALOGE("width or height is odd, (%d, %d) error",
						flip->width, flip->height);
		return -1;
	}

	switch (flip->format) {
	case FLIP_COLOR_NV12:
		ret = yuv420sp_flip(flip);
		break;
	case FLIP_COLOR_NV21:
		ret = yuv420sp_flip(flip);
		break;
	case FLIP_COLOR_YV12:
	case FLIP_COLOR_YU12:
	default:
		ALOGE("input color format(%d) not support", flip->format);
		return -1;
	}

	return ret;
}

