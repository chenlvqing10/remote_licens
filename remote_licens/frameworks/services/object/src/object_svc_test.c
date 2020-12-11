/*
 * object_svc_test.c
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
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "jpeg/jpeg_dec.h"

#include "vgg_api.h"
#define LOG_TAG "object_svc_test"
#include <log/log.h>

#ifndef MIN
#define MIN(a, b)            (((a) < (b)) ? (a) : (b))
#endif

#define RECG_WIDTH 3840
#define RECG_HEIGHT 2160

#define TEST_NUMBERS 1000

char label[1000][128];

void load_label(const char *file_path)
{
	FILE *fin = fopen(file_path, "r");
	char *buf = NULL;
	char *substr = NULL;

	if (fin == NULL) {
		ALOGE("open %s failed\n", file_path);
		return;
	}

	buf = &label[0][0];

	while (!feof(fin)) {
		fgets(buf, 128, fin);
		substr = strchr(buf, ',');
		if (substr)
			buf[substr - buf] = '\0';

		buf[32] = '\0';
		buf += 128;
	}
	fclose(fin);
}


int main(int argc, char *argv[])
{
	int count, ret;
	vgg_dt_in_t dt_in;
	int result[4] = {0};
	FILE *f_result = NULL;

	load_label("/usr/share/ax/object_labels_cn.txt");

	f_result = fopen("/mnt/usb/object_result", "wb");
	if (f_result == NULL)
		return -1;

	vgg_fn_t *vgg_fn = vgg_open(RECG_WIDTH, RECG_HEIGHT);

	for (count = 0; count < TEST_NUMBERS; count++) {
		struct jpeg_dec_input input;
		struct jpeg_dec_output output;
		char path[128];

		sprintf(path, "/mnt/usb/object/objrecg-%d.jpg", count);

		input.file = path;
		input.format = JPEG_COLOR_NV12;
		input.scale = JPEG_SCALEDOWN_0;
		input.width_stride = RECG_WIDTH;
		input.height_stride = RECG_HEIGHT;
		ret = jpeg_dec_begin(&input, &output);
		if (ret) {
			ALOGE("jpeg dec failed");
			continue;
		}

		dt_in.target_sz.width = output.stride_width;
		dt_in.target_sz.height = output.stride_height;
		dt_in.yuv.y = output.phy_addr;
		dt_in.yuv.uv = output.phy_addr
				+ output.stride_width * output.stride_height;

		dt_in.dt_roi.x = 0;
		dt_in.dt_roi.y = 0;
		dt_in.dt_roi.width = MIN(output.actual_width, output.stride_width);
		dt_in.dt_roi.height = MIN(output.actual_height, output.stride_height);

		vgg_fn->vgg_recv_buffer(vgg_fn, &dt_in, output.addr[0], result);

		for (int i = 0; i < 1; i++) {
			ALOGD("%s %d %s--%d path is %s", __func__, __LINE__,
					&label[result[i]][0], result[i], path);

			fwrite(path, 1, strlen(path), f_result);
			fwrite(&label[result[i]][0], 1, strlen(&label[result[i]][0]),
					f_result);

		}
		fwrite("\n", 1, 1, f_result);

		jpeg_dec_end(&output);
		usleep(20 * 1000);
	}

	vgg_close(vgg_fn);
	fclose(f_result);

	return 0;
}
