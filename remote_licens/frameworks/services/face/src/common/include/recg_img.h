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

#ifndef __RECG_IMG_H__
#define __RECG_IMG_H__

#include "jpeg_dec.h"
#include "bmp_dec.h"
#include "png_dec.h"

#ifndef FACE_EXPORT
#define FACE_EXPORT
#if __GNUC__ >= 4 // it means the compiler is GCC version 4.0 or later
	#ifdef FACE_EXPORT
		#define PUBLIC __attribute__((visibility ("default")))
		#define LOCAL __attribute__((visibility("hidden")))
	#else
		#define PUBLIC
		#define LOCAL
	#endif
#else
	#error "##### requires gcc version >= 4.0 #####"
#endif
#endif

typedef enum {
	type_jpeg = 0,
	type_bmp,
	type_png,
	type_unknow,
} e_recg_img_type;

struct recg_img_intput {
	char *file; /* file path */
	char *data; /* memery buffer */
	int len; /* memery buffer len */
	int stride_width;
	int stride_height;
};

struct jpg_dec_info {
	int width;
	int height;
};

struct recg_img_dec_info {
	union {
		struct jpg_dec_info out_jpg;
		struct bmp_dec_info out_bmp;
		struct png_dec_info out_png;
	};
	int width;
	int height;
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
LOCAL int recg_img_dec_begin(struct recg_img_intput *input,
		struct recg_img_output *output);
LOCAL int recg_img_dec_data(struct recg_img_intput *input,
		struct recg_img_output *output);
LOCAL void recg_img_dec_end(struct recg_img_output *output);
LOCAL int recg_img_dec_info(struct recg_img_intput *input,
		struct recg_img_dec_info *info);
LOCAL int recg_img_data_dec_info(struct recg_img_intput *input,
		struct recg_img_dec_info *info);
LOCAL void recg_img_output_adjust_end(struct recg_img_output *output);

#endif /* __RECG_IMG_H__ */
