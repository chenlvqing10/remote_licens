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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <log/log.h>

#include "cut.h"

#define CUT_NV12_FILE		"/mnt/sdcard/test.yuv"
#define CUT_SAVE_FILE		"/mnt/sdcard/test_cut.yuv"
#define CUT_IMG_WIDTH		800
#define CUT_IMG_HEIGHT		1408
#define CUT_DEST_X		321
#define CUT_DEST_Y		180
#define CUT_DEST_WIDTH		362
#define CUT_DEST_HEIGHT		362

int main(int argc, char *argv[])
{
	struct cut_input input;
	struct cut_output output;
	FILE *flip = NULL, *out_flip = NULL;
	unsigned char *buf = NULL;
	int buf_size, count, ret;

	flip = fopen(CUT_NV12_FILE, "rb");
	if (flip == NULL) {
		ALOGE("nv12 file not found:%s", CUT_NV12_FILE);
		goto exit;
	}

	buf_size = CUT_IMG_WIDTH * CUT_IMG_HEIGHT * 3 / 2;
	buf = (unsigned char *)malloc(buf_size);
	if (!buf) {
		ALOGE("malloc unca failed");
		goto exit;
	}

	count = fread(buf, 1, buf_size, flip);
	if (count != buf_size) {
		ALOGE("read failed, count(%d) != buf_size(%d)",
					count, buf_size);
		goto exit;
	}

	input.addr[0] = buf;
	input.addr[1] = buf + CUT_IMG_WIDTH * CUT_IMG_HEIGHT / 2;
	input.addr[2] = NULL;
	input.size[0] = CUT_IMG_WIDTH * CUT_IMG_HEIGHT;
	input.size[1] = CUT_IMG_WIDTH * CUT_IMG_HEIGHT / 2;
	input.size[2] = 0;
	input.src_width = CUT_IMG_WIDTH;
	input.src_height = CUT_IMG_HEIGHT;
	input.format = CUT_COLOR_NV12;
	input.dest_x = CUT_DEST_X;
	input.dest_y = CUT_DEST_Y;
	input.dest_width = CUT_DEST_WIDTH;
	input.dest_height = CUT_DEST_HEIGHT;
	memset(&output, 0, sizeof(struct cut_output));
	ret = yuv_cut_start(&input, &output);
	if (ret) {
		ALOGE("yuv cut failed");
		goto exit;
	}

	out_flip = fopen(CUT_SAVE_FILE, "wb+");
	if (out_flip == NULL) {
		ALOGE("nv12 file not found:%s", CUT_NV12_FILE);
		goto exit;
	}

	ALOGD("output buf:%p size:%d", output.addr[0], output.size[0]);
	count = fwrite(output.addr[0], 1, output.size[0], out_flip);
	if (count != output.size[0]) {
		ALOGE("read failed, count(%d) != buf_size(%d)",
					count, output.size[0]);
		goto exit;
	}
	count = fwrite(output.addr[1], 1, output.size[1], out_flip);
	if (count != output.size[1]) {
		ALOGE("read failed, count(%d) != buf_size(%d)",
					count, output.size[1]);
		goto exit;
	}
	yuv_cut_end(&output);

	ALOGD("cut test OK\n");

exit:
	if (flip)
		fclose(flip);
	if (out_flip)
		fclose(out_flip);
	if (buf)
		free(buf);

	return 0;
}
