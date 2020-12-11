/*
 * png_dec.h - Public interface for personnel management
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

#ifndef __PNG_DEC_H__
#define __PNG_DEC_H__


enum png_format {
	PNG_COLOR_NV12,
	PNG_COLOR_MAX
};

enum png_scale {
	PNG_SCALE_0FF,
	PNG_SCALE_AUTO,
};

struct png_dec_input {
	char *file;
	enum png_format format;
	enum png_scale scale;
	int stride_width;
	int stride_height;
};

struct png_dec_output {
	int width; /* img width px */
	int height; /* img height px */
	int stride_width;
	int stride_height;
	unsigned char *virt_addr[3];
	unsigned long phy_addr[3];
	int size[3];
	long long begin;
	long long end;
};

void png_dec_end(struct png_dec_output *output);
int png_dec_begin(struct png_dec_input *input,
		struct png_dec_output *output);


#endif /* __PNG_DEC_H__ */
