/*
 * yuv_flip_test.c
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
#include <sys/time.h>

#define LOG_TAG		"FLIP"
#include <log/log.h>

#include "flip.h"

#define FLIP_NV12_FILE		"/data/yuv420sp.yuv"
#define FLIP_SAVE_FILE		"/data/flip.yuv"
#define FLIP_WIDTH		800
#define FLIP_HEIGHT		1024

static long long time_get_us(void)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);

	return (long long)tv.tv_sec * 1000000 + tv.tv_usec;
}

int main(int argc, char *argv[])
{
	struct flip_input input;
	FILE *flip = NULL, *out_flip = NULL;
	unsigned char *buf = NULL;
	int buf_size, count, ret;
	long long time_start, time_end;

	flip = fopen(FLIP_NV12_FILE, "rb");
	if (flip == NULL) {
		ALOGE("nv12 file not found:%s", FLIP_NV12_FILE);
		goto exit;
	}

	buf_size = FLIP_WIDTH * FLIP_HEIGHT * 3 / 2;
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

	time_start = time_get_us();

	input.addr[0] = buf;
	input.addr[1] = buf + FLIP_WIDTH * FLIP_HEIGHT / 2;
	input.addr[2] = NULL;
	input.size[0] = FLIP_WIDTH * FLIP_HEIGHT;
	input.size[1] = FLIP_WIDTH * FLIP_HEIGHT / 2;
	input.size[2] = 0;
	input.width = FLIP_WIDTH;
	input.height = FLIP_HEIGHT;
	input.format = FLIP_COLOR_NV12;
	ret = yuv_flip(&input);
	if (ret) {
		ALOGE("yuv cut failed");
		goto exit;
	}

	time_end = time_get_us();
	ALOGD("w:%d, h:%d, time consume:%lld", FLIP_WIDTH, FLIP_HEIGHT,
					time_end - time_start);

	out_flip = fopen(FLIP_SAVE_FILE, "wb+");
	if (out_flip == NULL) {
		ALOGE("nv12 file not found:%s", FLIP_SAVE_FILE);
		goto exit;
	}

	ALOGD("output buf:%p size:%d", buf, buf_size);
	count = fwrite(buf, 1, buf_size, out_flip);
	if (count != buf_size) {
		ALOGE("write failed, count(%d) != buf_size(%d)",
					count, buf_size);
		goto exit;
	}

	ALOGD("flip test OK\n");

exit:
	if (flip)
		fclose(flip);
	if (out_flip)
		fclose(out_flip);
	if (buf)
		free(buf);

	return 0;
}
