/*
 * yuv_cut.c
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

#define LOG_TAG		"cut"

#include <log/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cut.h"

static int yuv420sp_cut(struct cut_input *input, struct cut_output *output)
{
	int x, y, pix;
	unsigned char *inbuf, *outbuf, *inuvbuf, *outuvbuf;
	int size;

	if (input->addr[0] == NULL || input->addr[1] == NULL) {
		ALOGE("parameter error, null pointer");
		return -1;
	}

	size  = input->dest_width * input->dest_height * 3 / 2;
	outbuf = malloc(size);
	if (!outbuf) {
		ALOGE("malloc failed");
		return -1;
	}
	memset(outbuf, 0, size);

	inbuf = input->addr[0];
	inuvbuf = input->addr[1];
	outuvbuf = outbuf + input->dest_width * input->dest_height;
	for (x = 0; x < input->dest_width; x++) {
		for (y = 0; y < input->dest_height; y++) {
			/* Y vector */
			*(outbuf + y * input->dest_width + x) =
				*(inbuf + (x + input->dest_x) + input->src_width
							* (y + input->dest_y));
			/* UV vector */
			if ((x + input->dest_x) % 2 == 1 &&
				(y + input->dest_y) % 2 == 1) {
				pix = (input->src_width >> 0) *
				((input->dest_y + y) >> 1) + ((input->dest_x + x) >> 0);
				*(outuvbuf + (input->dest_width / 1) * (y / 2) +
					(x / 1)) = *(inuvbuf + pix);
				*(outuvbuf + (input->dest_width / 1) * (y / 2) +
					(x / 1) + 1) = *(inuvbuf + pix + 1);
			}
		}
	}

	output->addr[0] = outbuf;
	output->addr[1] = outuvbuf;
	output->addr[2] = NULL;
	output->size[0] = input->dest_width * input->dest_height;
	output->size[1] = input->dest_width * input->dest_height / 2;
	output->size[2] = 0;
	output->width = input->dest_width;
	output->height = input->dest_height;
	output->format = input->format;

	return 0;
}

static int yuv420p_cut(struct cut_input *input, struct cut_output *output)
{
	int x, y, pix;
	unsigned char *inbuf, *outbuf, *inuvbuf, *outubuf, *outvbuf;
	int size;

	if (input->addr[0] == NULL || input->addr[1] == NULL) {
		ALOGE("parameter error, null pointer");
		return -1;
	}

	size  = input->dest_width * input->dest_height * 3 / 2;
	outbuf = malloc(size);
	if (!outbuf) {
		ALOGE("malloc failed");
		return -1;
	}
	memset(outbuf, 0, size);

	inbuf = input->addr[0];
	inuvbuf = input->addr[1];
	outubuf = outbuf + input->dest_width * input->dest_height;
	outvbuf = outubuf + input->dest_width * input->dest_height / 4;
	for (x = 0; x < input->dest_width; x++) {
		for (y = 0; y < input->dest_height; y++) {
			/* Y vector */
			*(outbuf + y * input->dest_width + x) =
				*(inbuf + (x + input->dest_x) + input->src_width *
							(y + input->dest_y));
			if ((x + input->dest_x) % 2 == 1 &&
				(y + input->dest_y) % 2 == 1) {
				pix = (input->src_width / 2) *
				((input->dest_y + y) / 2) + ((input->dest_x + x) / 2);
				/* U vector */
				*(outubuf + (input->dest_width / 2) * (y / 2) + (x / 2))
						= *(inuvbuf + pix);
				/* Y vector */
				pix += input->src_width * input->src_height / 4;
				*(outvbuf + (input->dest_width / 2) * (y / 2) + (x / 2))
						= *(inuvbuf + pix);
			}
		}
	}

	output->addr[0] = outbuf;
	output->addr[1] = outubuf;
	output->addr[2] = outvbuf;
	output->size[0] = input->dest_width * input->dest_height;
	output->size[1] = input->dest_width * input->dest_height / 4;
	output->size[2] = input->dest_width * input->dest_height / 4;
	output->width = input->dest_width;
	output->height = input->dest_height;
	output->format = input->format;

	return 0;
}

int yuv_cut_start(struct cut_input *input, struct cut_output *output)
{
	int dest_width, dest_height, ret = 0;

	/* Check parameters */
	if (input == NULL || output == NULL) {
		ALOGE("parameter error, null pointer");
		return -1;
	}

	if ((input->dest_x < 0) || (input->dest_y < 0)) {
		ALOGE("input parameter dest(%d, %d) error", input->dest_x, input->dest_y);
		return -1;
	}

	if ((input->dest_width <= 0) || (input->dest_height <= 0)) {
		ALOGE("input parameter width(%d) height(%d) error", input->dest_width,
			input->dest_height);
		return -1;
	}

	/* Attention: dest x should be aligned with 2.
	* if x is odd, u and v will be reverse */
	if (input->dest_x & 0x1)
		input->dest_x += 0x1;
	if (input->dest_y & 0x1)
		input->dest_y -= 0x1;

#ifdef CUT_ALIGNED_4
	/* Destination width should be aligned with 4 */
	dest_width = input->dest_width;
	if (dest_width & 0x3) {
		dest_width += 0x3;
		dest_width &= ~0x3;
	}
#else
	/* Destination width should be aligned with 2 */
	dest_width = input->dest_width;
	if (dest_width & 0x1)
		dest_width -= 0x1;
#endif
	if (input->dest_x + dest_width > input->src_width) {
		ALOGE("destination width error, cross the boundary");
		return -1;
	}
#ifdef CUT_ALIGNED_4
	/* Destination height should be aligned with 4 */
	dest_height = input->dest_height;
	if (dest_height & 0x3) {
		dest_height += 0x3;
		dest_height &= ~0x3;
	}
#else
	/* Destination height should be aligned with 2 */
	dest_height = input->dest_height;
	if (dest_height & 0x1)
		dest_height -= 0x1;
#endif
	if (input->dest_y + dest_height > input->src_height) {
		ALOGE("destination height error, cross the boundary");
		return -1;
	}
	input->dest_width = dest_width;
	input->dest_height = dest_height;

	switch (input->format) {
	case CUT_COLOR_YV12:
		ret = yuv420p_cut(input, output);
		break;
	case CUT_COLOR_YU12:
		ret = yuv420p_cut(input, output);
		break;
	case CUT_COLOR_NV12:
		ret = yuv420sp_cut(input, output);
		break;
	case CUT_COLOR_NV21:
		ret = yuv420sp_cut(input, output);
		break;
	default:
		ALOGE("input color format(%d) not support", input->format);
		return -1;
	}

	return ret;
}

void yuv_cut_end(struct cut_output *output)
{
	if (output == NULL)
		return;

	if (output->addr[0] != NULL) {
		free(output->addr[0]);
		output->addr[0] = NULL;
		output->addr[1] = NULL;
		output->addr[2] = NULL;
	}
}

