/*
 * jpeg_enc_test_nv12.c
 *
 * Copyright (C) 2016-2019, LomboTech Co.Ltd.
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

#define LOG_TAG "JPEG_ENC"
#include "log/log.h"

#include "jpeg_enc.h"

#define NV12_FILE		"/mnt/usb/test.yuv420sp"
#define NV12_FILE_SAVE_PATH	"/mnt/usb/test.jpg"
#define IMG_WIDTH	1920
#define IMG_HEIGHT	1088

int main(int argc, char *argv[])
{
	struct input_image_info input;
	struct output_image_info output;
	FILE *flip = NULL;
	char *input_buffer = NULL;
	int buf_size, count, ret;

	flip = fopen(NV12_FILE, "rb");
	if (flip == NULL) {
		ALOGE("nv12 file not found:%s\n", NV12_FILE);
		goto exit;
	}

	buf_size = IMG_WIDTH * IMG_HEIGHT * 3 / 2;
	input_buffer = (char *)malloc(buf_size);
	if (!input_buffer) {
		ALOGE("malloc unca failed\n");
		goto exit;
	}

	count = fread(input_buffer, 1, buf_size, flip);
	if (count != buf_size) {
		ALOGE("read failed, count(%d) != buf_size(%d)\n",
					count, buf_size);
		goto exit;
	}

	input.size[0] = IMG_WIDTH * IMG_HEIGHT;
	input.size[1] = IMG_WIDTH * IMG_HEIGHT/2;
	input.addr[0] = (unsigned char *)input_buffer;
	input.addr[1] = input.addr[0] + input.size[0];
	input.width = IMG_WIDTH;
	input.height = IMG_HEIGHT;
	input.format = IMG_COLOR_NV12;
	input.qualify = 70;
	input.file = NV12_FILE_SAVE_PATH;
	ret = jpeg_enc_file(&input, &output);
	if (ret) {
		ALOGE("jpeg enc file failed\n");
		goto exit;
	}

	ALOGD("jpeg enc test OK\n");

exit:
	if (flip)
		fclose(flip);
	if (input_buffer)
		free(input_buffer);

	return 0;
}
