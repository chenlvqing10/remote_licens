/*
 * flip.h - Standard functionality for lombo
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

#ifndef __FLIP_H__
#define __FLIP_H__

enum flip_color_format {
	FLIP_COLOR_YV12,
	FLIP_COLOR_YU12,
	FLIP_COLOR_NV12,
	FLIP_COLOR_NV21,
};

struct flip_input {
	enum flip_color_format format;
	int width;
	int height;
	unsigned char *addr[3];
	int size[3];
};

int yuv_flip(struct flip_input *flip);

#endif

