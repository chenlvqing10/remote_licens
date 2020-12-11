/*
 * jpeg_dec_test.c
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <stdlib.h>

#define LOG_TAG "JPEG_DEC"
#include "log/log.h"

#include "jpeg_dec.h"

#define JPEG_FILE		"/mnt/sdcard/test.jpg"
#define YUV_SAVE_PATH		"/mnt/sdcard/test.yuv"

int main(int argc, char *argv[])
{
	struct jpeg_dec_input input;
	struct jpeg_dec_output output;
	FILE *flip = NULL;
	int count, ret, size;

	input.file = JPEG_FILE;
	input.format = JPEG_COLOR_NV12;
	input.scale = JPEG_SCALEDOWN_0;
	input.width_stride = 0;
	input.height_stride = 0;
	ret = jpeg_dec_begin(&input, &output);
	if (ret) {
		ALOGE("jpeg dec failed");
		return -1;
	}

	flip = fopen(YUV_SAVE_PATH, "wb");
	if (flip == NULL) {
		ALOGE("open file:%s failed\n", JPEG_FILE);
		return -1;
	}
	size = output.size[0] + output.size[1] + output.size[2];
	count = fwrite(output.addr[0], 1, size, flip);
	if (count != size) {
		ALOGE("write failed, count(%d) != output.size(%d)\n",
					count, size);
		jpeg_dec_end(&output);
		fclose(flip);
		return -1;
	}
	jpeg_dec_end(&output);
	fclose(flip);
	ALOGD("jpeg dec test OK\n");

	return 0;
}

