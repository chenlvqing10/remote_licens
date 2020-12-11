/*
 * cut.h - Standard functionality for lombo
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

#ifndef __CUT_H__
#define __CUT_H__

enum cut_color_format {
	CUT_COLOR_YV12,
	CUT_COLOR_YU12,
	CUT_COLOR_NV12,
	CUT_COLOR_NV21,
};

struct cut_input {
	int src_width;
	int src_height;
	enum cut_color_format format;
	unsigned char *addr[3];
	int size[3];
	int dest_x;
	int dest_y;
	int dest_width;
	int dest_height;
};

struct cut_output {
	int width;
	int height;
	enum cut_color_format format;
	unsigned char *addr[3];
	int size[3];
};

int yuv_cut_start(struct cut_input *input, struct cut_output *output);
void yuv_cut_end(struct cut_output *output);


#endif

