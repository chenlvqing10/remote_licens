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
	char *data;
	int len;
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

struct png_dec_info {
	int width;
	int height;
};

LOCAL int png_dec_begin(struct png_dec_input *input,
	struct png_dec_output *output);
LOCAL int png_dec_data(struct png_dec_input *input,
	struct png_dec_output *output);
LOCAL void png_dec_end(struct png_dec_output *output);
LOCAL int png_dec_info_read(struct png_dec_input *input,
	struct png_dec_info *dec_info);
LOCAL int png_dec_info_read_data(struct png_dec_input *input,
	struct png_dec_info *dec_info);

#endif /* __PNG_DEC_H__ */
