/*
 * rotation_sw.h - Rotation sw module head file
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

#ifndef _ROT_COPY_H_
#define _ROT_COPY_H_

#include "disp_fourcc.h"

#define DISP_F				1.0

/*
 * mode:
 * 0: copy from the src to the dst
 * 1: rotate the src imgae by 90 degree
 * 2: rotate the src image by 180 degree
 * 3: rotate the src image by 270 degree
 * 4: horizontal flip the src image
 * 5: horizontal flip and then rotate the src image by 90 degree
 * 6: vertical flip the src image
 * 7: transpose the src image
 */
typedef enum tag_rot_mode_type {
	LOMBO_TRANSFORM_COPY,
	LOMBO_TRANSFORM_ROT_90,
	LOMBO_TRANSFORM_ROT_180,
	LOMBO_TRANSFORM_ROT_270,
	LOMBO_TRANSFORM_FLIP_H,
	LOMBO_TRANSFORM_FLIP_H_ROT_90,
	LOMBO_TRANSFORM_FLIP_V,
	LOMBO_TRANSFORM_FLIP_V_ROT_90,
	LOMBO_TRANSFORM_MAX,
} rot_mode_type_t;

typedef enum tag_rot_way {
	HW_ROT,
	SW_ROT,
} rot_way_t;

/*
 * Framebuffer information
 */
typedef struct tag_disp_rot_fb {
	unsigned int format;		/* Pixel format */
	unsigned int addr[3];		/* Buffer addr */
	unsigned int width[3];		/* Image width */
	unsigned int height[3];		/* Image height */
	unsigned int linestride_bit[3];	/* Line stride of buffer */
} disp_rot_fb_t;

typedef struct tag_disp_rot_cfg {
	rot_mode_type_t mode;
	disp_rot_fb_t infb;
	disp_rot_fb_t outfb;
	rot_way_t rot_way; /* use sw rot when true; otherwise use hw rot */
	/*
	* how many ms for hw rot time out, only valid when  rot_way is HW_ROT
	* default value is 200 ms
	* when time_out is less than 200 ms use 200ms
	* when time_out is more than 2000ms use 2000ms
	*/
	unsigned int time_out;
} disp_rot_cfg_t;

int rot_sw_process(disp_rot_cfg_t *cfgs);

#endif

