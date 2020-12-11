/*
 * recg_img.c - Public interface for personnel management
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
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include "recg_img.h"

int main(int argc, char *argv[])
{
	char img[128] = {0};

	if(argc != 2) {
		printf("check input param\n");
		return -1;
	}

	int ret = 0;
	int count = 0;
	FILE *file = NULL;
	FILE * fp = NULL;
	int len = 0;
	void *data = NULL;
	struct recg_img_intput input;
	struct recg_img_output output;
	struct recg_img_dec_info dec_info;

	memset(&input, 0, sizeof(input));
	memset(&output, 0, sizeof(output));
	memset(&dec_info, 0, sizeof(dec_info));

	snprintf(img, 128 - 10, "%s", argv[1]);

#if 1

	fp = fopen((const char *)img, "rb");
	if (fp == NULL) {
		printf("open %s failed", img);
		return -1;
	}

	/* get file length */
	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	if (len <= 0) {
		printf("get file length error");
		goto exit;
	}
	fseek(fp, 0, SEEK_SET);

	data = (unsigned char *)malloc(len);
	if (data == NULL) {
		printf("alloc mem for file failed");
		goto exit;
	}

	count = fread(data, 1, len, fp);
	if (count != len) {
		printf("read file failed, count:%d, len:%d", count, len);
		goto exit;
	}

	input.file = img;
	input.data = data;
	input.len = len;
	printf("%s\n", input.file);
	input.stride_width = 800;
	input.stride_height = 1024;

	recg_img_data_dec_info(&input, &dec_info);

	ret = recg_img_dec_data(&input, &output);
	if (ret) {
		printf("recg_img_dec_begin:%s failed\n", input.file);
		goto exit;
	}

#else

	input.file = img;
	printf("%s\n", input.file);
	input.stride_width = 800;
	input.stride_height = 1024;

	recg_img_dec_info(&input, &dec_info);

	ret = recg_img_dec_begin(&input, &output);
	if (ret) {
		printf("recg_img_dec_begin:%s failed\n", input.file);
		goto exit;
	}

#endif

	/* save yuv */
	strcat(input.file, ".yuv");
	file = fopen(input.file, "wb");
	if (file == NULL) {
		printf("open file:%s failed\n", input.file);
		goto exit;
	}


	count = fwrite(output.virt_addr[0], 1, output.size[0], file);
	if (count != output.size[0]) {
		printf("write failed, count(%d) != output.size(%d)\n",
			count, output.size[0]);
		goto exit;
	}
	count = fwrite(output.virt_addr[1], 1, output.size[1], file);
	if (count != output.size[1]) {
		printf("write failed, count(%d) != output.size(%d)\n",
			count, output.size[1]);
		goto exit;
	}

exit:

	recg_img_dec_end(&output);
	if (data) {
		free(data);
		data = NULL;
	}
	if (fp) {
		fclose(fp);
		fp = NULL;
	}
	if (file) {
		fclose(file);
		file = NULL;
	}

	return 0;
}

