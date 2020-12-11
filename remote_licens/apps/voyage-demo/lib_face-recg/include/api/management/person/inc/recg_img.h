/*
 * recg_img.h - Public interface for personnel management
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

#ifndef __RECG_IMG_H___
#define __RECG_IMG_H___

#include "jpeg_dec.h"
#include "bmp_dec.h"
#include "png_dec.h"

typedef enum {
	type_jpeg = 0,
	type_bmp,
	type_png,
	type_unknow,
} e_recg_img_type;

struct recg_img_intput {
	char *file;
	int stride_width;
	int stride_height;
};

struct recg_img_output {
	e_recg_img_type type; /* image type */
	union {
		struct jpeg_dec_output out_jpeg;
		struct bmp_dec_output out_bmp;
		struct png_dec_output out_png;
	};
	int stride_width;
	int stride_height;
	unsigned char *virt_addr[3];	/* virtual address of yuv buffer */
	unsigned long phy_addr[3];	/* physical address of yuv buffer */
	int size[3];

};

int recg_img_size_check(struct recg_img_intput *input,
		struct recg_img_output *output);

int recg_img_dec_begin(struct recg_img_intput *input,
		struct recg_img_output *output);
void recg_img_dec_end(struct recg_img_output *output);

#endif /* __RECG_IMG_H__ */
