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

enum bmp_format {
	BMP_COLOR_NV12,
	BMP_COLOR_MAX
};

enum bmp_scale {
	BMP_SCALE_OFF,
	BMP_SCALE_AUTO,
};


struct bmp_dec_input {
	char *file;
	enum bmp_format format;
	enum bmp_scale scale;
	int stride_width;
	int stride_height;
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

#pragma pack(2)
typedef struct _BITMAPFILEHEADER
{
	u_int16_t bf_type;
	u_int32_t bf_size;
	u_int16_t bf_reserved1;
	u_int16_t bf_reserved2;
	u_int32_t bf_off_bits;
} BITMAPFILEHEADER;

typedef struct _BITMAPINFOHEADER
{
	u_int32_t bi_size;
	u_int32_t bi_width;
	u_int32_t bi_height;
	u_int16_t bi_planes;
	u_int16_t bi_bit_count;
	u_int32_t bi_compression;
	u_int32_t bi_size_image;
	u_int32_t bi_x_pels_per_meter;
	u_int32_t bi_y_pels_per_meter;
	u_int32_t bi_clr_used;
	u_int32_t bi_clr_important;
} BITMAPINFOHEADER;
#pragma pack(0)

void bmp_dec_end(struct bmp_dec_output *output);
int bmp_dec_begin(struct bmp_dec_input *input,
		struct bmp_dec_output *output);

#endif /* __BMP_DEC_H__ */
