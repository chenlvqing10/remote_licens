/*
 * rotation_sw.c - Rotation sw module driver code for LomboTech
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

#ifndef __ROTATION_SW_C__
#define __ROTATION_SW_C__
#include <stdio.h>
#include "rotation_neon.h"

typedef void (*func_image)(const unsigned char *src,
			   int src_line_stride,
			   unsigned char *dst,
			   int dst_line_stride,
			   int src_width, int src_height);
struct trops {
	func_image copy_image;
	func_image mirror_image;
	func_image transpose_image;
};

static void transpose_8bit_c(const unsigned char *src,
			     int src_line_stride,
			     unsigned char *dst,
			     int dst_line_stride,
			     int src_width, int src_height)
{
	int dst_row, dst_col;

	for (dst_row = 0; dst_row < src_width; dst_row++) {
		for (dst_col = 0; dst_col < src_height; dst_col++) {
			int src_row = dst_col;
			int src_col = dst_row;

			dst[dst_row * dst_line_stride + dst_col] =
				src[src_row * src_line_stride + src_col];
		}
	}
}

static void transpose_16bit_c(const unsigned char *src,
			     int src_line_stride,
			     unsigned char *dst,
			     int dst_line_stride,
			     int src_width, int src_height)
{
	int dst_row, dst_col;

	for (dst_row = 0; dst_row < src_width; dst_row++) {
		for (dst_col = 0; dst_col < src_height; dst_col++) {
			int src_row = dst_col;
			int src_col = dst_row;

			dst[dst_row * dst_line_stride + 2 * dst_col] =
			  src[src_row * src_line_stride + 2 * src_col];
			dst[dst_row * dst_line_stride + 2 * dst_col + 1] =
			  src[src_row * src_line_stride + 2 * src_col + 1];
		}
	}
}

static void transpose_32bit_c(const unsigned char *src,
			     int src_line_stride,
			     unsigned char *dst,
			     int dst_line_stride,
			     int src_width, int src_height)
{
	int dst_row, dst_col;

	for (dst_row = 0; dst_row < src_width; dst_row++) {
		for (dst_col = 0; dst_col < src_height; dst_col++) {
			int src_row = dst_col;
			int src_col = dst_row;

			dst[dst_row * dst_line_stride + 4 * dst_col] =
			  src[src_row * src_line_stride + 4 * src_col];
			dst[dst_row * dst_line_stride + 4 * dst_col + 1] =
			  src[src_row * src_line_stride + 4 * src_col + 1];
			dst[dst_row * dst_line_stride + 4 * dst_col + 2] =
			  src[src_row * src_line_stride + 4 * src_col + 2];
			dst[dst_row * dst_line_stride + 4 * dst_col + 3] =
			  src[src_row * src_line_stride + 4 * src_col + 3];
		}
	}
}

static void transpose_8bit(const unsigned char *src,
			     int src_line_stride,
			     unsigned char *dst,
			     int dst_line_stride,
			     int src_width, int src_height)
{
	int height = src_height;
	int bytes_per_pixel = 1;
	int rows = 8;  /* the neon core process 4rows each time */

	/* process 8*n rows */
	while (height >= rows) {
		transpose_8bit_8row_neon(src, src_line_stride,
					 dst, dst_line_stride, src_width);
		src += rows * src_line_stride;
		dst += rows * bytes_per_pixel;
		height -= rows;
	}

	/* process the rest rows */
	if (height > 0) {
		transpose_8bit_c(src, src_line_stride,
				 dst, dst_line_stride, src_width, height);
	}
}

static void transpose_16bit(const unsigned char *src,
			     int src_line_stride,
			     unsigned char *dst,
			     int dst_line_stride,
			     int src_width, int src_height)
{
	int height = src_height;
	int bytes_per_pixel = 2;
	int rows = 8;  /* the neon core process 8rows each time */

	/* process 8*n rows */
	while (height >= rows) {
		transpose_16bit_8row_neon(src, src_line_stride,
					  dst, dst_line_stride, src_width);
		src += rows * src_line_stride;
		dst += rows * bytes_per_pixel;
		height -= rows;
	}

	/* process the rest rows */
	if (height > 0) {
		transpose_16bit_c(src, src_line_stride,
				  dst, dst_line_stride, src_width, height);
	}
}

static void transpose_32bit(const unsigned char *src,
			     int src_line_stride,
			     unsigned char *dst,
			     int dst_line_stride,
			     int src_width, int src_height)
{
	int height = src_height;
	int bytes_per_pixel = 4;
	int rows = 4;  /* the neon core process 4rows each time */

	/* process 4*n rows */
	while (height >= rows) {
		transpose_32bit_4row_neon(src, src_line_stride,
					  dst, dst_line_stride, src_width);
		src += rows * src_line_stride;
		dst += rows * bytes_per_pixel;
		height -= rows;
	}

	/* process the rest rows */
	if (height > 0) {
		transpose_32bit_c(src, src_line_stride,
				  dst, dst_line_stride, src_width, height);
	}
}

static void mirror_8bit(const unsigned char *src,
			int src_line_stride,
			unsigned char *dst,
			int dst_line_stride,
			int width, int height)
{
	int row;

	for (row = 0; row < height; row++) {
		mirror_8bit_1row_neon(src, dst, width);
		src += src_line_stride;
		dst += dst_line_stride;
	}
}

static void mirror_16bit(const unsigned char *src,
			int src_line_stride,
			unsigned char *dst,
			int dst_line_stride,
			int width, int height)
{
	int row;

	for (row = 0; row < height; row++) {
		mirror_16bit_1row_neon(src, dst, width);
		src += src_line_stride;
		dst += dst_line_stride;
	}
}

static void mirror_32bit(const unsigned char *src,
			int src_line_stride,
			unsigned char *dst,
			int dst_line_stride,
			int width, int height)
{
	int row;

	for (row = 0; row < height; row++) {
		mirror_32bit_1row_neon(src, dst, width);
		src += src_line_stride;
		dst += dst_line_stride;
	}
}

static void copy_8bit(const unsigned char *src,
			int src_line_stride,
			unsigned char *dst,
			int dst_line_stride,
			int width, int height)
{
	int row;

	for (row = 0; row < height; row++) {
		copy_neon(src, dst, width);
		src += src_line_stride;
		dst += dst_line_stride;
	}
}

static void copy_16bit(const unsigned char *src,
			int src_line_stride,
			unsigned char *dst,
			int dst_line_stride,
			int width, int height)
{
	int row;

	for (row = 0; row < height; row++) {
		copy_neon(src, dst, width * 2);
		src += src_line_stride;
		dst += dst_line_stride;
	}
}

static void copy_32bit(const unsigned char *src,
			int src_line_stride,
			unsigned char *dst,
			int dst_line_stride,
			int width, int height)
{
	int row;

	for (row = 0; row < height; row++) {
		copy_neon(src, dst, width * 4);
		src += src_line_stride;
		dst += dst_line_stride;
	}
}

static int sw_transform_main(const unsigned char *src,
			     int src_line_stride,
			     unsigned char *dst,
			     int dst_line_stride,
			     int width, int height,
			     int mode, struct trops *ops)
{
	int ret = 0;

	switch (mode) {
	case 0:
		ops->copy_image(src, src_line_stride,
			  dst, dst_line_stride,
			  width, height);
		break;
	case 1:
		src += (height - 1) * src_line_stride;
		src_line_stride = -src_line_stride;
		ops->transpose_image(src, src_line_stride,
				  dst, dst_line_stride,
				  width, height);
		break;
	case 2:
		src += (height - 1) * src_line_stride;
		src_line_stride = -src_line_stride;
		ops->mirror_image(src, src_line_stride,
			  dst, dst_line_stride,
			  width, height);
		break;
	case 3:
		dst += (width - 1) * dst_line_stride;
		dst_line_stride = -dst_line_stride;
		ops->transpose_image(src, src_line_stride,
				  dst, dst_line_stride,
				  width, height);
		break;
	case 4:
		ops->mirror_image(src, src_line_stride,
			  dst, dst_line_stride,
			  width, height);
		break;
	case 5:
		src += (height - 1) * src_line_stride;
		src_line_stride = -src_line_stride;
		dst += (width - 1) * dst_line_stride;
		dst_line_stride = -dst_line_stride;
		ops->transpose_image(src, src_line_stride,
				  dst, dst_line_stride,
				  width, height);
		break;
	case 6:
		src += (height - 1) * src_line_stride;
		src_line_stride = -src_line_stride;
		ops->copy_image(src, src_line_stride,
			  dst, dst_line_stride,
			  width, height);
		break;
	case 7:
		ops->transpose_image(src, src_line_stride,
				  dst, dst_line_stride,
				  width, height);
		break;
	default:
		ret = __LINE__;
		break;
	}

	return ret;
}

static int sw_transform_core(const unsigned char *src,
			     int src_line_stride,
			     unsigned char *dst,
			     int dst_line_stride,
			     int width, int height,
			     int mode, int bytes_per_pixel)
{
	int ret = 0;
	struct trops ops;

	switch (bytes_per_pixel) {
	case 1:
		ops.copy_image = copy_8bit;
		ops.mirror_image = mirror_8bit;
		ops.transpose_image = transpose_8bit;
		ret = sw_transform_main(src, src_line_stride,
				  dst, dst_line_stride,
				  width, height, mode, &ops);
		break;
	case 2:
		ops.copy_image = copy_16bit;
		ops.mirror_image = mirror_16bit;
		ops.transpose_image = transpose_16bit;
		ret = sw_transform_main(src, src_line_stride,
				  dst, dst_line_stride,
				  width, height, mode, &ops);
		break;
	case 4:
		ops.copy_image = copy_32bit;
		ops.mirror_image = mirror_32bit;
		ops.transpose_image = transpose_32bit;
		ret = sw_transform_main(src, src_line_stride,
				  dst, dst_line_stride,
				  width, height, mode, &ops);
		break;
	default:
		ret = __LINE__;
		break;
	}

	return ret;
}

/**
 * sw_transform - transform image data
 * @src:             the buffer address of source image
 * @src_line_stride: the line stride of source image
 * @dst:             the buffer address of destination image
 * @dst_line_stride: the line stride of destination image
 * @src_width:       the horizontal resolution of source image
 * @src_height:      the vertical resolution of source image
 * @mode:            the transform mode
 * @bytes_per_pixel: the bytes of data for each pixel
 *
 * Return Zero when success, other for failture.
 */
int sw_transform(const unsigned char *src,
		int src_line_stride,
		unsigned char *dst,
		int dst_line_stride,
		int src_width, int src_height,
		int mode, int bytes_per_pixel)
{
	int ret = 0;

	if (bytes_per_pixel == 0) {
		ret = __LINE__;
		goto exit;
	}
	if (src == NULL) {
		ret = __LINE__;
		goto exit;
	}
	if (src_line_stride < bytes_per_pixel) {
		ret = __LINE__;
		goto exit;
	}
	if (dst == NULL) {
		ret = __LINE__;
		goto exit;
	}
	if (dst_line_stride < bytes_per_pixel) {
		ret = __LINE__;
		goto exit;
	}
	if (src_width <= 0) {
		ret = __LINE__;
		goto exit;
	}
	if (src_height <= 0) {
		ret = __LINE__;
		goto exit;
	}

	ret = sw_transform_core(src, src_line_stride,
		  dst, dst_line_stride,
		  src_width, src_height, mode, bytes_per_pixel);

exit:
	return ret;
}

#endif /* __ROTATION_SW_C__ */
