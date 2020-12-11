/*
 * bmp_dec.h - Public interface for personnel management
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

#ifndef __BMP_DEC_H__
#define __BMP_DEC_H__

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

enum bmp_format {
	BMP_COLOR_NV12,
	BMP_COLOR_MAX
};

enum bmp_scale {
	BMP_SCALE_OFF,
	BMP_SCALE_AUTO,
};


struct bmp_dec_input {
	char *file; /* file path */
	char *data; /* memery buffer */
	int len; /* memery buffer len */
	enum bmp_format format;
	enum bmp_scale scale;
	int stride_width;
	int stride_height;
};

struct bmp_dec_info {
	int width;
	int height;
};

struct bmp_dec_output {
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

LOCAL void bmp_dec_end(struct bmp_dec_output *output);
LOCAL int bmp_dec_data(struct bmp_dec_input *input,
	struct bmp_dec_output *output);
LOCAL int bmp_dec_begin(struct bmp_dec_input *input,
	struct bmp_dec_output *output);
LOCAL int bmp_dec_info_read(struct bmp_dec_input *input,
	struct bmp_dec_info *dec_info);
LOCAL int bmp_dec_info_read_data(struct bmp_dec_input *input,
	struct bmp_dec_info *dec_info);

#endif /* __BMP_DEC_H__ */
